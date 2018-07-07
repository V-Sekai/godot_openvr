////////////////////////////////////////////////////////////////////////////////////////////////
// GDNative module that exposes the render models available in OpenVR as resources to Godot

#ifndef OVR_RENDER_MODEL_H
#define OVR_RENDER_MODEL_H

#include "GodotCalls.h"
#include "OVRCalls.h"

bool render_model_exists(openvr_data_struct *ovr, godot_string *render_model_name);
bool load_render_model(openvr_data_struct *ovr, godot_string *render_model_name, godot_object *p_array_mesh, godot_dictionary *p_material_dictionary);

#ifdef __cplusplus
extern "C" {
#endif

GDCALLINGCONV void *openvr_render_model_constructor(godot_object *p_instance, void *p_method_data);
GDCALLINGCONV void openvr_render_model_destructor(godot_object *p_instance, void *p_method_data, void *p_user_data);
GDCALLINGCONV godot_variant openvr_render_model_list(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);
GDCALLINGCONV godot_variant openvr_render_model_load(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);

#ifdef __cplusplus
}
#endif

#endif /* !OVR_RENDER_MODEL_H */
