#pragma once
#include <vector>
#include <unordered_map>
#include "Animation.h"

struct Mesh;
struct Animation;
struct Bone;

struct SkeletalMesh
{
	bool loaded = false;
	std::vector<Mesh> meshes;
	std::unordered_map<std::string, Bone> bones;
	std::vector<Animation> animations;
	Vector3 min, max;

	Node root_node;
	std::vector<Matrix4x4> nodeMatrices;
	int nodeCount = 0;
	unsigned gBoneTranxSize = { 0 };
	Matrix4x4 mGlobalInverseTransform;
	bool globalTransformExist = { false };
	bool leavePreTransCalc = { false };
	int animationNum = 0;

	std::string gTranslationName;
	std::string gPreRotationName;
	std::string gRotationName;
	std::string gScalingName;
	Matrix4x4 gmTranslation = { Matrix4x4(1.0f) };
	Matrix4x4 gmRotation = { Matrix4x4(1.0f) };
	Matrix4x4 gmScaling = { Matrix4x4(1.0f) };
	Matrix4x4 gmTransformation = { Matrix4x4(1.0f) };
	//not needed to save
	std::vector<Matrix4x4> gBoneTransformations;
	std::unordered_map<std::string, unsigned> mBoneIDMapping;
  std::vector<Material*> materials;

	void calculate_transformation(double anim_time);
	void calculate_trans_aux(double anim_time, Node& n, const Matrix4x4& parentTrans);
	Matrix4x4 interpolate_pos(double anim_time, const std::vector<VectorKey>& pos_key);
	Matrix4x4 interpolate_rot(double anim_time, const std::vector<QuatKey>& rot_key);
	Matrix4x4 interpolate_scale(double anim_time, const std::vector<VectorKey>& scale_key);
	void fix_bone_id_mapping();
	std::pair<Vector3, Vector3> GetMinMax() const;
};
