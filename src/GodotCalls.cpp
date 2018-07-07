////////////////////////////////////////////////////////////////////////////////////////////////
// Just exposing an interface to a few callback functions on Godot objects
// Basically just ported some of the functions in the cpp_bindings for use in C

// Note, even though this is pure C code, we're using the C++ compiler as
// Microsoft never updated their C compiler to understand more modern dialects
// and openvr uses pesky things such as namespaces

#include "GodotCalls.h"

const godot_gdnative_core_api_struct *api = NULL;
const godot_gdnative_ext_arvr_api_struct *arvr_api = NULL;
const godot_gdnative_ext_nativescript_api_struct *nativescript_api = NULL;

void GDN_EXPORT godot_openvr_gdnative_init(godot_gdnative_init_options *p_options) {
	// get our main API struct
	api = p_options->api_struct;

	// now find our arvr extension
	for (int i = 0; i < api->num_extensions; i++) {
		// todo: add version checks
		switch (api->extensions[i]->type) {
			case GDNATIVE_EXT_ARVR: {
				if (api->extensions[i]->version.major > 1 || (api->extensions[i]->version.major == 1 && api->extensions[i]->version.minor >= 1)) {
					arvr_api = (godot_gdnative_ext_arvr_api_struct *)api->extensions[i];
				} else {
					printf("ARVR API version %i.%i isn't supported, need version 1.1 or higher\n", api->extensions[i]->version.major, api->extensions[i]->version.minor);
				}
			}; break;
			case GDNATIVE_EXT_NATIVESCRIPT: {
				if (api->extensions[i]->version.major > 1 || (api->extensions[i]->version.major == 1 && api->extensions[i]->version.minor >= 0)) {
					nativescript_api = (godot_gdnative_ext_nativescript_api_struct *)api->extensions[i];
				} else {
					printf("Native script API version %i.%i isn't supported, need version 1.1 or higher\n", api->extensions[i]->version.major, api->extensions[i]->version.minor);
				}
			}; break;
			default: break;
		};
	};
}

void GDN_EXPORT godot_openvr_gdnative_terminate(godot_gdnative_terminate_options *p_options) {
	api = NULL;
	nativescript_api = NULL;
	arvr_api = NULL;
}

void ___godot_icall_void(godot_method_bind *mb, godot_object *inst) {
	const void *args[1] = {};

	api->godot_method_bind_ptrcall(mb, inst, args, nullptr);
}

bool ___godot_icall_bool(godot_method_bind *mb, godot_object *inst) {
	bool ret;
	const void *args[1] = {};

	api->godot_method_bind_ptrcall(mb, inst, args, &ret);
	return ret;
}

int64_t ___godot_icall_int(godot_method_bind *mb, godot_object *inst) {
	int64_t ret;
	const void *args[1] = {
	};

	api->godot_method_bind_ptrcall(mb, inst, args, &ret);
	return ret;
}

void ___godot_icall_void_int(godot_method_bind *mb, godot_object *inst, const int arg0) {
	const void *args[] = {
		(void *) &arg0,
	};

	api->godot_method_bind_ptrcall(mb, inst, args, nullptr);
}

void ___godot_icall_void_int_Array_Array_int(godot_method_bind *mb, godot_object *inst, const int arg0, const godot_array& arg1, const godot_array& arg2, const int arg3) {
	const void *args[] = {
		(void *) &arg0,
		(void *) &arg1,
		(void *) &arg2,
		(void *) &arg3,
	};

	api->godot_method_bind_ptrcall(mb, inst, args, nullptr);
}

void ___godot_icall_void_int_Object(godot_method_bind *mb, godot_object *inst, const int arg0, const godot_object *arg1) {
	const void *args[] = {
		(void *) &arg0,
		(void *) arg1,
	};

	api->godot_method_bind_ptrcall(mb, inst, args, nullptr);
}

void ___godot_icall_void_Color(godot_method_bind *mb, godot_object *inst, const godot_color& arg0) {
	const void *args[] = {
		(void *) &arg0,
	};

	api->godot_method_bind_ptrcall(mb, inst, args, nullptr);
}

void ___godot_icall_void_Transform(godot_method_bind *mb, godot_object *inst, const godot_transform &arg0) {
	const void *args[] = {
		(void *)&arg0,
	};

	api->godot_method_bind_ptrcall(mb, inst, args, nullptr);
}

void ___godot_icall_void_String(godot_method_bind *mb, godot_object *inst, const godot_string *arg0) {
	const void *args[] = {
		(void *)arg0,
	};

	api->godot_method_bind_ptrcall(mb, inst, args, nullptr);
}

void ___godot_icall_void_Object(godot_method_bind *mb, godot_object *inst, const godot_object *arg0) {
	const void *args[] = {
		(void *)arg0,
	};

	api->godot_method_bind_ptrcall(mb, inst, args, nullptr);
}

void ___godot_icall_void_Object_bool(godot_method_bind *mb, godot_object *inst, const godot_object *arg0, const bool arg1) {
	const void *args[] = {
		(void *)arg0,
		(void *)&arg1,
	};

	api->godot_method_bind_ptrcall(mb, inst, args, nullptr);
}

void ___godot_icall_void_Object_int(godot_method_bind *mb, godot_object *inst, const godot_object *arg0, const int arg1) {
	const void *args[] = {
		(void *) arg0,
		(void *) &arg1,
	};

	api->godot_method_bind_ptrcall(mb, inst, args, nullptr);
}

void ___godot_icall_void_int_int_bool_int_PoolByteArray(godot_method_bind *mb, godot_object *inst, const int arg0, const int arg1, const bool arg2, const int arg3, const godot_pool_byte_array *arg4) {
	const void *args[] = {
		(void *) &arg0,
		(void *) &arg1,
		(void *) &arg2,
		(void *) &arg3,
		(void *) arg4,
	};

	api->godot_method_bind_ptrcall(mb, inst, args, nullptr);
}

godot_vector2 ___godot_icall_Vector2_int(godot_method_bind *mb, godot_object *inst, const int arg0) {
	godot_vector2 ret;
	const void *args[] = {
		(void *) &arg0,
	};

	api->godot_method_bind_ptrcall(mb, inst, args, &ret);
	return ret;
}

godot_string ___godot_icall_String(godot_method_bind *mb, godot_object *inst) {
	godot_string ret;
	const void *args[1] = {};

	api->godot_method_bind_ptrcall(mb, inst, args, &ret);
	return ret;
}

godot_object *___godot_icall_Object_int(godot_method_bind *mb, godot_object *inst, const int arg0) {
	godot_object *ret;
	ret = nullptr;
	const void *args[] = {
		(void *)&arg0,
	};

	api->godot_method_bind_ptrcall(mb, inst, args, &ret);
	return ret;
}