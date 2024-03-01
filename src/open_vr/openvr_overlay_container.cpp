#include "openvr_overlay_container.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/rendering_device.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/sub_viewport.hpp>
#include <godot_cpp/classes/viewport_texture.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void OpenVROverlayContainer::_bind_methods() {
	// ClassDB::bind_method(D_METHOD("_ready"), &OpenVROverlayContainer::_ready);
	// ClassDB::bind_method(D_METHOD("_exit_tree"), &OpenVROverlayContainer::_exit_tree);

	ClassDB::bind_method(D_METHOD("is_overlay_visible"), &OpenVROverlayContainer::is_overlay_visible);
	ClassDB::bind_method(D_METHOD("set_overlay_visible", "visible"), &OpenVROverlayContainer::set_overlay_visible);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "overlay_visible"), "set_overlay_visible", "is_overlay_visible");

	ClassDB::bind_method(D_METHOD("get_overlay_width_in_meters"), &OpenVROverlayContainer::get_overlay_width_in_meters);
	ClassDB::bind_method(D_METHOD("set_overlay_width_in_meters", "width"), &OpenVROverlayContainer::set_overlay_width_in_meters);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "overlay_width_in_meters"), "set_overlay_width_in_meters", "get_overlay_width_in_meters");

	ClassDB::bind_method(D_METHOD("track_relative_to_device"), &OpenVROverlayContainer::track_relative_to_device);
	ClassDB::bind_method(D_METHOD("overlay_position_absolute"), &OpenVROverlayContainer::overlay_position_absolute);
}

OpenVROverlayContainer::OpenVROverlayContainer() {
	ovr = openvr_data::retain_singleton();
	overlay_width_in_meters = 1.0;
	overlay_visible = true;
	overlay = 0;
}

OpenVROverlayContainer::~OpenVROverlayContainer() {
	if (ovr != nullptr) {
		ovr->release();
		ovr = nullptr;
	}
}

void OpenVROverlayContainer::_ready() {
	if (Engine::get_singleton()->is_editor_hint()) {
		return;
	}

	String appname = ProjectSettings::get_singleton()->get_setting("application/config/name");
	String overlay_identifier = appname + String::num(ovr->get_overlay_count() + 1);

	const char *overlay_key = overlay_identifier.utf8().get_data();
	const char *overlay_name = overlay_key;

	vr::EVROverlayError vrerr = vr::VROverlay()->CreateOverlay(overlay_key, overlay_name, &overlay);
	if (vrerr != vr::VROverlayError_None) {
		Array arr;
		arr.push_back(String::num(vrerr));
		arr.push_back(String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));
		UtilityFunctions::print(String("Could not create overlay, OpenVR error: {0}, {1}").format(arr));
	}

	// Tie our new overlay to this container so that events can make it back here later.
	overlay_id = ovr->add_overlay(overlay, get_instance_id());

	// TODO: Use the position of this container in a 3d scene, if it has one.
	Transform3D initial_transform;
	initial_transform = initial_transform.translated(Vector3(0, 0, 1) * -1.4);

	overlay_position_absolute(initial_transform);
	set_overlay_width_in_meters(overlay_width_in_meters);
	set_overlay_visible(overlay_visible);
}

void OpenVROverlayContainer::_exit_tree() {
	if (overlay) {
		vr::EVROverlayError vrerr = vr::VROverlay()->DestroyOverlay(overlay);
		if (vrerr != vr::VROverlayError_None) {
			Array arr;
			arr.push_back(String::num(vrerr));
			arr.push_back(String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));
			UtilityFunctions::print(String("Could not destroy overlay, OpenVR error: {0}, {1}").format(arr));
			return;
		}

		ovr->remove_overlay(overlay_id);
		overlay_id = 0;
		overlay = vr::k_ulOverlayHandleInvalid;
	}
}

