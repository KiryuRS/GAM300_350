#pragma once
#include <string>
#include <unordered_map>
#include <future>
#include <optional>
#include "glew.h"
#include "Compiler.h"
struct SkeletalMesh;

template<typename T>
std::unique_ptr<T> MultithreadedLoadMesh(const std::string& ss);

template<typename T>
class BaseMeshLoader
{
	const std::string defaultMesh = "compiledassets/models/platform.mdl";
	std::vector<std::string> meshLoadQueue;
	std::unordered_map<std::string, T> m_meshManager;
	std::thread loadResources;
	std::packaged_task<std::unique_ptr<T>(const std::string&)> task;
	std::future<std::unique_ptr<T>> future;
	void BindMesh()
	{
		auto ptr = future.get();
		if (ptr.get())
		{
			m_meshManager[loadingMesh.value()] = *ptr;
			for (auto& mesh : m_meshManager[loadingMesh.value()].meshes)
			{
				mesh.OpenGLBinding();
			}
			m_meshManager[loadingMesh.value()].loaded = true;
		}
		else
		{
			TOOLS::CreateErrorMessageBox("Could not find model ", loadingMesh.value());
		}
	}
	void LoadNewMesh()
	{
		if (!loadingMesh && meshLoadQueue.size())
		{
			task = std::packaged_task< std::unique_ptr<T>(const std::string&)>
			( &MultithreadedLoadMesh<T>);
			future = task.get_future();
			loadResources = std::thread(std::move(task), meshLoadQueue.front());
			loadingMesh = meshLoadQueue.front();
			meshLoadQueue.erase(meshLoadQueue.begin());
		}
		else if (loadingMesh)
		{
			if (!future.valid())
			{
				CONSOLE_LN(loadingMesh.value(), " has an invalid future!");
				loadingMesh.reset();
			}
			else
			{
				auto status = future.wait_for(std::chrono::seconds(0));
				if (status == std::future_status::ready)
				{
					loadResources.join();
					BindMesh();
					loadingMesh.reset();
				}

			}
		}
	}
public:
	std::optional<std::string> loadingMesh;
	BaseMeshLoader()
	{
		GetMesh(defaultMesh);
		LoadNewMesh();
		JoinThread();
	}
	void Update(){

		LoadNewMesh();
	}
	void JoinThread()
	{
		loadResources.join();
		BindMesh();
		loadingMesh.reset();
	}
	void RecompileFile(const std::string& str) {
		auto newPath = Compiler::GetMappedDirectory(str);
		if (m_meshManager.find(newPath) == m_meshManager.end())
		{
			return;
		}
		meshLoadQueue.emplace_back(newPath);
		m_meshManager[newPath] = m_meshManager[defaultMesh];
	}
	T& GetMesh(const std::string& path) {
		auto newPath = Compiler::GetMappedDirectory(path);
		if (m_meshManager.find(newPath) == m_meshManager.end())
		{
			meshLoadQueue.emplace_back(newPath);
			auto& tex = m_meshManager[defaultMesh];
			UNREFERENCED_PARAMETER(tex);
			m_meshManager[newPath] = m_meshManager[defaultMesh];
			m_meshManager[newPath].loaded = false;
		}
		return m_meshManager[newPath];
	}

	~BaseMeshLoader()
	{
		if (loadingMesh)
			loadResources.join();
	}
};


