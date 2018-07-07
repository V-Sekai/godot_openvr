#include "Node.h"

godot_object *Nodes_new() {
	return api->godot_get_class_constructor((char *)"Node")();
};

void Node_set_name(godot_object *p_this, const godot_string *name) {
	static godot_method_bind *mb = nullptr;
	if (mb == nullptr) {
		mb = api->godot_method_bind_get_method("Node", "set_name");
	}
	___godot_icall_void_String(mb, p_this, name);
}

godot_string Node_get_name(godot_object *p_this) {
	static godot_method_bind *mb = nullptr;
	if (mb == nullptr) {
		mb = api->godot_method_bind_get_method("Node", "get_name");
	}
	return ___godot_icall_String(mb, p_this);
}

void Node_add_child(godot_object *p_this, const godot_object *node, const bool legible_unique_name) {
	static godot_method_bind *mb = nullptr;
	if (mb == nullptr) {
		mb = api->godot_method_bind_get_method("Node", "add_child");
	}
	___godot_icall_void_Object_bool(mb, p_this, node, legible_unique_name);
}

void Node_remove_child(godot_object *p_this, const godot_object *node) {
	static godot_method_bind *mb = nullptr;
	if (mb == nullptr) {
		mb = api->godot_method_bind_get_method("Node", "remove_child");
	}
	___godot_icall_void_Object(mb, p_this, node);
}

int64_t Node_get_child_count(godot_object *p_this) {
	static godot_method_bind *mb = nullptr;
	if (mb == nullptr) {
		mb = api->godot_method_bind_get_method("Node", "get_child_count");
	}
	return ___godot_icall_int(mb, p_this);
}

godot_object *Node_get_child(godot_object *p_this, const int64_t idx) {
	static godot_method_bind *mb = nullptr;
	if (mb == nullptr) {
		mb = api->godot_method_bind_get_method("Node", "get_child");
	}
	return ___godot_icall_Object_int(mb, p_this, idx);
}

void Node_queue_free(godot_object *p_this) {
	static godot_method_bind *mb = nullptr;
	if (mb == nullptr) {
		mb = api->godot_method_bind_get_method("Node", "queue_free");
	}
	___godot_icall_void(mb, p_this);
}

bool Node_is_inside_tree(godot_object *p_this) {
	static godot_method_bind *mb = nullptr;
	if (mb == nullptr) {
		mb = api->godot_method_bind_get_method("Node", "is_inside_tree");
	}
	return ___godot_icall_bool(mb, p_this);
}
