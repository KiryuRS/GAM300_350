#include "stdafx.h"
#include "ext.hpp"

CameraC::CameraC()
	: position{ 0,0,10.f },
	  target{ nullptr }, vp_info{ },
	  cp_info{ }, field_of_view{ 45 }, perspective{ true },
	  orthographic{ false }, trans_pos_active{ true }, camera{ std::make_unique<Camera>() },
	  camera_speed{ 0.7f }, target_rotation{ false }, free_will_cam{ true }, original_camera_speed{ 0 },
	  camera_rotation{ 2.f }, mouse_scroll{ 0 }
{
	RequiredComponents(transform);
}

void CameraC::Initialize()
{
	// Using the position set by the transformC instead
	if (trans_pos_active)
	{
		TransformC* comp_trans = owner->GetComponent<TransformC>();
		const Vector3& curr_pos = comp_trans->GetPosition();
		SetPosition(curr_pos);
		camera->SetPosition(curr_pos);
	}

	// By default we will find the player in the scene and append our camera to it
	if (target_name.size())
	{
		Entity *player = Component::entityList->FindEntity(target_name);
		if (player)
		{
			target = player;
			// Update the target position
			const Vector3& target_pos = target->GetComponent<TransformC>()->GetPosition();
			camera->SetTarget(target_pos);
			old_displacement = target_pos - position;
		}
	}
	else if (!free_will_cam)
		free_will_cam = true;

	// Set the camera settings
	camera->SetFarDist(cp_info.far_plane);
	camera->SetNearDist(cp_info.near_plane);
  camera->MakeDirty();
	if (perspective)
		camera->SetCameraIsPerspective();
	else if (orthographic)
		camera->SetCameraIsOrthographics();

	original_camera_speed = camera_speed;
}

void CameraC::Update(float dt)
{
  camera->SetFarDist(cp_info.far_plane);
  camera->SetNearDist(cp_info.near_plane);
  UpdateCameraShake(dt);
  transform->UpdateTransformationMatrix();
  camera->SetPosition(transform->GetGlobalPosition() + shakeAdjust);
  camera->MakeDirty();
}

void CameraC::AddSerializeData(LuaScript * state)
{
	// Serializable stuff for our camera
	if (!owner->GetComponent<TransformC>())
	{
		AddSerializable("Position", position, "", state);
		trans_pos_active = false;
	}
	AddSerializable("Camera_Speed", camera_speed, "", state);
	AddSerializable("Camera_Rotation", camera_rotation, "", state);
	AddSerializable("Clipping_Plane_Near", cp_info.near_plane, "Near Plane value", state);
	AddSerializable("Clipping_Plane_Far", cp_info.far_plane, "Far Plane value", state);
	AddSerializable("Field_Of_View", field_of_view, "", state);
	AddSerializable("Viewport_Width", vp_info.width, "Width of the Viewport", state);
	AddSerializable("Viewport_Height", vp_info.height, "Height of the Viewport", state);
	AddSerializable("Viewport_Start_Position", vp_info.xy, "", state);
	AddSerializable("Disable", Component::disabled, "To disable the camera during first run", state);
	AddSerializable("Perspective_View", perspective, "Please tick either perspective or orthographic view", state);
	AddSerializable("Orthographic_View", orthographic, "Please tick either perspective or orthographic view", state);
	AddSerializable("Free_Will_Camera", free_will_cam, "Determine if the camera should be free will or target based", state);
	AddSerializable("Target_Name_Identifier", target_name, "The target's identifier's name", state);
	AddSerializable("HDR_Exposure_Rate", camera->hdrExposure, "",state);
	AddSerializable("Camera_Gamma", camera->gamma, "", state);
	AddSerializable("Camera_Bloom", camera->bloom, "", state);
	AddSerializable("Bloom_Amount", camera->bloomAmount, "", state);
	AddSerializable("HDR", camera->hdr, "", state);


	// Callable functions
	AddCallableFunction("SetTarget", &CameraC::SetTarget, {}, state);
	AddCallableFunction("SetPosition", &CameraC::SetPosition, {}, state);
	AddCallableFunction("SetViewportInfo", &CameraC::SetViewportInfo, {}, state);
	AddCallableFunction("SetClippingPlaneInfo", &CameraC::SetClippingPlaneInfo, {}, state);
	AddCallableFunction("SetFieldOfView", &CameraC::SetFieldOfView, {}, state);
	AddCallableFunction("GetFieldOfView", &CameraC::GetFOV, state);
	AddCallableFunction("GetPosition", &CameraC::GetPosition, state);
	AddCallableFunction("GetTarget", &CameraC::GetTarget, state);
	AddCallableFunction("GetCameraPosition", &CameraC::GetCameraPosition, state);
	AddCallableFunction("GetTargetPosition", &CameraC::GetTargetPosition, state);
	AddCallableFunction("GetUp", &CameraC::GetUp, state);
	AddCallableFunction("SetCameraPosition", &CameraC::SetCameraPosition, state);
	AddCallableFunction("SetCameraTarget", &CameraC::SetCameraTarget, state);
	AddCallableFunction("SetCameraIsPerspective", &CameraC::SetCameraIsPerspective, state);
	AddCallableFunction("SetCameraIsOrthographics", &CameraC::SetCameraIsOrthographics, state);
	AddCallableFunction("SetFarDist", &CameraC::SetFarDist, state);
	AddCallableFunction("SetNearDist", &CameraC::SetNearDist, state);
	AddCallableFunction("M4Rotate", &CameraC::M4Rotate, state);
	AddCallableFunction("CameraShake", &CameraC::CameraShake, {"Intensity", "Duration"}, state);
	// Note: Names of any variable or function cannot contain spaces!
}