void OpenVROverlayContainer::draw_overlay(const Ref<Texture2D> &p_texture) {
	if (overlay == vr::k_ulOverlayHandleInvalid) {
		return;
	}

	RenderingServer *rendering_server = RenderingServer::get_singleton();
	ERR_FAIL_NULL(rendering_server);
	RenderingDevice *rendering_device = rendering_server->get_rendering_device();
	ERR_FAIL_NULL(rendering_device);

	RID texture_rid = rendering_server->texture_get_rd_texture(p_texture->get_rid());
	uint64_t image = rendering_device->get_driver_resource(RenderingDevice::DRIVER_RESOURCE_VULKAN_IMAGE, texture_rid, 0);
	uint32_t format = rendering_device->get_driver_resource(RenderingDevice::DRIVER_RESOURCE_VULKAN_IMAGE_NATIVE_TEXTURE_FORMAT, texture_rid, 0);

	if (image == 0 || format == 0) {
		return;
	}

	Size2 size = get_size();

	vr::VRTextureBounds_t bounds;
	bounds.uMin = 0.0f;
	bounds.uMax = 1.0f;
	bounds.vMin = 0.0f;
	bounds.vMax = 1.0f;

	vr::EVROverlayError vrerr;

	vr::VRVulkanTextureData_t vulkan_data;
	vulkan_data.m_pDevice = (VkDevice_T *)rendering_device->get_driver_resource(RenderingDevice::DRIVER_RESOURCE_VULKAN_DEVICE, RID(), 0);
	vulkan_data.m_pPhysicalDevice = (VkPhysicalDevice_T *)rendering_device->get_driver_resource(RenderingDevice::DRIVER_RESOURCE_VULKAN_PHYSICAL_DEVICE, RID(), 0);
	vulkan_data.m_pInstance = (VkInstance_T *)rendering_device->get_driver_resource(RenderingDevice::DRIVER_RESOURCE_VULKAN_INSTANCE, RID(), 0);
	vulkan_data.m_pQueue = (VkQueue_T *)rendering_device->get_driver_resource(RenderingDevice::DRIVER_RESOURCE_VULKAN_QUEUE, RID(), 0);
	vulkan_data.m_nQueueFamilyIndex = (uint32_t)rendering_device->get_driver_resource(RenderingDevice::DRIVER_RESOURCE_VULKAN_QUEUE_FAMILY_INDEX, RID(), 0);

	vulkan_data.m_nImage = image;
	vulkan_data.m_nFormat = format;
	vulkan_data.m_nWidth = size.width;
	vulkan_data.m_nHeight = size.height;
	vulkan_data.m_nSampleCount = 0;

	vr::Texture_t overlay_texture = { &vulkan_data, vr::TextureType_Vulkan, vr::ColorSpace_Gamma };

	vrerr = vr::VROverlay()->SetOverlayTexture(overlay, &overlay_texture);

	if (vrerr != vr::VROverlayError_None) {
		UtilityFunctions::printerr(String("OpenVR could not set texture for overlay: ") + String::num_int64(vrerr) + String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));
		return;
	}

	vrerr = vr::VROverlay()->SetOverlayTextureBounds(overlay, &bounds);

	if (vrerr != vr::VROverlayError_None) {
		UtilityFunctions::printerr(String("OpenVR could not set textute bounds for overlay: ") + String::num_int64(vrerr) + String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));
		return;
	}
}

void OpenVROverlayContainer::_notification(int p_what) {
	// TODO: I don't really know C++ and have no idea if I'm overriding this correctly or if there's a better way, but I couldn't seem to
	// hijack draw_texture because it's not virtual.
	SubViewportContainer::_notification(p_what);

	switch (p_what) {
		case NOTIFICATION_DRAW: {
			// TODO: We _just_ did this in the superclass, see if we can reuse that result somehow
			for (int i = 0; i < get_child_count(); i++) {
				SubViewport *c = Object::cast_to<SubViewport>(get_child(i));
				if (!c) {
					continue;
				}
				// TODO: Do we need stretch here?
				draw_overlay(c->get_texture());
			}
		} break;
	}
}

