#pragma once
#include "GuiWindow.h"
#include <map>

class Entity;
struct Material;

class MaterialWindow : public GuiWindow
{
	static constexpr unsigned numStr = 100;
	std::array<char, numStr> materialName;
	std::map<std::string, Material> materials;
public:
	static constexpr char windowName[] = "Materials";
	MaterialWindow(Editor& edit);
	void Display(Editor&) override;
	void CreateMaterial();
	void SaveMaterial(const std::string&);
	void DeleteMaterial(const std::string&);
	void Update(Editor&) override;
};