template<typename T>
std::unique_ptr<T> MultithreadedLoadMesh(const std::string& ss) {

	Serializer serial;
	serial.LoadFile(ss);
	MeshCompiler mMeshes;
	auto child = serial.FirstChild();
	if (!child)
		return std::unique_ptr<T>();
	mMeshes.ConvertSerializedData(*child);

	auto m = std::make_unique<T>();
	T& newMesh = *m;
	size_t num_mesh = mMeshes.sMeshes.size();

	if constexpr (std::is_same<T, SkeletalMesh>())
	{
		newMesh.mGlobalInverseTransform = mMeshes.transform;
		newMesh.mGlobalInverseTransform = Matrix4x4::Inverse(newMesh.mGlobalInverseTransform);
	}
	newMesh.meshes.resize(num_mesh);
  unsigned boneNum = 0;

	for (size_t nm = 0; nm < num_mesh; ++nm)
	{
		auto& mesh = mMeshes.sMeshes[nm];

#pragma region NORMAL_UV_PRESENT
		// normal and uv already generated
		if (mesh.normals.size() && mesh.uvs.size())
		{
			// add vertices
			for (unsigned int i = 0; i < mesh.vertices.size(); ++i)
			{
				newMesh.meshes[nm].AddVertex(
					Vertex(
						mesh.vertices[i].x, mesh.vertices[i].y, mesh.vertices[i].z,
						mesh.normals[i].x, mesh.normals[i].y, mesh.normals[i].z,
						mesh.uvs[i].x, mesh.uvs[i].y)
				);
			}

			// add indices
			for (unsigned int i = 0; i < mesh.faces.size(); i += 3)
			{
				newMesh.meshes[nm].AddFace(mesh.faces[i], mesh.faces[i + 1], mesh.faces[i + 2]);
				internal_gen_vertex_normal(newMesh.meshes[nm], mesh.faces[i], mesh.faces[i + 1], mesh.faces[i + 2]);

			}
		}
#pragma endregion 

#pragma region NORMAL_PRESENT
		if (mesh.normals.size() && !mesh.uvs.size())
		{
			for (unsigned int i = 0; i < mesh.vertices.size(); ++i)
			{
				newMesh.meshes[nm].AddVertex(
					Vertex(
						mesh.vertices[i].x, mesh.vertices[i].y, mesh.vertices[i].z,
						mesh.normals[i].x, mesh.normals[i].y, mesh.normals[i].z,
						0.f, 0.f)
				);
			}

			for (unsigned int i = 0; i < mesh.faces.size(); i += 3)
			{
				newMesh.meshes[nm].AddFace(mesh.faces[i], mesh.faces[i + 1], mesh.faces[i + 2]);
				internal_gen_vertex_normal(newMesh.meshes[nm], mesh.faces[i], mesh.faces[i + 1], mesh.faces[i + 2]);
			}
		}
#pragma endregion

#pragma region NORMAL_NOT_PRESENT
		// normal and uv are not generated
		else if (mesh.normals.size() == 0)
		{
			for (unsigned int i = 0; i < mesh.vertices.size(); ++i)
			{
				newMesh.meshes[nm].AddVertex(
					Vertex(
						mesh.vertices[i].x, mesh.vertices[i].y, mesh.vertices[i].z,
						0, 0, 0,
						0.f, 0.f)
				);
			}
			
			for (unsigned int i = 0; i < mesh.faces.size(); i += 3)
			{
				newMesh.meshes[nm].AddFace(mesh.faces[i], mesh.faces[i + 1], mesh.faces[i + 2]);
				internal_gen_vertex_normal(newMesh.meshes[nm], mesh.faces[i], mesh.faces[i + 1], mesh.faces[i + 2]);
			}
			
			newMesh.meshes[nm].WeightNormal();
			newMesh.meshes[nm].GenerateVertexNormal();
			newMesh.meshes[nm].GenerateFaceNormal();
		}
#pragma endregion
		if constexpr (std::is_same<T, SkeletalMesh>())
		{
			// add bones
			if (mesh.bones.size() > 0)
			{
				// fill in bones data to each vertex
				newMesh.meshes[nm].AllocateBoneData();
				for (unsigned int i = 0; i < mesh.bones.size(); ++i)
				{
					auto& sBone = mesh.bones[i];
					Bone& b = newMesh.bones[sBone.name];
					b.name = sBone.name;
					if (newMesh.mBoneIDMapping.find(b.name) == newMesh.mBoneIDMapping.end())
						newMesh.mBoneIDMapping[b.name] = boneNum++;
					for (unsigned int j = 0; j < sBone.weights.size(); ++j)
					{
						Weight& w = b.weights.emplace_back(Weight(sBone.weightID[j], sBone.weights[j]));

						for (unsigned k = 0; k < 4; ++k)
						{
							unsigned idSum = w.vertex_id * 4 + k;
							//if (newMesh.meshes[nm].bone_weights.size() <= idSum) continue;
							if (newMesh.meshes[nm].bone_weights[idSum] == 0)
							{
								newMesh.meshes[nm].bone_ids[idSum] = newMesh.mBoneIDMapping[b.name];
								newMesh.meshes[nm].bone_weights[idSum] = w.weight;
								break;
							}
						}
					}
					b.offset_matrix = sBone.offset_matrix;
				}

				if (newMesh.nodeMatrices.size() == 0)
				{
					newMesh.root_node = std::move(mMeshes.sMeshes[0].root_node);
					newMesh.nodeMatrices = std::move(mMeshes.sMeshes[0].nodeMatrices);
				}
			}
		}

		newMesh.materials.push_back(nullptr);
	}
	if constexpr (std::is_same<T, SkeletalMesh>())
	{
		for (unsigned i = 0; i < mMeshes.animations.size(); ++i)
		{
			newMesh.animations.push_back(mMeshes.animations[i]);
		}
		if (m->meshes.size())
		{
			m->gPreRotationName = mMeshes.sMeshes[0].gPreRotationName;
			m->gRotationName = mMeshes.sMeshes[0].gRotationName;
			m->gScalingName = mMeshes.sMeshes[0].gScalingName;
			m->gTranslationName = mMeshes.sMeshes[0].gTranslationName;
			m->globalTransformExist = mMeshes.sMeshes[0].globalTransformExist;
		}
		m->gBoneTranxSize = (unsigned)(m->bones.size());
	}

	return m;
}