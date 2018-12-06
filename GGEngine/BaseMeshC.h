#pragma once
#include "Component.h"

class LuaScript;

template<typename MESHTYPE, typename COMPTYPE>
struct BaseMeshC
{
	float animTime;
	COMPTYPE& owner;
	MESHTYPE* mesh_ptr;
	std::string modelDirectory;
	int modelNum, animNo;
	int modelIndex, animIndex, matIndex;
	TransformC* transform;
	std::vector<std::string> materials;
	std::vector<int> materialNum;
	bool loaded = false;
	bool updateModelMinMax = true;
	void AddSerializeData(LuaScript* luaScript = nullptr)
	{
		auto& item = owner.AddSerializable("Model_Directory", modelDirectory, "", luaScript);
		item.display = false;
		auto& item3 = owner.AddSerializable("Materials", materials, "", luaScript);
		item3.display = false;
	}


	void AddAnimSerializeData(LuaScript* luaScript = nullptr)
	{
		auto& item = owner.AddSerializable("Animation Number", animNo, "", luaScript);
		item.display = false;
	}
	bool ChooseModelGUI();
	bool ChooseAnimationGUI();
	bool ChooseMaterialGUI();
	void Setup();
	void UpdateTransformMinMax();
	void UpdateModelNum();
	void UpdateMaterialNum();

	BaseMeshC(COMPTYPE& comp) :
		modelDirectory("CompiledAssets/Models/rhino.mdl"),
		transform(nullptr), owner(comp), modelNum(0), animNo(0), animTime(0.f)
	{
		comp.RequiredComponents(transform);
	}

	void Update()
	{
		if (!loaded)
		{
			if (transform && mesh_ptr && mesh_ptr->loaded)
			{
				UpdateTransformMinMax();
				if(materials.size() != mesh_ptr->meshes.size())
					ResetMaterials();
				loaded = true;
			}
		}
		UpdateTransformMinMax();
	}
	void ResetMaterials()
	{
		materials.resize(mesh_ptr->meshes.size());
		materialNum.resize(mesh_ptr->meshes.size());
		for (unsigned i = 0; i < mesh_ptr->meshes.size(); ++i)
		{
			materials[i] = "Metal";
			materialNum[i] = 0;
		}
		owner.UpdateSerializables(materials);
	}
};

template<typename MESHTYPE, typename COMPTYPE>
void BaseMeshC<MESHTYPE, COMPTYPE>::Setup()
{
	UpdateModelNum();
	UpdateMaterialNum();
	UpdateTransformMinMax();
	modelIndex = owner.GetAsteriskIndex(modelDirectory);
	matIndex = owner.GetAsteriskIndex(materials);
	animIndex = owner.GetAsteriskIndex(animNo);
}

template<typename MESHTYPE, typename COMPTYPE>
bool BaseMeshC<MESHTYPE, COMPTYPE>::ChooseMaterialGUI()
{
	if (!mesh_ptr) return false;
	auto& mList = GFX_S.GetMaterialList();
	std::stringstream ss;
	ss << "Materials";
	if (owner.asterisks[matIndex])
		ss << "*";
	ImGui::Text(ss.str().c_str());
	bool returnVal = false;
	materialNum.resize(mesh_ptr->meshes.size());
	if (materials.size() == 0)
	{
		materials.resize(mesh_ptr->meshes.size());
		for (unsigned i = 0; i < mesh_ptr->meshes.size(); ++i)
		{
			materials[i] = "Metal";
			materialNum[i] = 0;
		}
	}
	for (unsigned i = 0; i < materials.size(); ++i)
	{
		int prevNum = materialNum[i];
		TOOLS::PushID(UITYPE::DEFAULT);
		ImGui::Combo("", &materialNum[i], mList.data(), int(mList.size()), int(mList.size()));
		ImGui::PopID();
		if (prevNum != materialNum[i])
		{
			materials[i] = mList[materialNum[i]];
			owner.UpdateSerializables(materials);
			returnVal = true;
		}
		else if (materials[i] != mList[materialNum[i]])
		{
			auto iter = std::find(mList.begin(), mList.end(), materials[i]);
			if (iter != mList.end())
				materialNum[i] = iter - mList.begin();
		}
	}
	return returnVal;
}

