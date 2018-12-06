#include "stdafx.h"
#include "ComponentWindow.h"

ComponentWindow::ComponentWindow(Editor& edit) : 
	GuiWindow(edit, windowName)
{
	std::vector<std::string> prefabLocations;
	std::string path = PREFAB_DIRECTORY;
	for (const auto &p : fs::directory_iterator(path))
		prefabLocations.emplace_back(TOOLS::ReplaceBSlashWithFSlash(p.path().string()));
	for (auto& item : prefabLocations)
	{
		if (prefabs[item].LoadPrefab(item))
		{
			prefabs[item].UpdateComponentData();
			prefabs[item].UpdateRequiredComponents();
		}
	}
}

void ComponentWindow::Display(Editor& edit)
{
	auto& selection = edit.selectedEntities;
	size_t sz = selection.size();
	// Check if only 1 item is selected
	if (sz == 1)
	{
		auto& ent = selection[0];
		ImGui::InputText("Prefab Name", &ent->prefabName[0], ent->prefabName.size());
		std::stringstream fnNames;
		fnNames << "Callable Functions:\n";
		for (auto& item : edit.entityList->luaScript->luaFunctionNames)
			fnNames << item << "\n";
		TOOLS::AddToolTip(fnNames.str());
		if (ImGui::Button("Save Prefab"))
		{
			SavePrefab(*ent);
			edit.entityList->UpdatePrefabs(*ent);
		}
		ImGui::SameLine();
		if (ImGui::Button("Revert to Prefab"))
		{
			bool found = false;
			std::string nameStr = std::string(PREFAB_DIRECTORY) + 
				ent->prefabName.data() + ".pfb";
			for (auto& item : prefabs)
			{
				if (nameStr == item.first && ent->LoadPrefab(item.first))
				{
					edit.entityList->SetupEntityComponent(*ent);
					ent->UpdateComponentData();
					ent->UpdateRequiredComponents();
					ent->EditorSetup();
					found = true;
					break;
				}
			}
			if (!found)
			{
				TOOLS::CreateErrorMessageBox("Could not find prefab to load!");
			}
		}

		// Generate the option to Add more components
		ComponentList(selection);

		ImGui::BeginChild("Components Section", ImVec2{ 0, -ImGui::GetTextLineHeightWithSpacing() }, true, ImGuiWindowFlags_HorizontalScrollbar);


		if (currentComponent.get() && ImGui::BeginPopupContextWindow("Context"))
		{
			ImGui::Text(ComponentDirectory::names[int(currentComponent->c_type)].data());
			ImGui::Separator();
			if (ImGui::MenuItem("Copy Component"))
			{
				copiedComponent = currentComponent->Clone();
			}
			if (copiedComponent.get())
			{
				std::stringstream ss;
				ss << "Paste " << ComponentDirectory::names[int(copiedComponent->c_type)]
					<< " Component";
				if (ImGui::MenuItem(ss.str().c_str()))
					PasteCopiedComponent(*selection[0]);
			}
			ImGui::EndPopup();
		}
	// Generate the List of components
		if (ent->components.size())
		{
			auto iter = ent->components.begin();
			while (iter != ent->components.end())
			{
				auto& item = *iter;
				bool active = true;
				const char* _name = ComponentDirectory::names[static_cast<unsigned>(item->GetType())].data();

				std::string ss{ "Callable Functions:\n" };
				for (auto& function : item->callableFunctions)
					ss.append(function + "\n");

				// We would want to update the serializables during playing mode as well as during frame skipping mode
				if (edit.IsPlaying() || edit.FrameSkipping())
				{
					item->UpdateSerializables();
				}
				if (ImGui::CollapsingHeader(_name, &active))
				{
					TOOLS::AddToolTip(ss);
					// In this state, we will allow the custom display of the imgui serializables
					std::optional<int> hasChanged = item->GenerateUI();
					bool editorChanged = item->EditorGUI(edit);

					if (hasChanged || editorChanged)
					{
						std::string pfbName = std::string(PREFAB_DIRECTORY) + ent->prefabName.data() + ".pfb";
						pfbName = TOOLS::ReplaceBSlashWithFSlash(pfbName);
						auto pfb = prefabs.find(pfbName);
						if (pfb != prefabs.end())
						{
							auto comp = pfb->second.GetComponent((*iter)->c_type);
							if (comp)
							{
								// Script checking
								bool updateActive = true;
								if ((*iter)->c_type == COMPONENTTYPE::SCRIPT)
								{
									ScriptC* scriptComp = static_cast<ScriptC*>(comp);
									ScriptC* scriptIter = static_cast<ScriptC*>((*iter).get());
									// @todo: change this to only one vector
									updateActive = scriptIter->scripts == scriptComp->scripts;
								}

								if (updateActive)
								{
									(*iter)->UpdateComponentData();
									if (editorChanged)
										comp->ApplyAsterisks(**iter);
									else
									{
										if ((*iter)->CheckDifferences(*comp, hasChanged.value()))
											(*iter)->asterisks[hasChanged.value()] = true;
										else
											(*iter)->asterisks[hasChanged.value()] = false;
									}
								}
							}
						}
						BROADCAST_MESSAGE(EditorMessage{ EDITORMSG::SAVEHISTORY });
					}
				}
				else if (ImGui::OpenPopupOnItemClick("Context"))
					currentComponent = (*iter)->Clone();
				else
					TOOLS::AddToolTip(ss);

				// Check if the close button is toggled
				if (!active)
					if (CheckRemoveComponent((*iter)->GetType(), *ent))
					{
						ent->RemoveComponent((*iter)->GetType());
						BROADCAST_MESSAGE(EditorMessage{ EDITORMSG::SAVEHISTORY, "Removed component" });
						break;
					}

				++iter;
			}
		}
		ImGui::EndChild();
		if (INPUT_S.GetReleaseKey(KEY_RETURN))
		{
			for (auto& comp : ent->components)
				comp->UpdateComponentData();
		}
	}

	// Otherwise check if there are more than 1 selection
	else if (sz > 1)
	{
		if (ImGui::BeginPopupContextWindow("Context"))
		{
			if (copiedComponent.get())
			{
				std::stringstream ss;
				ss << "Paste " << ComponentDirectory::names[int(copiedComponent->GetType())]
					<< " Component";
				if (ImGui::MenuItem(ss.str().c_str()))
				{
					for (auto& item : selection)
					{
						PasteCopiedComponent(*item);
						item->UpdateRequiredComponents();
					}

					BROADCAST_MESSAGE(EditorMessage{ EDITORMSG::SAVEHISTORY, "Pasted components" });
				}
			}
			ImGui::EndPopup();
		}

		std::string textStr("Multiple Items Selected");
		ImGui::Text(textStr.c_str());
		ImGui::OpenPopupOnItemClick("Context");
	}
}

