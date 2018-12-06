#include "stdafx.h"
#include "Compiler.h"
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>


Matrix4x4 aimat4_to_mat4(aiMatrix4x4 m)
{
	return Matrix4x4(
		m.a1, m.b1, m.c1, m.d1,
		m.a2, m.b2, m.c2, m.d2,
		m.a3, m.b3, m.c3, m.d3,
		m.a4, m.b4, m.c4, m.d4);
}


const std::unordered_map<std::string, Extensioninfo> Compiler::extensionDirectory
{
	std::make_pair(".fbx", Extensioninfo{ ".mdl", "Models", CompileFBX }),
	std::make_pair(".obj", Extensioninfo{ ".mdl", "Models", CompileObj }),
	std::make_pair(".mdl", Extensioninfo{ ".mdl", "Models", CompileMdl }),
	std::make_pair(".3ds", Extensioninfo{ ".mdl", "Models", CompileFBX }),
	std::make_pair(".dae", Extensioninfo{ ".mdl", "Models", CompileFBX }),
	// DevIL stuff
#ifdef EDITOR
	std::make_pair(".png", Extensioninfo{ ".dds", "Sprites", ConvertImageFile }),
	std::make_pair(".jpeg", Extensioninfo{ ".dds", "Sprites", ConvertImageFile }),
	std::make_pair(".jpg", Extensioninfo{ ".dds", "Sprites", ConvertImageFile }),
	std::make_pair(".jpe", Extensioninfo{ ".dds", "Sprites", ConvertImageFile }),
	std::make_pair(".dds", Extensioninfo{ ".dds", "Sprites", ConvertImageFile }),
	std::make_pair(".tga", Extensioninfo{ ".dds", "Sprites", ConvertImageFile }),
#endif
};

Compiler::Compiler():
	messageQueue(std::make_unique<ThreadsafeQueue<CompilerMessage>>()),
	running(true)
{
}

void Compiler::Update()
{
  //std::this_thread::sleep_for(std::chrono::seconds(3));
	while (running)
	{
		auto messages = messageQueue->ReadMessages();

		for (auto& item : messages)
		{
			switch (item.type)
			{
			case COMPILERMSG::COMPILEFILE:
			{
				compilingAssets = true;
				CompileFile(item.directory);
				break;
			}
			}
		}
		/*if (messages.size() == 0)
		{
			GraphicsMessage msg;
			msg.type = GFXMSG::STARTLOADINGASSETS;
			BROADCAST_MESSAGE(msg);
		}*/
	}
}

std::string Compiler::CompileFile(const std::string & source)
{
	std::string destination = GetMappedDirectory(source);
	std::string extension = TOOLS::GetExtension(source);
	std::transform(extension.begin(), extension.end(), extension.begin(), TOOLS::modifiedtolower);
	auto iter = extensionDirectory.find(extension);
	if (iter == extensionDirectory.end())
		return "";
	CONSOLE_LN("Converting file from ", source, " to ", destination, ".");
	compilingAssets = true;
	iter->second.compileFunc(source, destination);
	compilingAssets = false;
	return destination;
}

std::string Compiler::GetMappedDirectory(const std::string & source)
{
	auto extension = TOOLS::GetExtension(source);
	std::transform(extension.begin(), extension.end(), extension.begin(), TOOLS::modifiedtolower);
	auto iter = extensionDirectory.find(extension);
	std::string destination = "CompiledAssets\\";

	if (iter == extensionDirectory.end())
		return "";

	destination += iter->second.folder;
	auto pos = std::find(source.rbegin(), source.rend(), '\\');
	auto pos2 = std::find(source.rbegin(), source.rend(), '/');
	if (pos == source.rend()) pos = pos2;
	auto diff = std::distance(source.begin(), pos.base()) - 1;
	if (pos != source.rend())
	{
		destination += source.substr(diff, source.size() - 4 - diff);
	}
	destination += iter->second.extension;
	destination = TOOLS::ReplaceBSlashWithFSlash(destination);
	std::transform(destination.begin(), destination.end(), destination.begin(), TOOLS::modifiedtolower);
	return destination;
}


