#pragma once

#define POINT_LIGHT 0
#define DIRECTIONAL_LIGHT 1
#define SPOT_LIGHT 2

struct Light
{
  Vector3 pos;
  float intensity = { 1.0f };
  Vector4 ambient;
  Vector4 diffuse;
  Vector4 specular;
  Vector3 dir;
  float radius = { 40.0f };
  int light_type;
  float inner_cos;
  float outer_cos;
  float falloff;
};

struct Light_container
{
	static constexpr unsigned MAX_LIGHTS = 32;
  bool HasSpace() const;
  bool Empty() const;
  unsigned find(Light* l) const;

  Light* mValue[MAX_LIGHTS];
  unsigned mCount;
};