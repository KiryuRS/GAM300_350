/*****************************************************************************/
/*!
\file Animation.h
\author Zhou Zhihua
\par zhihua.z\@digipen.edu
\date September 8, 2018
\brief

Contains data to display an animation

Copyright (C) 2017 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/*****************************************************************************/
#pragma once
#include <string>
#include <vector>
#include "Vector.h"

struct aiNode;
struct sAnimation;

struct VectorKey
{
  VectorKey() : time_stamp{ 0.0 }, value{ } {}
  VectorKey(double t, const Vector3& keys) : time_stamp{ t }, value{ Vector3(keys.x, keys.y, keys.z) } {}
  double time_stamp;
  Vector3 value;
};

struct QuatKey
{
  QuatKey() : time_stamp{ 0.0 }, value{ } {}
  QuatKey(double t, const Vector4& keys) : time_stamp{ t }, value{ keys } {}
  double time_stamp;
  Vector4 value;
};

struct Channel
{
  Channel() : name{}, positionKeys{}, rotationKeys{}, scalingKeys{} {}

  std::string name;
  std::vector<VectorKey> positionKeys;
  std::vector<QuatKey> rotationKeys;
  std::vector<VectorKey> scalingKeys;
};

struct Node
{
  Node() : name{}, children{} {}
  ~Node();

  int mMappingIndex = { 0 };
  std::string name;
  std::vector<Node> children;
  bool operator!=(const Node&) const
  {
	  return true;
  }
};


struct Animation
{
  Animation() : name{}, duration{ 0.0 }, ticks_per_sec{ 0.0 }, channels{ std::map<std::string, Channel>() }{}
  Animation(const sAnimation& anim);

  ~Animation();


  std::string name;
  double duration;
  double ticks_per_sec;
  std::map<std::string, Channel> channels;
};