void hierachical_add(aiNode * n, Node& myn, SerialMesh& mcomp, std::vector<Matrix4x4>& mat)
{
	// check global scaling, rotation, pre-rotation, translation keys
	std::string name = n->mName.C_Str();
	size_t begintoken = name.find_first_of('$', 0);
	size_t endtoken = std::string::npos;

	if (begintoken != std::string::npos)
		endtoken = name.find_first_of('$', begintoken + 1);
	std::string token;

	if (begintoken != std::string::npos && endtoken != std::string::npos)
	{
		token = name.substr(begintoken + 1, endtoken - begintoken - 1);
	}

	if (!token.empty())
	{
		if (token.compare("AssimpFbx") == 0)
		{
			mcomp.globalTransformExist = true;

			std::string tag = name.substr(endtoken + 2, name.length() - endtoken - 2);
			if (tag.compare("Translation") == 0)
				mcomp.gTranslationName = name;

			if (tag.compare("PreRotation") == 0)
				mcomp.gPreRotationName = name;

			if (tag.compare("Rotation") == 0)
				mcomp.gRotationName = name;

			if (tag.compare("Scaling") == 0)
				mcomp.gScalingName = name;
/*
			Node newnode;
			newnode.name = n->mName.C_Str();
			myn.children.push_back(newnode);*/
		}
	}
	else
	{
		/*name = std::string(n->mName.C_Str());
		auto iter = std::find_if(mcomp.bones.begin(), mcomp.bones.end(), 
			[&](const sBone& bone)
		{
			if (bone.name == name) return true;
			return false;
		});
		if (iter != mcomp.bones.end())
		{
			myn.children.emplace_back();
			myn.children.back().name = name;
			myn.children.back().mMappingIndex = (int)(mcomp.boneMappingNum.size() + 1);
			mcomp.boneMappingNum.emplace_back((int)(mcomp.boneMappingNum.size() + 1));
			mcomp.boneMappingName.emplace_back(name);
		}*/
	}
	mat.emplace_back(aimat4_to_mat4(n->mTransformation));

	for (unsigned i = 0; i < n->mNumChildren; ++i)
	{
		myn.children.emplace_back();
		myn.children.back().name = n->mChildren[i]->mName.C_Str();
		myn.children.back().mMappingIndex = (int)(mcomp.boneMappingNum.size() + 1);
		mcomp.boneMappingNum.emplace_back((int)(mcomp.boneMappingNum.size() + 1));
		mcomp.boneMappingName.emplace_back(myn.children.back().name);
		hierachical_add(n->mChildren[i], myn.children.back(), mcomp, mat);
	}
}

Node populate_node(aiNode * rootnode, SerialMesh& sm, std::vector<Matrix4x4>& mat)
{
	std::queue<aiNode*> bfsq;
	Node root_node;

	hierachical_add(rootnode, root_node, sm, mat);

	/*for (unsigned int i = 0; i < rootnode->mNumChildren; ++i)
	{
		bfsq.push(rootnode->mChildren[i]);
		root_node.children.emplace_back();
		root_node.children.back().name = rootnode->mChildren[i]->mName.C_Str();
	}

	int num = 0;
	while (!bfsq.empty())
	{
		aiNode* n = bfsq.front();
		bfsq.pop();
		root_node.children.emplace_back();
		root_node.children.back().name = n->mName.C_Str();
		hierachical_add(n, root_node.children.back(), sm);

		if (n->mNumChildren > 0)
			for (unsigned i = 0; i < n->mNumChildren; ++i)
				bfsq.push(n->mChildren[i]);
	}*/
	return std::move(root_node);
}



