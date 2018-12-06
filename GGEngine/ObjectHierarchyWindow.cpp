#include "stdafx.h"
#include "ObjectHierarchyWindow.h"

#define PItoDegree 180.0f/3.1415926535f

void ObjectHierarchyWindow::AddChildrens(std::vector<std::string>* names, Entity* pp)
{
	const std::vector<Entity*>& childrens = pp->children;
	for (auto& child : childrens)
	{
		std::string str = child->prefabName[0] ?
			child->prefabName.data() :
			static_cast<const char*>("strange Object"); //in this case there will be an error
		auto parent = child->parent;
		while (parent)
		{
			str = "  " + str;
			parent = parent->parent;
		}
		names->emplace_back(str);
		if (child->children.size())
			AddChildrens(names, child);
	}
}

void ObjectHierarchyWindow::DeleteObjects(Editor & edit)
{
	auto& eList = edit.entityList->entityList;
	auto& selection = edit.selectedEntities;
	std::vector<Entity*> entitiesToDelete;
	std::vector<std::vector<std::unique_ptr<Entity>>> hierarchies;
	for (auto item : edit.selectedEntities)
	{
		unsigned childIndex = edit.entityList->GetChildIndex(item);
		hierarchies.emplace_back();
		if (eList[childIndex].get() == nullptr) continue;

		//a chunk that represents the children of the child
		std::vector<std::unique_ptr<Entity>>& childHierarchy = hierarchies.back();
		childHierarchy.emplace_back(std::move(eList[childIndex]));
		auto childEndIndex = childIndex + 1;
		while (childEndIndex != eList.size())
		{
			if (!eList[childEndIndex].get()) break;
			bool found = false;
			for (auto& hierarchyItem : childHierarchy)
			{
				if (eList[childEndIndex]->parent == hierarchyItem.get())
				{
					edit.entityList->RemoveEntityComponents(*eList[childEndIndex]);
					found = true;
					childHierarchy.emplace_back(std::move(eList[childEndIndex]));
					++childEndIndex;
					break;
				}
			}
			if (!found)
				break;
		}
		edit.entityList->RemoveEntityComponents(*item);
		item->RemoveParent();
	}
	hierarchies.clear();
	auto removeIters = std::remove_if(edit.entityList->entityList.begin(), edit.entityList->entityList.end(),
		[&](std::unique_ptr<Entity>& ent)
	{
		if (ent.get() == nullptr) return true;
		return false;
	});
	if (removeIters != edit.entityList->entityList.end())
	{
		edit.entityList->entityList.erase(removeIters, edit.entityList->entityList.end());
		BROADCAST_MESSAGE(EditorMessage({ EDITORMSG::SAVEHISTORY, "Deleted objects" }));
	}
	selection.clear();
}

void ObjectHierarchyWindow::CopyPaste(Editor& edit)
{
	if (edit.selectedEntities.size())
	{
		//copy entity
		if (INPUT_S.GetHoldKey(KEY_LCTRL) && INPUT_S.GetReleaseKey(KEY_C))
		{
			copiedEntities.resize(edit.selectedEntities.size());
			for (size_t i = 0; i < copiedEntities.size(); ++i)
				copiedEntities[i] = std::move(edit.selectedEntities[i]->SerializeEntity());
		}
	}
	if (INPUT_S.GetHoldKey(KEY_LCTRL) && INPUT_S.GetReleaseKey(KEY_V))
	{
		if (edit.selectedEntities.size() == 1)
		{
			auto oldSelection = edit.selectedEntities[0];
			for (size_t i = 0; i < copiedEntities.size(); ++i)
			{
				auto ent = edit.CreateEntity(copiedEntities[i]);
				if (ent)
					ent->SetParent(*oldSelection);
			}
		}
		else
		{
			for (size_t i = 0; i < copiedEntities.size(); ++i)
			{
				edit.CreateEntity(copiedEntities[i]);
			}
		}
	}
}