void ComponentWindow::PasteCopiedComponent(Entity& ent)
{
	auto* comp = TOOLS::FindComponent(copiedComponent->c_type, ent.components);
	if (comp == nullptr)
	{
		if (ent.AddComponent(copiedComponent->c_type))
		{
			ent.components.back()->CopySerializableData(*copiedComponent);
			comp = ent.components.back().get();
		}
		else return;
	}
	else
	{
		comp->CopySerializableData(*copiedComponent);
	}
	comp->EditorSetup();
}


bool ComponentWindow::GizmosMove(Editor & edit, std::vector<Entity*>::iterator iter)
{
	bool result = false;
	auto& selection = edit.selectedEntities;
	auto selectionEnd = selection.end();
	auto xform = (*iter)->GetComponent<TransformC>();

	float c_translate[3]{}, c_rotation[3]{}, c_scale[3]{};

	// Get the settings for ImGuizmo::Manipulate
	Camera& cam = *GFX_S.GetCurrentCamera();
	Matrix4x4& view = *cam.GetView();
	Matrix4x4& projection = *cam.GetProjection();
	Matrix4x4 modelData{ *xform->GetTransformationMatrix() };
	result = ImGuizmo::Manipulate(view.data(), projection.data(), edit.current_operation, edit.current_mode, modelData.data(), NULL, edit.gizmos_snapping ? &edit.gsnap[0] : NULL);
	Vector3 changes{ 0,0,0 };

	auto & cur_operation = edit.current_operation;

	// Check if there's any transformation made
	if (result)
	{
		// Decompose our matrix and update them
		ImGuizmo::DecomposeMatrixToComponents(modelData.data(), c_translate, c_rotation, c_scale);
		switch (cur_operation)
		{
		case ImGuizmo::OPERATION::TRANSLATE:
			changes = xform->GetGlobalPosition();
			xform->SetGlobalPosition(c_translate);
			changes = c_translate - changes;
			break;
		case ImGuizmo::OPERATION::ROTATE:
			changes = xform->localRotation;
			xform->SetGlobalRotation(c_rotation);
			changes = xform->localRotation - changes;
			break;
		case ImGuizmo::OPERATION::SCALE:
			changes = xform->localScale;
			xform->SetScale(c_scale);
			changes = xform->localScale - changes;
			break;
		}
		xform->UpdateSerializables();

		auto ent = *iter;
		std::string pfbName = std::string(PREFAB_DIRECTORY) + ent->prefabName.data() + ".pfb";
		pfbName = TOOLS::ReplaceBSlashWithFSlash(pfbName);
		auto pfb = prefabs.find(pfbName);
		if (pfb != prefabs.end())
		{
			auto comp = pfb->second.GetComponent<TransformC>();
			if (comp)
			{
				comp->ApplyAsterisks(*xform);
			}
		}

		iter++;
		while (iter != selectionEnd)
		{
			auto xform2 = (*iter++)->GetComponent<TransformC>();
			switch (cur_operation)
			{
			case ImGuizmo::OPERATION::TRANSLATE:
				xform2->SetGlobalPosition(xform2->GetGlobalPosition() + changes);
				break;
			case ImGuizmo::OPERATION::ROTATE:
				xform2->localRotation += changes;
				break;
			case ImGuizmo::OPERATION::SCALE:
				xform2->localScale += changes;
				break;
			}
			DEBUGDRAW_S.DrawAabb({ xform2->GetGlobalPosition() - xform2->ModelMin, xform2->GetGlobalPosition() + xform2->ModelMax });
			xform2->UpdateSerializables();
		}

	}

	return result;
}
void ComponentWindow::GizmoUpdate(Editor & edit)
{
	auto& selection = edit.selectedEntities;
	auto selectionItr = selection.begin();
	auto selectionEnd = selection.end();
	if (selectionItr != selectionEnd)
	{
		auto xform = (*selectionItr)->GetComponent<TransformC>();
		if (xform)
		{
			// First instance of using the gizmos
			if (ImGuizmo::IsUsing() && !(*selectionItr)->onGizmos)
			{
				(*selectionItr)->onGizmos = true;
				// This portion to set our gizmos vectors (for the "before")
				xform->gizmosPosition = xform->GetGlobalPosition();
				xform->gizmosRotation = xform->localRotation;
				xform->gizmosScale = xform->localScale;
			}
			bool moved = GizmosMove(edit, selectionItr);
			UNREFERENCED_PARAMETER(moved);
			// Set the boolean to false after moving
			if (!ImGuizmo::IsUsing() && (*selectionItr)->onGizmos)
			{
				(*selectionItr)->onGizmos = false;

				// Detect if there's any changes
				switch (edit.current_operation)
				{
				case ImGuizmo::OPERATION::TRANSLATE:
					if ((xform->GetGlobalPosition() - xform->gizmosPosition) != Vector3{})
						// Send an update to editor serializable that this is updated
						BROADCAST_MESSAGE(EditorMessage{ EDITORMSG::SAVEHISTORY, "Entity moved" });
					break;
				case ImGuizmo::OPERATION::ROTATE:
					if ((xform->localRotation - xform->gizmosRotation) != Vector3{})
						BROADCAST_MESSAGE(EditorMessage{ EDITORMSG::SAVEHISTORY, "Entity rotated" });
					break;
				case ImGuizmo::OPERATION::SCALE:
					if ((xform->localScale - xform->gizmosScale) != Vector3{})
						BROADCAST_MESSAGE(EditorMessage{ EDITORMSG::SAVEHISTORY, "Entity scaled" });
					break;
				}
			}
		}
	}
}