void CameraC::SetTarget(std::string identifier_name)
{
	// Search through the target in the entity_list
	Entity* _targ = Component::entityList->FindEntity(identifier_name);
	if (!_targ)
	{
		CONSOLE_ERROR("Invalid Identifier passed in! Unable to assign a target!");
		return;
	}
	// Set the target to the camera
	target = _targ;
}

void CameraC::SetPosition(Vector3 _pos)
{
	if (trans_pos_active)
		owner->GetComponent<TransformC>()->SetPosition(_pos);
	position = _pos;
}

void CameraC::SetViewportInfo(Vector2 _x_y, int _width, int _height)
{
	vp_info.xy = _x_y;
	vp_info.width = _width;
	vp_info.height = _height;
}

void CameraC::SetClippingPlaneInfo(float _near, float _far)
{
	cp_info.near_plane = _near;
	cp_info.far_plane = _far;
}

void CameraC::SetFieldOfView(float fov)
{
	field_of_view = fov;
}

void CameraC::EditorUpdate(float)
{
	/*UpdateComponentData(camera->hdrExposure);
	UpdateComponentData(camera->gamma);*/

  camera->SetFarDist(cp_info.far_plane);
  camera->SetNearDist(cp_info.near_plane);
  camera->MakeDirty();
}

EntityWrapper CameraC::GetTarget() const
{
	return target;
}

Vector3 CameraC::GetPosition() const
{
	return trans_pos_active ? owner->GetComponent<TransformC>()->GetPosition() : position;
}

float CameraC::GetFOV() const
{
	return field_of_view;
}

Vector3 CameraC::GetCameraPosition() const
{
	return camera->GetPosition();
}

Vector3 CameraC::GetTargetPosition() const
{
	return camera->GetTarget();
}

Vector3 CameraC::GetUp() const
{
	return camera->GetUp();
}

void CameraC::SetCameraPosition(Vector3 pos)
{
	camera->SetPosition(pos);
}

void CameraC::SetCameraTarget(Vector3 pos)
{
	camera->SetTarget(pos);
}

void CameraC::SetCameraIsPerspective()
{
	camera->SetCameraIsPerspective();
}

void CameraC::SetCameraIsOrthographics()
{
	camera->SetCameraIsOrthographics();
}

void CameraC::SetFarDist(float f)
{
	camera->SetFarDist(f);
}

void CameraC::SetNearDist(float f)
{
	camera->SetNearDist(f);
}

Vector4 CameraC::M4Rotate(Vector4 v4, float f, Vector3 v3)
{
	return Matrix4x4::Rotate(v4, f, v3);
}

void CameraC::CameraShake(float amount, float duration)
{
	shakeTimer = duration;
	shakeAmount = amount;
}


void CameraC::UpdateCameraShake(float dt)
{
	if (shakeTimer > 0)
	{
		//The multiplied number represents how frequently
		//you want the direction of the shake to change.
		int switchcase = (int)(shakeTimer * 99);
		switch (switchcase % 8)
		{
		case 0:
			shakeAdjust.x += shakeAmount;
			shakeAdjust.y += shakeAmount;
			shakeAdjust.z += shakeAmount;
			break;
		case 1:
			shakeAdjust.x -= shakeAmount;
			shakeAdjust.y -= shakeAmount;
			shakeAdjust.z -= shakeAmount;
			break;
		case 2:
			shakeAdjust.x += shakeAmount;
			shakeAdjust.y -= shakeAmount;
			shakeAdjust.z -= shakeAmount;
			break;
		case 3:
			shakeAdjust.x += shakeAmount;
			shakeAdjust.y += shakeAmount;
			shakeAdjust.z -= shakeAmount;
			break;
		case 4:
			shakeAdjust.x += shakeAmount;
			shakeAdjust.y -= shakeAmount;
			shakeAdjust.z += shakeAmount;
			break;
		case 5:
			shakeAdjust.x -= shakeAmount;
			shakeAdjust.y -= shakeAmount;
			shakeAdjust.z += shakeAmount;
			break;
		case 6:
			shakeAdjust.x -= shakeAmount;
			shakeAdjust.y += shakeAmount;
			shakeAdjust.z += shakeAmount;
			break;
		case 7:
			shakeAdjust.x -= shakeAmount;
			shakeAdjust.y += shakeAmount;
			shakeAdjust.z -= shakeAmount;
			break;

		}
		shakeTimer -= dt;
	}
	else
	{
		shakeAdjust = { 0, 0, 0 };
		shakeTimer = 0;
	}
}