void ObjectHierarchyWindow::Display(Editor& edit)
{
	CopyPaste(edit);

	static ImGuiTextFilter filter;
	// Draw the filter
	ImGui::Text("Search: ");
	ImGui::SameLine();
	filter.Draw("", ImGui::GetWindowContentRegionMax().x * 0.5f);
	ImGui::SameLine();
	if (ImGui::Button("Clear"))
		filter.Clear();

	std::vector<const char*> args(edit.entityList->entityList.size() + 1);
	args[0] = "Nothing Selected";
	selectedObjects.clear();
	selectedObjects.emplace_back(false);

	//push all the names found into a vector
	std::vector<std::string> hierarchyNames;
	for (auto& item : edit.entityList->entityList)
	{
		//go through all selected entity pointers and see which ones to select
		//yes its O(n^2), will come up with a better solution later
		bool found = false;
		for (auto& selectEnt : edit.selectedEntities)
		{
			if (item.get() == selectEnt)
			{
				found = true;
				selectedObjects.emplace_back(true);
				break;
			}
		}
		if (!found)
			selectedObjects.emplace_back(false);

		if (item.get()->parent)
			continue;
		std::string str = item->prefabName[0] ?
			item->prefabName.data() :
			static_cast<const char*>("New Object");
		auto parent = item.get()->parent;
		while (parent)
		{
			str = "  " + str;
			parent = parent->parent;
		}
		hierarchyNames.emplace_back(str);
		AddChildrens(&hierarchyNames, item.get());
	}

	if (!edit.selectedEntities.size())
		selectedObjects[0] = true;

	// turn all the arguements into const char* arrays
	std::transform(hierarchyNames.begin(), hierarchyNames.end(), args.begin() + 1,
		[](const std::string& s) { return s.c_str(); });


	char** charArgs = const_cast<char**>(&args[0]);
	if (ImGui::ListBoxHeader("", (int)args.size(), 18, ImVec2{ ImGui::GetWindowWidth() * 0.95f, 0 }))
	{
		for (unsigned i = 0; i < args.size(); ++i)
		{
			// Check for filtering
			if (!filter.PassFilter(charArgs[i]))
				continue;
			TOOLS::PushID(UITYPE::DEFAULT);
			if (ImGui::Selectable(charArgs[i], selectedObjects[i], ImGuiSelectableFlags_AllowDoubleClick))
			{
				selectedObjects[i] = true;
				if (!INPUT_S.GetHoldKey(KEY_LSHIFT) &&
					!INPUT_S.GetHoldKey(KEY_LCTRL))
					edit.selectedEntities.clear();
				if (i)
				{
					Entity* curr_selected = edit.entityList->entityList[i - 1].get();
					edit.selectedEntities.emplace_back(curr_selected);
					// Double clicked
					if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
						edit.FocusOnSelectedEntity(*GFX_S.GetCurrentCamera(), curr_selected);
				}
			}
			if (i) //dragging onto an object makes it a parent
			{
				if (ImGui::BeginDragDropSource())
				{
					int payload = i - 1;
					ImGui::SetDragDropPayload("Hierarchy", &payload, sizeof(payload));
					ImGui::Text(charArgs[i]);
					ImGui::EndDragDropSource();
				}
				if (ImGui::BeginDragDropTarget())
				{
					auto& entityList = edit.entityList->entityList;
					auto payLoad = ImGui::AcceptDragDropPayload("Hierarchy");
					if (payLoad)
					{
						int index = *(int*)payLoad->Data;
						if (entityList[index]->SetParent(*entityList[i - 1]))
						{
							entityList[index]->GetComponent<TransformC>()->UpdateLocal();
							MoveChildToParent(index, i - 1, entityList);
							BROADCAST_MESSAGE(EditorMessage({ EDITORMSG::SAVEHISTORY, "Changed parent" }));
						}
						else
							TOOLS::CreateErrorMessageBox("A parent cannot be a descendant of its child!");
					}
					ImGui::EndDragDropTarget();
				}
			}
			else //dragging onto "nothing selected" removes any existing parents
			{
				if (ImGui::BeginDragDropTarget())
				{
					auto payLoad = ImGui::AcceptDragDropPayload("Hierarchy");
					if (payLoad)
					{
						int index = *(int*)payLoad->Data;
						auto& er = edit.entityList->entityList;
						if (er[index]->parent)
						{
							er[index]->RemoveParent();
							er[index]->GetComponent<TransformC>()->UpdateLocal();
							while (index < er.size() - 1 && er[index + 1]->parent)
							{
								std::swap(er[index], er[index + 1]);
								++index;
							}
							//std::rotate(er.begin() + index, er.begin() + index + 1, er.end());
							BROADCAST_MESSAGE(EditorMessage({ EDITORMSG::SAVEHISTORY, "Removed parent" }));
						}

					}
					ImGui::EndDragDropTarget();
				}
			}
			ImGui::PopID();
		}
		ImGui::ListBoxFooter();
	}
	ImGui::Separator();
	if (edit.target_rotation)
	{
		// Inform that the camera is rotating about an object
		auto& selection = edit.selectedEntities;
		if (!selection.size() || selection[0] != edit.ent_target)
		{
			edit.target_rotation = false;
			edit.ent_target = nullptr;
		}
		else
		{
			auto& selected_entity = selection[0];
			std::string str{ "Camera is currently locked onto: " };
			ImGui::Text(str.c_str());
			ImGui::TextDisabled(selected_entity->prefabName.data());
			// Check if there's a name
			auto identifier = selected_entity->GetComponent<IdentifierC>();
			if (identifier)
			{
				// Print out the identifier's name
				str = "with the identifier's name: ";
				ImGui::Text(str.c_str());
				ImGui::TextDisabled(identifier->name.c_str());
			}
			// Display the tooltip box
			ImGui::TextDisabled("(?)");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(450.f);
				ImGui::TextUnformatted("To exit out of target-rotation, de-select the object!");
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}
		}
	}
}