void ComponentWindow::ComponentList(std::vector<Entity*>& selection)
{
	ImGui::Separator();
	static auto vec_names = ComponentDirectory::names;
	static size_t sz = vec_names.size();
	static std::vector<const char*> args;

	if (!args.size())
	{
		// Push everything but the first value
		for (unsigned i = 1; i != sz; ++i)
			args.push_back(vec_names[i].data());
		// Sort all of them in ascending order
		std::sort(args.begin(), args.end(), [](const char* lhs, const char* rhs)
		{
			return strcmp(lhs, rhs) < 0;
		});
		// Insert the "None" to the front
		args.insert(args.begin(), vec_names[0].data());
	}
	char** charArgs = const_cast<char**>(&args[0]);
	int componentID = 0;
	ImGui::Combo("Components", &componentID, charArgs, (int)ComponentDirectory::names.size(), (int)ComponentDirectory::names.size());

	if (componentID)
	{
		const char* _str = args[componentID];
		auto iter = std::find_if(vec_names.begin(), vec_names.end(), [&_str](const std::string_view& strv)
		{
			return !strcmp(_str, strv.data());
		});
		int pos = (int)(iter - vec_names.begin());
		auto comp  = selection[0]->AddComponent(static_cast<COMPONENTTYPE>(pos));
		if (comp)
		{
			comp->EditorSetup();
			selection[0]->UpdateRequiredComponents();
			BROADCAST_MESSAGE(EditorMessage{ EDITORMSG::SAVEHISTORY });
		}
		componentID = 0;
	}
}