float OpenVROverlayContainer::get_overlay_width_in_meters() {
	if (overlay) {
		vr::VROverlay()->GetOverlayWidthInMeters(overlay, &overlay_width_in_meters);
	}
	return overlay_width_in_meters;
}

void OpenVROverlayContainer::set_overlay_width_in_meters(real_t p_new_size) {
	overlay_width_in_meters = p_new_size;

	if (overlay) {
		vr::EVROverlayError vrerr = vr::VROverlay()->SetOverlayWidthInMeters(overlay, p_new_size);

		if (vrerr != vr::VROverlayError_None) {
			Array arr;
			arr.push_back(String::num(vrerr));
			arr.push_back(String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));
			UtilityFunctions::print(String("Could not set overlay width in meters, OpenVR error: {0}, {1}").format(arr));
		}
	}
}

bool OpenVROverlayContainer::is_overlay_visible() {
	if (overlay) {
		overlay_visible = vr::VROverlay()->IsOverlayVisible(overlay);
	}
	return overlay_visible;
}

void OpenVROverlayContainer::set_overlay_visible(bool p_visible) {
	overlay_visible = p_visible;

	if (overlay) {
		if (p_visible) {
			vr::EVROverlayError vrerr = vr::VROverlay()->ShowOverlay(overlay);

			if (vrerr != vr::VROverlayError_None) {
				Array arr;
				arr.push_back(String::num(vrerr));
				arr.push_back(String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));
				UtilityFunctions::print(String("Could not show overlay, OpenVR error: {0}, {1}").format(arr));
			}
		} else {
			vr::EVROverlayError vrerr = vr::VROverlay()->HideOverlay(overlay);

			if (vrerr != vr::VROverlayError_None) {
				Array arr;
				arr.push_back(String::num(vrerr));
				arr.push_back(String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));
				UtilityFunctions::print(String("Could not hide overlay, OpenVR error: {0}, {1}").format(arr));
			}
		}
	}
}

bool OpenVROverlayContainer::track_relative_to_device(vr::TrackedDeviceIndex_t p_tracked_device_index, Transform3D p_transform) {
	if (overlay) {
		XRServer *server = XRServer::get_singleton();
		double ws = server->get_world_scale();
		vr::HmdMatrix34_t matrix;

		ovr->matrix_from_transform(&matrix, &p_transform, ws);

		vr::EVROverlayError vrerr = vr::VROverlay()->SetOverlayTransformTrackedDeviceRelative(overlay, p_tracked_device_index, &matrix);

		if (vrerr != vr::VROverlayError_None) {
			Array arr;
			arr.push_back(String::num(vrerr));
			arr.push_back(String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));
			UtilityFunctions::print(String("Could not track overlay relative to device, OpenVR error: {0}, {1}").format(arr));

			return false;
		}

		return true;
	}
	return false;
}

bool OpenVROverlayContainer::overlay_position_absolute(Transform3D p_transform) {
	if (overlay) {
		XRServer *server = XRServer::get_singleton();
		double ws = server->get_world_scale();
		vr::HmdMatrix34_t matrix;
		vr::TrackingUniverseOrigin origin;

		ovr->matrix_from_transform(&matrix, &p_transform, ws);

		openvr_data::OpenVRTrackingUniverse tracking_universe = ovr->get_tracking_universe();
		if (tracking_universe == openvr_data::OpenVRTrackingUniverse::SEATED) {
			origin = vr::TrackingUniverseSeated;
		} else if (tracking_universe == openvr_data::OpenVRTrackingUniverse::STANDING) {
			origin = vr::TrackingUniverseStanding;
		} else {
			origin = vr::TrackingUniverseRawAndUncalibrated;
		}

		vr::EVROverlayError vrerr = vr::VROverlay()->SetOverlayTransformAbsolute(overlay, origin, &matrix);

		if (vrerr != vr::VROverlayError_None) {
			Array arr;
			arr.push_back(String::num(vrerr));
			arr.push_back(String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));
			UtilityFunctions::print(String("Could not track overlay absolute, OpenVR error: {0}, {1}").format(arr));

			return false;
		}

		return true;
	}
	return false;
}