void ObjectHierarchyWindow::Update(Editor& edit)
{
	auto& selection = edit.selectedEntities;
	// Only want the keys to be active when there isn't any scene playing in editor mode
	if (!edit.EditorKeyLocked())
	{
		if (INPUT_S.GetReleaseKey(KEY_DELETE))
			DeleteObjects(edit);
	}
	for (const auto & form : selection)
	{
		auto xform = form->GetComponent<TransformC>();
		Vector3 offSets{ 0.05f,0.05f,0.05f };
		Vector3 minDraw{ xform->GetGlobalScale() };
		Vector3 maxDraw{ minDraw };
		minDraw.x *= -xform->ModelMin.x;
		minDraw.y *= -xform->ModelMin.y;
		minDraw.z *= -xform->ModelMin.z;
		maxDraw.x *= xform->ModelMax.x;
		maxDraw.y *= xform->ModelMax.y;
		maxDraw.z *= xform->ModelMax.z;
		minDraw -= offSets;
		maxDraw += offSets;

    Vector3 ref = xform->GetRotation();

    while (ref.x < 0)
      ref.x += 360.0f;

    int tmp2 = ((static_cast<int>(ref.x) + 45) / 90) % 2;

    if (tmp2)
    {
      std::swap(minDraw.y, minDraw.z);
      std::swap(maxDraw.y, maxDraw.z);
    }

    while (ref.y < 0)
      ref.y += 360.0f;

    tmp2 = ((static_cast<int>(ref.y) + 45) / 90) % 2;

    if (tmp2)
    {
      std::swap(minDraw.x, minDraw.z);
      std::swap(maxDraw.x, maxDraw.z);
    }

    while (ref.z < 0)
      ref.z += 360.0f;

    tmp2 = ((static_cast<int>(ref.z) + 45) / 90) % 2;

    if (tmp2)
    {
      std::swap(minDraw.x, minDraw.y);
      std::swap(maxDraw.x, maxDraw.y);
    }


		DebugShape& tmp = DEBUGDRAW_S.DrawAabb({ xform->GetGlobalPosition() - minDraw, xform->GetGlobalPosition() + maxDraw });
		tmp.color.SetX(0.7f);
		tmp.color.SetY(0.4f);
	}
	for (const auto & form : edit.selectedGroupEntities)
	{
		auto xform = form->GetComponent<TransformC>();
		Vector3 offSets{ 0.05f,0.05f,0.05f };
		Vector3 minDraw{ xform->GetGlobalScale() };
		Vector3 maxDraw{ minDraw };
		minDraw.x *= -xform->ModelMin.x;
		minDraw.y *= -xform->ModelMin.y;
		minDraw.z *= -xform->ModelMin.z;
		maxDraw.x *= xform->ModelMax.x;
		maxDraw.y *= xform->ModelMax.y;
		maxDraw.z *= xform->ModelMax.z;
		minDraw -= offSets;
		maxDraw += offSets;
    Vector3 ref = xform->GetRotation();

    while (ref.x < 0)
      ref.x += 360.0f;

    int tmp2 = ((static_cast<int>(ref.x) + 45) / 90) % 2;

    if (tmp2)
    {
      std::swap(minDraw.y, minDraw.z);
      std::swap(maxDraw.y, maxDraw.z);
    }

    while (ref.y < 0)
      ref.y += 360.0f;

    tmp2 = ((static_cast<int>(ref.y) + 45) / 90) % 2;

    if (tmp2)
    {
      std::swap(minDraw.x, minDraw.z);
      std::swap(maxDraw.x, maxDraw.z);
    }

    while (ref.z < 0)
      ref.z += 360.0f;

    tmp2 = ((static_cast<int>(ref.z) + 45) / 90) % 2;

    if (tmp2)
    {
      std::swap(minDraw.x, minDraw.y);
      std::swap(maxDraw.x, maxDraw.y);
    }

		DebugShape& tmp = DEBUGDRAW_S.DrawAabb({ xform->GetGlobalPosition() - minDraw, xform->GetGlobalPosition() + maxDraw });
		tmp.color.SetX(0.7f);
		tmp.color.SetY(0.4f);
	}



	//for (const auto& item : selection)
	//{
	//	auto xform = item->GetComponent<TransformC>();
	//	if (xform)
	//	{
	//		float c_translate[3]{ }, c_rotation[3]{ }, c_scale[3]{ };
	//
	//		// Get the settings for ImGuizmo::Manipulate
	//		Camera& cam = *GFX_S.GetCurrentCamera();
	//		Matrix4x4& view = *cam.GetView();
	//		Matrix4x4& projection = *cam.GetProjection();
	//		Matrix4x4 modelData{ *xform->GetTransformationMatrix() };
	//		bool result = ImGuizmo::Manipulate(view.data(), projection.data(), edit.current_operation, edit.current_mode, modelData.data(), NULL, edit.gizmos_snapping ? &edit.gsnap[0] : NULL);
	//		// Check if there's any transformation made
	//		if (result)
	//		{
	//			// Decompose our matrix and update them
	//			ImGuizmo::DecomposeMatrixToComponents(modelData.data(), c_translate, c_rotation, c_scale);
	//			switch (edit.current_operation)
	//			{
	//			case ImGuizmo::OPERATION::TRANSLATE:
	//				xform->SetGlobalPosition(c_translate);
	//				break;
	//			case ImGuizmo::OPERATION::ROTATE:
	//				xform->SetGlobalRotation(c_rotation);
	//				break;
	//			case ImGuizmo::OPERATION::SCALE:
	//				xform->SetScale(c_scale);
	//				break;
	//			}
	//			xform->UpdateSerializables();
	//		}
	//	}
	//}

}

namespace
{
	size_t CountChildren(Entity& ent)
	{
		if (ent.children.size() == 0) return 0;
		else
		{
			size_t numChildren = ent.children.size();
			for (const auto& item : ent.children)
			{
				numChildren += CountChildren(*item);
			}
			return numChildren;
		}
	}
}

void ObjectHierarchyWindow::MoveChildToParent(int childIndex, int parentIndex,
	std::vector<std::unique_ptr<Entity>> & eList)
{
	auto childEndIndex = childIndex + CountChildren(*eList[childIndex]) + 1;
	//move the current entity such that it is right below its parent
	if (childIndex > parentIndex) //if index of the child is more than parent
	{
		std::rotate(eList.rend() - childEndIndex, eList.rend() - childIndex,
			eList.rend() - parentIndex - 1);
	}
	else if (childIndex < parentIndex) //if index of the child is less than parent
	{
		std::rotate(eList.begin() + childIndex, eList.begin() + childEndIndex, eList.begin() + parentIndex + 1);

	}
}
