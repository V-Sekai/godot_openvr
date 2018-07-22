////////////////////////////////////////////////////////////////////////////////////////////////
// GDNative module that exposes the render models available in OpenVR as resources to Godot

// Note, even though this is pure C code, we're using the C++ compiler as
// Microsoft never updated their C compiler to understand more modern dialects
// and openvr uses pesky things such as namespaces

#include "OVRComponentTree.h"
#include "OVRRenderModel.h"

#include "ArrayMesh.h"
#include "MeshInstance.h"
#include "Node.h"
#include "Spatial.h"

const char *attachment_node_name = "attach";

typedef struct component_tree_data_struct {
	openvr_data_struct *ovr;
	godot_string render_model_name_real;
	godot_string *render_model_name;
	uint32_t device_index;
} component_tree_data_struct;

GDCALLINGCONV void *openvr_component_tree_constructor(godot_object *p_instance, void *p_method_data) {
	component_tree_data_struct *component_tree_data;

	// log just for testing, should remove this
	printf("Creating component tree resource\n");

	component_tree_data = (component_tree_data_struct *)api->godot_alloc(sizeof(component_tree_data_struct));
	if (component_tree_data != NULL) {
		component_tree_data->ovr = openvr_get_data();
		component_tree_data->render_model_name = NULL;
		component_tree_data->device_index = -1;
	}

	return component_tree_data;
}

GDCALLINGCONV void openvr_component_tree_destructor(godot_object *p_instance, void *p_method_data, void *p_user_data) {
	// log just for testing, should remove this
	printf("Releasing component tree resource\n");

	if (p_user_data != NULL) {
		component_tree_data_struct *component_tree_data = (component_tree_data_struct *)p_user_data;

		if (component_tree_data->ovr != NULL) {
			openvr_release_data();
			component_tree_data->ovr = NULL;
		}

		if (component_tree_data->render_model_name != NULL) {
			api->godot_string_destroy(&component_tree_data->render_model_name_real);
			component_tree_data->render_model_name = NULL;
		}
	}
}