void CompileFBX(const std::string & source, const std::string & destination)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(source, aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene)
	{
		const char* errorstr = importer.GetErrorString();
		CONSOLE_ERROR(errorstr);
		return;
	}
	MeshCompiler compiler;
	unsigned int num_mesh = scene->mNumMeshes;
	compiler.sMeshes.reserve(num_mesh);
	compiler.transform = aimat4_to_mat4(scene->mRootNode->mTransformation);
	for (unsigned nm = 0; nm < num_mesh; ++nm)
	{
		aiMesh& mesh = *scene->mMeshes[nm];
		SerialMesh& newMesh = compiler.sMeshes.emplace_back();
#pragma region NORMAL_UV_PRESENT
		// normal and uv already generated
		if (mesh.mNormals != nullptr && mesh.mTextureCoords[0] != nullptr)
		{
			for (unsigned int i = 0; i < mesh.mNumVertices; ++i)
			{
				newMesh.vertices.emplace_back(mesh.mVertices[i].x, mesh.mVertices[i].y, mesh.mVertices[i].z);
				newMesh.normals.emplace_back(mesh.mNormals[i].x, mesh.mNormals[i].y, mesh.mNormals[i].z);
				newMesh.uvs.emplace_back(mesh.mTextureCoords[0][i].x, mesh.mTextureCoords[0][i].y);
			}

			for (unsigned int i = 0; i < scene->mMeshes[nm]->mNumFaces; ++i)
			{
				newMesh.faces.emplace_back(mesh.mFaces[i].mIndices[0]);
				newMesh.faces.emplace_back(mesh.mFaces[i].mIndices[1]);
				newMesh.faces.emplace_back(mesh.mFaces[i].mIndices[2]);
			}

			//newMesh.meshes[nm].AssignBuffers();
		}
#pragma endregion 

#pragma region NORMAL_PRESENT
		if (mesh.mNormals != nullptr && mesh.mTextureCoords[0] == nullptr)
		{
			for (unsigned int i = 0; i < mesh.mNumVertices; ++i)
			{
				newMesh.vertices.emplace_back(mesh.mVertices[i].x, mesh.mVertices[i].y, mesh.mVertices[i].z);
			}

			for (unsigned int i = 0; i < scene->mMeshes[nm]->mNumFaces; ++i)
			{
				newMesh.faces.emplace_back(mesh.mFaces[i].mIndices[0]);
				newMesh.faces.emplace_back(mesh.mFaces[i].mIndices[1]);
				newMesh.faces.emplace_back(mesh.mFaces[i].mIndices[2]);
			}
		}
#pragma endregion

#pragma region NORMAL_NOT_PRESENT
		// normal and uv are not generated
		else if (mesh.mNormals == nullptr)
		{
			for (unsigned int i = 0; i < mesh.mNumVertices; ++i)
			{
				newMesh.vertices.emplace_back(mesh.mVertices[i].x, mesh.mVertices[i].y, mesh.mVertices[i].z);

			}

			for (unsigned int i = 0; i < scene->mMeshes[nm]->mNumFaces; ++i)
			{
				newMesh.faces.emplace_back(mesh.mFaces[i].mIndices[0]);
				newMesh.faces.emplace_back(mesh.mFaces[i].mIndices[1]);
				newMesh.faces.emplace_back(mesh.mFaces[i].mIndices[2]);
			}
		}
#pragma endregion
		newMesh.bones.reserve(mesh.mNumBones);
		for (unsigned int i = 0; i < mesh.mNumBones; ++i)
		{
			sBone b;
			aiBone& mb = *mesh.mBones[i];

			b.name = mb.mName.data;
			for (unsigned int j = 0; j < mb.mNumWeights; ++j)
			{
				b.weights.push_back(mb.mWeights[j].mWeight);
				b.weightID.push_back(mb.mWeights[j].mVertexId);
			}
			b.offset_matrix = aimat4_to_mat4(mb.mOffsetMatrix);
			
			newMesh.bones.emplace_back(b);
		}

		hierachical_add(scene->mRootNode, newMesh.root_node, newMesh, newMesh.nodeMatrices);
		newMesh.UpdateSerializables();
	}
	 if (scene->HasAnimations())
	 {
	   for (unsigned i0 = 0; i0 < scene->mNumAnimations; ++i0)
	   {
		   auto anim = scene->mAnimations[i0];
		   compiler.animations.emplace_back();
		   compiler.animations.back().name = anim->mName.C_Str();
		   compiler.animations.back().duration = (float)anim->mDuration;
		   compiler.animations.back().ticks_per_sec = (float)anim->mTicksPerSecond;
		   float minChnlDurationMinus = std::numeric_limits<float>::max();
		   for (unsigned i1 = 0; i1 < anim->mNumChannels; ++i1)
		   {
			   compiler.animations.back().channels.emplace_back();
			   auto ch = anim->mChannels[i1];
			   auto& chnl = compiler.animations.back().channels.back();
			   chnl.name = ch->mNodeName.C_Str();
			   float minTime = std::numeric_limits<float>::max();
			   if (ch->mNumPositionKeys > 0)
				   minTime = minTime > ch->mPositionKeys[0].mTime ? ch->mPositionKeys[0].mTime : minTime;
			   if (ch->mNumRotationKeys > 0)
				   minTime = minTime > ch->mRotationKeys[0].mTime ? ch->mRotationKeys[0].mTime : minTime;
			   if (ch->mNumScalingKeys > 0)
				   minTime = minTime > ch->mScalingKeys[0].mTime ? ch->mScalingKeys[0].mTime : minTime;
			   if (minTime == std::numeric_limits<float>::max())
				   minTime = 0.f;
			   for (unsigned i2 = 0; i2 < ch->mNumPositionKeys; ++i2)
			   {
				   chnl.pk_timestamp.emplace_back((float)ch->mPositionKeys[i2].mTime - minTime);
				   chnl.pk_value.emplace_back(Vector3(ch->mPositionKeys[i2].mValue.x,
					   ch->mPositionKeys[i2].mValue.y, ch->mPositionKeys[i2].mValue.z));
			   }
			   for (unsigned i3 = 0; i3 < ch->mNumRotationKeys; ++i3)
			   {
				   chnl.rk_timestamp.emplace_back((float)ch->mRotationKeys[i3].mTime - minTime);
				   chnl.rk_value.emplace_back(Vector4(ch->mRotationKeys[i3].mValue.x,
					   ch->mRotationKeys[i3].mValue.y, ch->mRotationKeys[i3].mValue.z,
					   ch->mRotationKeys[i3].mValue.w));
			   }
			   for (unsigned i4 = 0; i4 < ch->mNumScalingKeys; ++i4)
			   {
				   chnl.sk_timestamp.emplace_back((float)ch->mScalingKeys[i4].mTime - minTime);
				   chnl.sk_value.emplace_back(Vector3(ch->mScalingKeys[i4].mValue.x,
					   ch->mScalingKeys[i4].mValue.y, ch->mScalingKeys[i4].mValue.z));
			   }
			   minChnlDurationMinus = minTime > minChnlDurationMinus ? minChnlDurationMinus : minTime;
		   }
		   compiler.animations.back().duration -= minChnlDurationMinus;
	   }

	 }
	compiler.UpdateSerializables();
	compiler.UpdateSerialMeshInfo();
	compiler.UpdateSerialAnimInfo();
	Serializer serial;
	auto& node = serial.NewElement("ModelFile");
	compiler.Serialize(node);
	serial.SaveFile(destination);

	GraphicsMessage msg;
	msg.type = GFXMSG::COMPILEDFILE;
	std::get<ModelDetails>(msg.data).model = destination;
	BROADCAST_MESSAGE(msg);
}

void CompileObj(const std::string & source, const std::string & destination)
{
	UNREFERENCED_PARAMETER(source);
	UNREFERENCED_PARAMETER(destination);
}

void CompileMdl(const std::string & source, const std::string & destination)
{
	UNREFERENCED_PARAMETER(source);
	UNREFERENCED_PARAMETER(destination);
}

#ifdef EDITOR
void ConvertImageFile(const std::string & source, const std::string & destination)
{
	STARTDEVIL
	DevIL::DevILLoadImage(source);
	DevIL::DevILSaveImage(destination);
	ENDDEVIL
	GraphicsMessage msg;
	msg.type = GFXMSG::COMPILEDFILE;
	std::get<ModelDetails>(msg.data).texture = destination;
	BROADCAST_MESSAGE(msg);
}
#endif