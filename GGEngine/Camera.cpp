/*****************************************************************************/
/*!
\file Camera.cpp
\author Zhou Zhihua
\par zhihua.z\@digipen.edu
\date August 31, 2017
\brief

Implements the camera object, recalculation of camera members
 - up 
 - right
 - front 
 - view
 - projection : orthographic or perspective

Copyright (C) 2017 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/*****************************************************************************/

#include "stdafx.h"
#include "Camera.h"
#include "ext.hpp"
#include "Vector.h"

Camera::Camera()
  : position(0.0f, 0.0f, 10.0f),
  target(0.0f, 0.0f, 0.0f),
  up(0.0f, 1.0f, 0.0f),
  FOV(45.0f),
  camera_zoom{ 1.0f },
  projection{ Matrix4x4::Perspective(FOV, static_cast<float>(x_resolution) / y_resolution, 5.0f, 7000.0f) },
  view { Matrix4x4::LookAt(Vector3(position.x, position.y, position.z),
	Vector3(target.x, target.y, target.z),
	Vector3(0.0f, 1.0, 0.0f)) },
	nearPlane(5.0f), farPlane(7000.0f),
	hdrExposure{1.0f},
	gamma{ 2.2f },
	bloomAmount{ 10 }, hdr{ false }, bloom{false}
{
  front = target - position;
  front.Normalize();
  right = front ^ up;
}

void Camera::CheckRecalculate()
{
  if (camera_dirty)
  {
	  front = target - position;
	  front.Normalize();
	  right = front ^ up;
	  right.Normalize();

	  //x_resolution = COREENGINE_S.GetWindow().GetScreenWidth();
	  //y_resolution = COREENGINE_S.GetWindow().GetScreenHeight();

	  if (is_orthographics)
	  {
		  float f_resx = (float)x_resolution;
		  float f_resy = (float)y_resolution;
		  projection = Matrix4x4::Ortho(
			  -f_resx * 0.5f * camera_zoom,
			  f_resx * 0.5f * camera_zoom,
			  -f_resy * 0.5f * camera_zoom,
			  f_resy * 0.5f * camera_zoom, 5.0f, 10000.0f);
	  }
	  else
	    projection = Matrix4x4::Perspective(FOV, static_cast<float>(x_resolution) / y_resolution, nearPlane, farPlane);

	  view = Matrix4x4::LookAt(Vector3(position.x, position.y, position.z),
	  	Vector3(target.x, target.y, target.z),
	  	Vector3(0.0f, 1.0, 0.0f));
	  
	  pv = projection * view;

	  camera_dirty = false;
  }
}

void Camera::MakeDirty()
{
  camera_dirty = true;
}

void Camera::SetPosition(float x, float y, float z)
{
  position = Vector3(x, y, z);
  MakeDirty();
}

void Camera::SetPosition(const Vector3 & v)
{
  position = v;
  MakeDirty();
}

Vector3 Camera::GetPosition() const
{
  return position;
}

void Camera::SetTarget(float x, float y, float z)
{
  target = Vector3(x, y, z);
  MakeDirty();
}

void Camera::SetTarget(const Vector3 & v)
{
  target = v;
  MakeDirty();
}

Vector3 Camera::GetTarget() const
{
  return target;
}

void Camera::SetUp(float x, float y, float z)
{
  up = Vector3(x, y, z);
}

void Camera::SetUp(const Vector3 & v)
{
  up = v;
}

Vector3 Camera::GetUp() const
{
  return up;
}

Vector3 Camera::GetRight() const
{

  return right;
}

Vector3 Camera::GetFront() const
{
  return front;
}

Matrix4x4 Camera::GetPV() const
{
  return pv;
}

Matrix4x4* Camera::GetProjection()
{
	return &projection;
}

Matrix4x4* Camera::GetView()
{
	return &view;
}

void Camera::SetCameraIsOrthographics()
{
  is_orthographics = true;
  CheckRecalculate();
  MakeDirty();
}

void Camera::SetCameraIsPerspective()
{
  is_orthographics = false;
  CheckRecalculate();
  MakeDirty();
}

void Camera::SetNearDist(float near_plane)
{
	nearPlane = near_plane;
	// Update the camera
  MakeDirty();
	CheckRecalculate();
}

void Camera::SetFarDist(float far_plane)
{
	farPlane = far_plane;
	// Update the camera
  MakeDirty();
	CheckRecalculate();
}

float Camera::GetDistance() const
{
  return camera_distance;
}

void Camera::SetDistance(float f)
{
  Vector3 v = target - position;
  v.Normalize();
  v *= f;
  position = target - v;
  MakeDirty();
}

bool Camera::IsOrthographic() const
{
	return is_orthographics;
}

void Camera::ChangeResolution(int x, int y)
{
	x_resolution = x;
	y_resolution = y;
  MakeDirty();
  CheckRecalculate();
}

void Camera::CopySettings(const Camera & cam)
{
	hdrExposure = cam.hdrExposure;
	gamma = cam.gamma;
	bloom = cam.bloom;
	hdr = cam.hdr;
	bloomAmount = cam.bloomAmount;
}