template<typename MESHTYPE, typename COMPTYPE>
bool BaseMeshC<MESHTYPE, COMPTYPE>::ChooseModelGUI()
{
	auto& list = GFX_S.GetModelList();
	int prevNum = modelNum;
	std::stringstream ss;
	ss << "Model";
	if (owner.asterisks[modelIndex])
		ss << "*";
	ImGui::Text(ss.str().c_str());
	TOOLS::PushID(UITYPE::DEFAULT);
	ImGui::Combo("", &modelNum, list.data(), int(list.size()), int(list.size()));
	ImGui::PopID();
	if (prevNum != modelNum)
	{
		animNo = 0;
		animTime = 0.f;
		modelDirectory = list[modelNum];
		if constexpr(std::is_same<MESHTYPE, SkeletalMesh>())
			mesh_ptr = GFX_S.GetSkeletalMesh(modelDirectory.c_str());
		else
			mesh_ptr = GFX_S.GetStaticMesh(modelDirectory.c_str());
		loaded = mesh_ptr->loaded;
		//UpdateTransformMinMax();
		owner.UpdateSerializables(modelDirectory);
		ResetMaterials();
		BROADCAST_MESSAGE(EditorMessage{ EDITORMSG::SAVEHISTORY, "Changed model" });
		return true;
	}
	return false;
}

template<typename MESHTYPE, typename COMPTYPE>
bool BaseMeshC<MESHTYPE, COMPTYPE>::ChooseAnimationGUI()
{
	if (mesh_ptr->animations.size())
	{
		int oldNum = animNo;
		std::vector<const char*> args;
		for (const auto& item : mesh_ptr->animations)
		{
			args.push_back(item.name.data());
		}
		char** charArgs = const_cast<char**>(&args[0]);
		std::stringstream ss;
		ss << "Animation";
		if (owner.asterisks[animIndex])
			ss << "*";
		ImGui::Text(ss.str().c_str());
		ImGui::SameLine();
		TOOLS::PushID(UITYPE::DEFAULT);
		ImGui::Combo("", &animNo, charArgs, int(mesh_ptr->animations.size()),
			int(mesh_ptr->animations.size()));
		ImGui::PopID();
		if (oldNum != animNo) return true;
	}
	return false;
}


template<typename MESHTYPE, typename COMPTYPE>
void BaseMeshC<MESHTYPE, COMPTYPE>::UpdateTransformMinMax()
{
	if (!updateModelMinMax) return;
	if (mesh_ptr && !mesh_ptr->meshes.empty())
	{
		std::vector<Mesh>& tmp = mesh_ptr->meshes;
		Vector3 max = tmp[0].toprightback;
		Vector3 min = tmp[0].btmleftfront;
		for (const auto & elem : tmp)
		{
			if (elem.toprightback.x > max.x)
				max.x = elem.toprightback.x;

			if (elem.toprightback.y > max.y)
				max.y = elem.toprightback.y;

			if (elem.toprightback.z > max.z)
				max.z = elem.toprightback.z;

			if (elem.btmleftfront.x < min.x)
				min.x = elem.btmleftfront.x;

			if (elem.btmleftfront.y < min.y)
				min.y = elem.btmleftfront.y;

			if (elem.btmleftfront.z < min.z)
				min.z = elem.btmleftfront.z;
		}
		if(transform)
			transform->SetModelData(max, min);
		mesh_ptr->min = min;
		mesh_ptr->max = max;
	}
}

template<typename MESHTYPE, typename COMPTYPE>
void BaseMeshC<MESHTYPE, COMPTYPE>::UpdateModelNum()
{
	auto& list = GFX_S.GetModelList();
	for (unsigned i = 0; i < list.size(); ++i)
	{
		std::string ll = list[i];
		if (ll == modelDirectory)
		{
			modelNum = i;
			return;
		}
	}
	CONSOLE_ERROR("Could not find model ", modelDirectory, "!");
}

template<typename MESHTYPE, typename COMPTYPE>
void BaseMeshC<MESHTYPE, COMPTYPE>::UpdateMaterialNum()
{
	auto& mList = GFX_S.GetMaterialList();
	materialNum.resize(materials.size());
	for (unsigned i = 0; i < materials.size(); ++i)
	{
		unsigned j = 0;
		for (; j < mList.size(); ++j)
		{
			std::string str = mList[j];
			if (str == materials[i])
			{
				materialNum[i] = j;
				break;
			}
		}
		if (j == mList.size())
		{
			CONSOLE_ERROR("Could not find material ", materials[i], "!");
			materialNum[i] = 0;
		}
	}
}

