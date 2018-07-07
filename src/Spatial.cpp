#include "Spatial.h"

godot_object *Spatial_new()
{
	return api->godot_get_class_constructor((char *)"Spatial")();
}

void Spatial_set_transform(godot_object *p_this, const godot_transform local) {
	static godot_method_bind *mb = nullptr;
	if (mb == nullptr) {
		mb = api->godot_method_bind_get_method("Spatial", "set_transform");
	}
	___godot_icall_void_Transform(mb, p_this, local);
}

void Spatial_show(godot_object *p_this) {
	static godot_method_bind *mb = nullptr;
	if (mb == nullptr) {
		mb = api->godot_method_bind_get_method("Spatial", "show");
	}
	___godot_icall_void(mb, p_this);
}

void Spatial_hide(godot_object *p_this) {
	static godot_method_bind *mb = nullptr;
	if (mb == nullptr) {
		mb = api->godot_method_bind_get_method("Spatial", "hide");
	}
	___godot_icall_void(mb, p_this);
}