#pragma once
#include <glew.h>
#include <gl/GL.h>
#include <imgui.h>
#include <imgui_guizmo.h>
#include <imgui_dock.h>
#include <array>

class Window;

class GuiManager
{
	bool CreateDeviceObjects();
	void UpdateFontTexture(GLuint&) const;
	std::array<bool, 3> s_mousePressed;

public:
	unsigned defaultNum, collapseNum, treeNum;
	int g_ShaderHandle = 0, g_VertHandle = 0, g_FragHandle = 0;
	int g_AttribLocationTex = 0, g_AttribLocationProjMtx = 0;
	int g_AttribLocationPosition = 0, g_AttribLocationUV = 0, g_AttribLocationColor = 0;
	unsigned int g_VboHandle = 0, g_VaoHandle = 0, g_ElementsHandle = 0;

	GuiManager(const Window& window);
	void LoadStyle() const;
	void SwitchStyle(bool scene_mode) const;
	void Update(Input& mousePos, const Window& displaySize, float dt = 1.f / 60.f);
	void UpdateIOs(UINT Msg, WPARAM wParam, LPARAM lParam);
	void Render();
	~GuiManager();
};

void RenderDrawLists(ImDrawData* draw_data);