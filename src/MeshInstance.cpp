////////////////////////////////////////////////////////////////////////////////////////////////
// Just exposing an interface to the ArrayMesh functions reachable in GDNative
// Basically just ported some of the functions in the cpp_bindings for use in C

// Note, even though this is pure C code, we're using the C++ compiler as
// Microsoft never updated their C compiler to understand more modern dialects
// and openvr uses pesky things such as namespaces

#include "MeshInstance.h"

godot_object *MeshInstance_new() {
	return api->godot_get_class_constructor((char *)"MeshInstance")();
};

void MeshInstance_set_mesh(godot_object *p_this, godot_object *mesh) {
	static godot_method_bind *mb = nullptr;
	if (mb == nullptr) {
		mb = api->godot_method_bind_get_method("MeshInstance", "set_mesh");
	}
	___godot_icall_void_Object(mb, p_this, mesh);
}