void ComponentWindow::Update(Editor& edit)
{
	// Only want the keys to be active when there isn't any scene playing in editor mode
	if (!edit.EditorKeyLocked())
	{
		//for (auto& ent : edit.selectedEntities)
			//ent->UpdateComponents();
		/*if (edit.ShortcutPressed(KEY_M))
			edit.MoveItems();
		if (edit.ShortcutPressed(KEY_C))
			edit.CopyItems();
		if (edit.ShortcutPressed(KEY_DELETE))
			edit.DeleteItems();*/
	}
	GizmoUpdate(edit);
}

void ComponentWindow::CheckEntityListForPrefabs(EntityList & elist)
{
	for (auto& ent : elist.entityList)
	{
		std::string pfbName = std::string(PREFAB_DIRECTORY) + ent->prefabName.data() + ".pfb";
		pfbName = TOOLS::ReplaceBSlashWithFSlash(pfbName);
		auto pfb = prefabs.find(pfbName);
		if (pfb != prefabs.end())
		{
			for (const auto& pfbComp : pfb->second.components)
			{
				auto cc = ent->GetComponent(pfbComp->c_type);
				if (cc)
				{
					pfbComp->ApplyAsterisks(*cc);
				}
			}
		}
	}
}

bool ComponentWindow::CheckRemoveComponent(COMPONENTTYPE type, const Entity& ent) const
{
	for (auto&item : ent.components)
	{
		auto& rComponents = item->requiredComponents;
		for (auto& comp : rComponents)
		{
			if (!comp->required)
				continue;
			if (type == comp->GetType())
			{
				std::string str =
					ComponentDirectory::names[unsigned(type)].data();
				str += " cannot be removed because it is required for ";
				str += ComponentDirectory::names[unsigned(item->GetType())].data();
				TOOLS::CreateErrorMessageBox(str);
				return false;
			}
		}
	}
	for (auto&item : ent.components)
		item->RemoveOptionalComponent(type);
	return true;
}
namespace
{
	void SerializeEntity(SerializeElem & elem, Entity& ent)
	{
		elem.SetAttribute("Name", &ent.prefabName[0]);
		for (auto& item : ent.components)
		{
			SerializeElem& cElement = elem.InsertChild(
				ComponentDirectory::names[unsigned(item->GetType())].data());
			item->Serialize(cElement);
			for (size_t i = 0; i < item->asterisks.size(); ++i)
				item->asterisks[i] = false;
		}
		auto& childrenElem = elem.InsertChild("Children");
		int childNum = 0;
		for (auto child : ent.children)
		{
			std::stringstream ss;
			ss << "Child" << childNum++;
			auto& childElem = childrenElem.InsertChild(ss.str());
			SerializeEntity(childElem, *child);
		}
	}
}

void ComponentWindow::SavePrefab(Entity & ent)
{
	if (!strlen(&ent.prefabName[0]))
	{
		CONSOLE_ERROR("Cannot have empty prefab name!");
		return;
	}
	Serializer Doc = ent.SerializeEntity();
	/*Serializer Doc;
	SerializeElem &pRoot = Doc.NewElement("Root");
	SerializeElem & pElement = pRoot.InsertChild("Prefab");
	SerializeEntity(pElement, ent);*/
	std::string saveString = std::string(PREFAB_DIRECTORY);
	saveString += &ent.prefabName[0];
	saveString += ".pfb";
	Doc.SaveFile(saveString.c_str());
	TOOLS::CreateInfoMessageBox("Prefab saved!");
	if (prefabs[saveString].LoadPrefab(saveString))
	{
		prefabs[saveString].UpdateComponentData();
		prefabs[saveString].UpdateRequiredComponents();
	}
}