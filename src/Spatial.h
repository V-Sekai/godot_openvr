////////////////////////////////////////////////////////////////////////////////////////////////
// Just exposing an interface to the Image functions reachable in GDNative
// Basically just ported some of the functions in the cpp_bindings for use in C

#ifndef SPATIAL_H
#define SPATIAL_H

#include "GodotCalls.h"


#ifdef __cplusplus
extern "C" {
#endif

godot_object *Spatial_new();
void Spatial_set_transform(godot_object *p_this, const godot_transform local);
void Spatial_show(godot_object *p_this);
void Spatial_hide(godot_object *p_this);

#ifdef __cplusplus
}
#endif

#endif /* !SPATIAL_H */