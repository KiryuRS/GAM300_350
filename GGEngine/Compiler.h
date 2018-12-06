#pragma once
#include <string>
#include <map>
#include "Serializable.h"

template<typename T>
struct ThreadsafeQueue;

enum class COMPILERMSG {
	COMPILEFILE
};

struct CompilerMessage
{
	COMPILERMSG type;
	std::string directory = "";
};

struct Extensioninfo
{
	std::string extension;
	std::string folder;
	void(*compileFunc)(const std::string&, const std::string&);
};

void CompileFBX(const std::string& source, const std::string& destination);
void CompileObj(const std::string& source, const std::string& destination);
void CompileMdl(const std::string& source, const std::string& destination);
#ifdef EDITOR
void ConvertImageFile(const std::string& source, const std::string& destination);
#endif

//the compiler compiles any files obtained from file checker into our own custom format
class Compiler
{
	std::unique_ptr<ThreadsafeQueue<CompilerMessage>> messageQueue;
	static const std::unordered_map<std::string, Extensioninfo> extensionDirectory;

public:
	std::atomic<bool> running;
	std::atomic<bool> compilingAssets;

	Compiler();
	void Update();
	std::string CompileFile(const std::string& source);
	static std::string GetMappedDirectory(const std::string&);
};


struct sBone : public Serializable
{
	sBone()
	{
		AddSerializable("Name", name);
		AddSerializable("WeightIDs", weightID);
		AddSerializable("Weights", weights);
		AddSerializable("Offset Matrix", offset_matrix);
		AddSerializable("Final Transform", finalTransformation);
	}
	sBone(const sBone& bb) :
		name(bb.name), weightID(bb.weightID), weights(bb.weights),
		offset_matrix(bb.offset_matrix), finalTransformation(bb.finalTransformation)
	{
		AddSerializable("Name", name);
		AddSerializable("WeightIDs", weightID);
		AddSerializable("Weights", weights);
		AddSerializable("Offset Matrix", offset_matrix);
		AddSerializable("Final Transform", finalTransformation);
	}
	std::string name;
	std::vector<int> weightID;
	std::vector<float> weights;
	Matrix4x4 offset_matrix;
	Matrix4x4 finalTransformation;
	bool operator!=(const sBone&) const
	{
		return true;
	}
	bool operator==(const sBone&) const
	{
		return false;
	}
};

struct sChannel : public Serializable
{
	std::string name;
	std::vector<float> pk_timestamp;
	std::vector<Vector3> pk_value;
	std::vector<float> sk_timestamp;
	std::vector<Vector3> sk_value;
	std::vector<float> rk_timestamp;
	std::vector<Vector4> rk_value;
	sChannel()
	{
		AddSerializable("Name", name);
		AddSerializable("pk_timestamp", pk_timestamp);
		AddSerializable("pk_value", pk_value);
		AddSerializable("sk_value", sk_value);
		AddSerializable("sk_timestamp", sk_timestamp);
		AddSerializable("rk_timestamp", rk_timestamp);
		AddSerializable("rk_value", rk_value);
	}
	sChannel(const sChannel& sc):
		name(sc.name), pk_timestamp(sc.pk_timestamp),
		pk_value(sc.pk_value), sk_value(sc.sk_value),
		sk_timestamp(sc.sk_timestamp), rk_timestamp(sc.rk_timestamp),
		rk_value(sc.rk_value)
	{
		AddSerializable("Name", name);
		AddSerializable("pk_timestamp", pk_timestamp);
		AddSerializable("pk_value", pk_value);
		AddSerializable("sk_value", sk_value);
		AddSerializable("sk_timestamp", sk_timestamp);
		AddSerializable("rk_timestamp", rk_timestamp);
		AddSerializable("rk_value", rk_value);
	}
	bool operator!=(const sChannel&) const
	{
		return true;
	}
	bool operator==(const sChannel&) const
	{
		return false;
	}
};

