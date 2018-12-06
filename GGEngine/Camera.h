#pragma once
/*****************************************************************************/
/*!
\file Camera.h
\author Zhou Zhihua, zhihua.z, 390001016
\par zhihua.z\@digipen.edu
\date August 12, 2017
\brief

This file is a class for camera control in the engine

Copyright (C) 2017 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/*****************************************************************************/

class Camera
{
public:
  Camera();

  void CheckRecalculate();
  void MakeDirty();

  void SetPosition(float x, float y, float z);
  void SetPosition(const Vector3& v);
  Vector3 GetPosition() const;
  void SetTarget(float x, float y, float z);
  void SetTarget(const Vector3& v);
  Vector3 GetTarget() const;
  void SetUp(float x, float y, float z);
  void SetUp(const Vector3& v);
  Vector3 GetUp() const;
  Vector3 GetRight() const;
  Vector3 GetFront() const;
  Matrix4x4 GetPV() const;
  Matrix4x4* GetProjection();
  Matrix4x4* GetView();
  void SetCameraIsOrthographics();
  void SetCameraIsPerspective();
  void SetNearDist(float near_plane);
  void SetFarDist(float far_plane);
  float GetDistance() const;
  void SetDistance(float f);
  float GetFOV() { return FOV; }
  float GetNearDist() { return nearPlane; }
  float GetFarDist() { return farPlane; }
  bool IsOrthographic() const;

  void ChangeResolution(int x, int y);
  void CopySettings(const Camera&);

  float hdrExposure;
  float gamma;
  bool bloom;
  bool hdr;
  int bloomAmount;

private:
  float camera_distance = { 10.0f };
  Vector3 position;
  Vector3 target;
  Vector3 up;
  Vector3 front;
  Vector3 right;
  float nearPlane, farPlane;

  int x_resolution = { 1500 };
  int y_resolution = { 843 };

  float         _camera_speed;
  double        _mousex, _mousey;
  Vector2     _old_mouse_pos;
  int           _rbuttonstate;

  bool is_orthographics = { false };
  float FOV;
  float camera_zoom;
  Matrix4x4 projection;
  Matrix4x4 view;
  Matrix4x4 pv;
  bool camera_dirty = { true };
};