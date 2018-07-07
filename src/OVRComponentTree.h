////////////////////////////////////////////////////////////////////////////////////////////////
// GDNative module that exposes the render models available in OpenVR as resources to Godot

#ifndef OVR_COMPONENT_TREE_H
#define OVR_COMPONENT_TREE_H

#include "GodotCalls.h"
#include "OVRCalls.h"

#ifdef __cplusplus
extern "C" {
#endif

GDCALLINGCONV void *openvr_component_tree_constructor(godot_object *p_instance, void *p_method_data);
GDCALLINGCONV void openvr_component_tree_destructor(godot_object *p_instance, void *p_method_data, void *p_user_data);
GDCALLINGCONV godot_variant openvr_component_tree_load(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);
GDCALLINGCONV godot_variant openvr_component_tree_update(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);

#ifdef __cplusplus
}
#endif

#endif /* !OVR_COMPONENT_TREE_H */