struct sAnimation : public Serializable
{
	std::string name;
	float duration;
	float ticks_per_sec;
	std::vector<sChannel> channels;
	sAnimation()
	{
		AddSerializable("Name", name);
		AddSerializable("duration", duration);
		AddSerializable("ticks_per_sec", ticks_per_sec);
		AddSerializable("channels", channels);
	}
	sAnimation(const sAnimation& sa):
		name(sa.name), duration(sa.duration),
		ticks_per_sec(sa.ticks_per_sec), channels(sa.channels)
	{
		AddSerializable("Name", name);
		AddSerializable("duration", duration);
		AddSerializable("ticks_per_sec", ticks_per_sec);
		AddSerializable("channels", channels);
	}
	bool operator!=(const sAnimation&) const
	{
		return true;
	}
	bool operator==(const sAnimation&) const
	{
		return false;
	}
};

struct SerialMesh: public Serializable
{
	std::vector<Vector3> vertices;
	std::vector<Vector3> normals;
	std::vector<Vector2> uvs;
	std::vector<int> faces;
	std::vector<sBone> bones;
	std::vector<int> boneMappingNum;
	std::vector<std::string> boneMappingName;
	Matrix4x4 transform;
	bool globalTransformExist;
	std::string gTranslationName, gPreRotationName, gRotationName, gScalingName;

	Node root_node;
	std::vector<Matrix4x4> nodeMatrices;
	SerialMesh()
	{
		AddSerializable("Vertices",vertices);
		AddSerializable("Normals", normals);
		AddSerializable("UVs", uvs);
		AddSerializable("Faces", faces);
		AddSerializable("Transform", transform);
		AddSerializable("Bones", bones);
		AddSerializable("GlobalExists", globalTransformExist);
		AddSerializable("Translation Name", gTranslationName);
		AddSerializable("Pre Rotation Name", gPreRotationName);
		AddSerializable("Rotation Name", gRotationName);
		AddSerializable("Scaling Name", gScalingName);
		AddSerializable("Bone Mapping Name", boneMappingName);
		AddSerializable("Bone Mapping Num", boneMappingNum);
		AddSerializable("RootNode", root_node);
		AddSerializable("Node Matrices", nodeMatrices);

	}
	SerialMesh(const SerialMesh& mm):
		vertices(mm.vertices), normals(mm.normals), uvs(mm.uvs), faces(mm.faces), bones(mm.bones), globalTransformExist(mm.globalTransformExist),
		gTranslationName(mm.gTranslationName), gPreRotationName(mm.gPreRotationName),
		gScalingName(mm.gScalingName), boneMappingName(mm.boneMappingName), boneMappingNum(mm.boneMappingNum),
		root_node(mm.root_node), transform(mm.transform), gRotationName(mm.gRotationName), nodeMatrices(mm.nodeMatrices)
	{
		AddSerializable("Vertices", vertices);
		AddSerializable("Normals", normals);
		AddSerializable("UVs", uvs);
		AddSerializable("Faces", faces);
		AddSerializable("Transform", transform);
		AddSerializable("Bones", bones);
		AddSerializable("GlobalExists", globalTransformExist);
		AddSerializable("Translation Name", gTranslationName);
		AddSerializable("Pre Rotation Name", gPreRotationName);
		AddSerializable("Rotation Name", gRotationName);
		AddSerializable("Scaling Name", gScalingName);
		AddSerializable("Bone Mapping Name", boneMappingName);
		AddSerializable("Bone Mapping Num", boneMappingNum);
		AddSerializable("RootNode", root_node);
		AddSerializable("Node Matrices", nodeMatrices);
	}
	bool operator!=(const SerialMesh&) const
	{
		return true;
	}
	bool operator==(const SerialMesh&) const
	{
		return false;
	}
};

struct MeshCompiler : public Serializable
{
	std::vector<SerialMesh> sMeshes;
	Matrix4x4 transform;
	std::vector<sAnimation> animations;
	MeshCompiler()
	{
		AddSerializable("StaticMeshes", sMeshes);
		AddSerializable("GlobalTransform", transform);
		AddSerializable("animations", animations);
	}
	MeshCompiler(const MeshCompiler& sm):
		sMeshes(sm.sMeshes), transform(sm.transform),
		animations(sm.animations)
	{
		AddSerializable("StaticMeshes", sMeshes);
		AddSerializable("GlobalTransform", transform);
		AddSerializable("animations", animations);
	}

};