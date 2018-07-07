////////////////////////////////////////////////////////////////////////////////////////////////
// Just exposing an interface to the ArrayMesh functions reachable in GDNative
// Basically just ported some of the functions in the cpp_bindings for use in C

#ifndef MESH_INSTANCE_H
#define MESH_INSTANCE_H

#include "GodotCalls.h"

#ifdef __cplusplus
extern "C" {
#endif

	godot_object *MeshInstance_new();
	void MeshInstance_set_mesh(godot_object *p_this, godot_object *mesh);

#ifdef __cplusplus
}
#endif


#endif