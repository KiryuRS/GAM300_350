#pragma once
#include "Component.h"
#include "EntityWrapper.h"

class TransformC;

namespace
{
	struct ViewportInfo
	{
		Vector2 xy;
		int width, height;

		ViewportInfo()
			: xy{ 0,0 },
			  width{ 1 }, height{ 1 }
		{ }
	};

	struct ClippingPlanesInfo
	{
		float near_plane, far_plane;

		ClippingPlanesInfo()
			: near_plane{ 0.01f },
			  far_plane{ 7000.f }
		{ }
	};
}

class CameraC : public Component
{
	Vector2 old_mouse_position;		// For rotation purpose
	Vector3 position;				// World position
	Vector3 old_displacement;		// Displacement between the camera and the target (if there is)
	Entity* target;					// Camera to fix the target at
	ViewportInfo vp_info;			// Viewport information (to determine how the camera should be setup)
	ClippingPlanesInfo cp_info;		// Clipping plane information (View Fustrum)
	float field_of_view;			// Determine how near or how far the camera should be zoomed (z-axis?)
	bool orthographic;				// Determine if the camera should be orthographic
	bool perspective;				// Determine if the camera should be perspective
	bool trans_pos_active;			// Using the transformC's position instead
	bool free_will_cam;				// Determine a free will camera movement or targetted based
	bool target_rotation;			// Rotation based on an arbitary target
	std::string target_name;		// Target's identifier's name
	std::unique_ptr<Camera> camera;
	float old_y_movement;			// the old position of mouse.y for movement
	float camera_speed;				// Speed of the camera on free-will
	float camera_rotation;			// Rotation speed
	int mouse_scroll;				// Determine the scrolling amount for the camera movement
	float original_camera_speed;	// The original camera speed (for free-will movement)
	friend struct GameSpace;
	float shakeTimer = 0.f;
	float shakeAmount;
	Vector3 shakeAdjust;
	TransformC* transform;

public:
	CameraC();

	void Initialize() override;
	void Update(float dt) override;
	void AddSerializeData(LuaScript* state = nullptr) override;
	void SetTarget(std::string identity_name);
	void SetPosition(Vector3 _pos);
	void SetViewportInfo(Vector2 _x_y, int _width, int _height);
	void SetClippingPlaneInfo(float _near, float _far);
	void SetFieldOfView(float fov);
	void EditorUpdate(float) override;

	EntityWrapper GetTarget() const;
	Vector3 GetPosition() const;
	float GetFOV() const;
	Camera* GetCamera() const			{ return camera.get(); }
	Vector3 GetCameraPosition() const;
	Vector3 GetTargetPosition() const;
	Vector3 GetUp() const;

	void SetCameraPosition(Vector3 pos);
	void SetCameraTarget(Vector3 pos);
	void SetCameraIsPerspective();
	void SetCameraIsOrthographics();
	void SetFarDist(float f);
	void SetNearDist(float f);
	Vector4 M4Rotate(Vector4 v4, float f, Vector3 v3);
	void CameraShake(float amount, float duration);
	void UpdateCameraShake(float dt);
	static constexpr auto GetType()		{ return COMPONENTTYPE::CAMERA; }
};