////////////////////////////////////////////////////////////////////////////////////////////////
// Just exposing an interface to the Image functions reachable in GDNative
// Basically just ported some of the functions in the cpp_bindings for use in C

#ifndef NODE_H
#define NODE_H

#include "GodotCalls.h"


#ifdef __cplusplus
extern "C" {
#endif

	godot_object *Nodes_new();
	void Node_set_name(godot_object *p_this, const godot_string *name);
	godot_string Node_get_name(godot_object *p_this);
	void Node_add_child(godot_object *p_this, const godot_object *node, const bool legible_unique_name = false);
	void Node_remove_child(godot_object *p_this, const godot_object *node);
	int64_t Node_get_child_count(godot_object *p_this);
	godot_object *Node_get_child(godot_object *p_this, const int64_t idx);
	void Node_queue_free(godot_object *p_this);
	bool Node_is_inside_tree(godot_object *p_this);

#ifdef __cplusplus
}
#endif

#endif /* !NODE_H */