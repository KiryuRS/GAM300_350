#include "stdafx.h"
#include "SkeletalMesh.h"

void SkeletalMesh::calculate_transformation(double anim_time)
{
	nodeCount = 0;
	gBoneTransformations.clear();
	gBoneTransformations.resize(bones.size());
	gmTranslation = Matrix4x4(1.0f);
	gmRotation = Matrix4x4(1.0f);
	gmScaling = Matrix4x4(1.0f);
	gmTransformation = Matrix4x4(1.0f);
	leavePreTransCalc = false;

	Matrix4x4 identity = Matrix4x4();
	if(nodeMatrices.size() > 0)
		calculate_trans_aux(anim_time, root_node, identity);
}

void SkeletalMesh::calculate_trans_aux(double anim_time, Node& n, const Matrix4x4 & parentTrans)
{
	Matrix4x4 final_trans = nodeMatrices[nodeCount];
	auto iter = animations[animationNum].channels.find(n.name);
	if (iter != animations[animationNum].channels.end())
	{
		Channel& c = iter->second;
		Matrix4x4 pos = interpolate_pos(anim_time, c.positionKeys);
		Matrix4x4 rot = interpolate_rot(anim_time, c.rotationKeys);
		Matrix4x4 scale = interpolate_scale(anim_time, c.scalingKeys);
		auto transform = pos * rot * scale;
		final_trans = parentTrans * transform;
		if (bones.find(n.name) != bones.end())
			gBoneTransformations[mBoneIDMapping[n.name]] = mGlobalInverseTransform * final_trans * bones[n.name].offset_matrix;
	}
	else
	{
		final_trans = parentTrans * nodeMatrices[nodeCount];
		if (bones.find(n.name) != bones.end())
			gBoneTransformations[mBoneIDMapping[n.name]] = mGlobalInverseTransform * final_trans * bones[n.name].offset_matrix;

	}
	nodeCount++;
	for (auto& elem : n.children)
		calculate_trans_aux(anim_time, elem, final_trans);
}

Matrix4x4 SkeletalMesh::interpolate_pos(double anim_time, const std::vector<VectorKey> & pos_key)
{
	if (pos_key.empty())
		return Matrix4x4(1.0f);

	for (size_t i = 0; i < pos_key.size() - 1; ++i)
	{
		// this frame animation p0
		if (anim_time < pos_key[i + 1].time_stamp)
		{
			double t0 = pos_key[i].time_stamp;
			double t1 = pos_key[i + 1].time_stamp;
			double t = (anim_time - t0) / (t1 - t0);

			const Vector3& q0 = pos_key[i].value;
			const Vector3& q1 = pos_key[i + 1].value;
			Vector3 finalpos = q0 + (q1 - q0) * static_cast<float>(t);

			return Matrix4x4({ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, finalpos.x, finalpos.y, finalpos.z, 1.0f });
		}
	}

	return Matrix4x4();
}

Matrix4x4 SkeletalMesh::interpolate_rot(double anim_time, const std::vector<QuatKey> & rot_key)
{
	if (rot_key.empty())
		return Matrix4x4(1.0f);

	for (size_t i = 0; i < rot_key.size() - 1; ++i)
	{
		// this frame animation p0
		if (anim_time < rot_key[i + 1].time_stamp)
		{
			double t0 = rot_key[i].time_stamp;
			double t1 = rot_key[i + 1].time_stamp;
			double t = (anim_time - t0) / (t1 - t0);
			Vector4 quad = Vector4::QuaternionInterpolate(rot_key[i].value, rot_key[i + 1].value, (float)t);
			quad.Normalize();
			return Matrix3x3::GetQuatMatrix(quad);
		}
	}

	return Matrix4x4();
}

Matrix4x4 SkeletalMesh::interpolate_scale(double anim_time, const std::vector<VectorKey> & scale_key)
{
	if (scale_key.empty())
		return Matrix4x4(1.0f);

	for (size_t i = 0; i < scale_key.size() - 1; ++i)
	{
		// this frame animation p0
		if (anim_time < scale_key[i + 1].time_stamp)
		{
			double t0 = scale_key[i].time_stamp;
			double t1 = scale_key[i + 1].time_stamp;
			double t = (anim_time - t0) / (t1 - t0);

			const Vector3& q0 = scale_key[i].value;
			const Vector3& q1 = scale_key[i + 1].value;
			Vector3 finalscale = q0 + (q1 - q0) * static_cast<float>(t);

			return Matrix4x4(finalscale.x, 0, 0, 0, 0, finalscale.y, 0, 0, 0, 0, finalscale.z, 0, 0, 0, 0, 1);
		}
	}

	return Matrix4x4();
}

void SkeletalMesh::fix_bone_id_mapping()
{
	int current_pos = 0;
	for (auto itr = bones.begin(); itr != bones.end(); ++itr)
	{
		mBoneIDMapping[itr->second.name] = current_pos;
		current_pos++;
	}
}

std::pair<Vector3, Vector3> SkeletalMesh::GetMinMax() const
{
	std::pair<Vector3, Vector3> minMax;

	return std::pair<Vector3, Vector3>();
}