GDCALLINGCONV godot_variant openvr_component_tree_load(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {
	godot_variant ret;
	bool loaded = false;

	// clear out all children
	int64_t child_count = Node_get_child_count(p_instance);
	for (int i = 0; i < child_count; i++) {
		godot_object *child = Node_get_child(p_instance, 0);
		if (child) {
			Node_queue_free(child);
			Node_remove_child(p_instance, child);
		}
	};

	// get our model from openvr
	if (p_user_data != NULL) {
		component_tree_data_struct *component_tree_data = (component_tree_data_struct *)p_user_data;

		printf("Argument count: %i\n", p_num_args);
		if ((component_tree_data->ovr != NULL) && (p_num_args > 0)) {
			// Destroy the previous string if it is already loaded
			if (component_tree_data->render_model_name != NULL) {
				api->godot_string_destroy(&component_tree_data->render_model_name_real);
				component_tree_data->render_model_name = NULL;
			}
			component_tree_data->render_model_name_real = api->godot_variant_as_string(p_args[0]);
			component_tree_data->render_model_name = &component_tree_data->render_model_name_real;

			if (render_model_exists(component_tree_data->ovr, component_tree_data->render_model_name)) {
				godot_char_string render_model_cs = api->godot_string_ascii(component_tree_data->render_model_name);
				const char *render_model_name_ptr = api->godot_char_string_get_data(&render_model_cs);

				uint32_t component_count = component_tree_data->ovr->render_models->GetComponentCount(render_model_name_ptr);
				if (component_count > 0) {

					godot_string attachment_name = api->godot_string_chars_to_utf8(attachment_node_name);

					godot_dictionary material_dictionary;
					api->godot_dictionary_new(&material_dictionary);
					for (int i = 0; i < component_count; i++) {

						uint32_t capacity = component_tree_data->ovr->render_models->GetComponentName(render_model_name_ptr, i, NULL, 0);
						if (capacity == 0) {
							continue;
						}

						char *component_name_ptr = (char *)api->godot_alloc(capacity);
						component_tree_data->ovr->render_models->GetComponentName(render_model_name_ptr, i, component_name_ptr, capacity);

						// Set up the name
						godot_string component_name;
						api->godot_string_new(&component_name);
						api->godot_string_parse_utf8_with_len(&component_name, component_name_ptr, strlen(component_name_ptr));

						godot_object *spatial = NULL;

						capacity = component_tree_data->ovr->render_models->GetComponentRenderModelName(render_model_name_ptr, component_name_ptr, NULL, 0);
						if (capacity == 0) {
							spatial = Spatial_new();
						} else {
							char *component_render_model_string_ptr = (char *)api->godot_alloc(capacity);
							if (component_tree_data->ovr->render_models->GetComponentRenderModelName(render_model_name_ptr, component_name_ptr, component_render_model_string_ptr, capacity) == 0) {
								// ERROR
								api->godot_string_destroy(&component_name);
								api->godot_free(component_name_ptr);
								api->godot_free(component_render_model_string_ptr);
								continue;
							}

							godot_string component_render_model_string;
							api->godot_string_new(&component_render_model_string);
							api->godot_string_parse_utf8_with_len(&component_render_model_string, component_render_model_string_ptr, strlen(component_render_model_string_ptr));
							api->godot_free(component_render_model_string_ptr);

							godot_object *array_mesh = NULL;
							array_mesh = ArrayMesh_new();

							if (load_render_model(component_tree_data->ovr, &component_render_model_string, array_mesh, &material_dictionary) == true) {
								spatial = MeshInstance_new();
								MeshInstance_set_mesh(spatial, array_mesh);
							} else {
								api->godot_object_destroy(array_mesh);
							}
							api->godot_string_destroy(&component_render_model_string);
						}

						Node_set_name(spatial, &component_name);
						Node_add_child(p_instance, spatial);

						// Create attachment
						godot_object *attachment = NULL;
						attachment = Spatial_new();

						Node_set_name(attachment, &attachment_name);
						Node_add_child(spatial, attachment);

						// Cleanup
						api->godot_string_destroy(&component_name);
						api->godot_free(component_name_ptr);
					}

					api->godot_string_destroy(&attachment_name);
					api->godot_dictionary_destroy(&material_dictionary);
					loaded = true;
				} else {
					godot_object *array_mesh = NULL;
					array_mesh = ArrayMesh_new();

					if (load_render_model(component_tree_data->ovr, component_tree_data->render_model_name, array_mesh, NULL) == true) {
						godot_object *mesh_instance = NULL;
						mesh_instance = MeshInstance_new();
						MeshInstance_set_mesh(mesh_instance, array_mesh);

						Node_set_name(mesh_instance, component_tree_data->render_model_name);
						Node_add_child(p_instance, mesh_instance);
						loaded = true;
					} else {
						api->godot_object_destroy(array_mesh);
					}
				}

				api->godot_char_string_destroy(&render_model_cs);

			}
		};
		godot_char_string gg = api->godot_string_ascii(component_tree_data->render_model_name); // Why doesn't this work?
		const char *vvv = api->godot_char_string_get_data(&gg);
	};

	api->godot_variant_new_bool(&ret, loaded);
	return ret;
};

GDCALLINGCONV godot_variant openvr_component_tree_update(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {
	godot_variant ret;

	if (p_user_data != NULL) {
		component_tree_data_struct *component_tree_data = (component_tree_data_struct *)p_user_data;

		godot_string *render_model_name = component_tree_data->render_model_name;

		if (render_model_name) {
			godot_char_string render_model_cs = api->godot_string_ascii(render_model_name); // Why doesn't this work?
			const char *render_model_name_ptr = api->godot_char_string_get_data(&render_model_cs);

			if (Node_is_inside_tree(p_instance)) {

				vr::VRControllerState_t controller_state;
				if (component_tree_data->device_index != -1) {
					//component_tree_data->ovr->hmd->GetControllerState(component_tree_data->device_index, &controller_state, sizeof(vr::VRControllerState_t));
				}

				int64_t child_count = Node_get_child_count(p_instance);
				for (int i = 0; i < child_count; i++) {
					godot_object *child = NULL;
					child = Node_get_child(p_instance, i);
					if (child == NULL) {
						continue;
					};

					godot_string node_name = Node_get_name(child);

					uint32_t capacity = component_tree_data->ovr->render_models->GetComponentName(render_model_name_ptr, i, NULL, 0);
					if (capacity != 0) {
						vr::RenderModel_ControllerMode_State_t controller_mode_state;
						vr::RenderModel_ComponentState_t component_state;

						godot_char_string node_name_cs = api->godot_string_ascii(&node_name);
						if (component_tree_data->ovr->render_models->GetComponentState(render_model_name_ptr, api->godot_char_string_get_data(&node_name_cs), &controller_state, &controller_mode_state, &component_state) == true) {
							godot_transform component_render_model_transform;

							//api->godot_transform_new_identity(&component_render_model_transform);
							//openvr_transform_from_matrix(&component_render_model_transform, &component_state.mTrackingToComponentRenderModel, 1.0f);

							//Spatial_set_transform(child, component_render_model_transform);

							if (Node_get_child_count(child) > 0) {
								godot_object *child_attachment = NULL;
								child_attachment = Node_get_child(child, 0);
								if (child_attachment != NULL) {
									godot_transform component_local_transform;
									api->godot_transform_new_identity(&component_local_transform);

									openvr_transform_from_matrix(&component_local_transform, &component_state.mTrackingToComponentLocal, 1.0f);
									Spatial_set_transform(child_attachment, component_local_transform);
								}
							}

							if (component_state.uProperties & vr::VRComponentProperty_IsVisible) {
								Spatial_show(child);
							} else {
								Spatial_hide(child);
							}
						};
						api->godot_char_string_destroy(&node_name_cs);
					};

					api->godot_string_destroy(&node_name);
				};
			};
			api->godot_char_string_destroy(&render_model_cs);
		};
	};

	api->godot_variant_new_nil(&ret);
	return ret;
}

GDCALLINGCONV godot_variant openvr_component_set_device_index(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {
	godot_variant ret;

	if (p_user_data != NULL) {
		component_tree_data_struct *component_tree_data = (component_tree_data_struct *)p_user_data;
		if (p_num_args > 0) {
			component_tree_data->device_index = api->godot_variant_as_int(p_args[0]);
		}
	}

	api->godot_variant_new_nil(&ret);
	return ret;
}