#pragma once
#include <imgui.h>
#include <vector>
#include <type_traits>
#include "Serializer.h"
#include "CoreEngine.h"

class Editor;
class GuiWindow;
class entityList;

// Custom flags for our ImGui Window display
using EditorImGuiFlags = int;
static constexpr float SCREEN_Y_OFFSET = 20.f;
enum EditorImGuiFlags_
{
	EditorImGuiFlags_None				= 0,
	EditorImGuiFlags_Free				= 1 << 0,			// Free placement
	EditorImGuiFlags_PanLeft			= 1 << 1,			// Place the window at the left of the screen
	EditorImGuiFlags_PanRight			= 1 << 2,			// Place the window at the right of the screen
	EditorImGuiFlags_PanTop				= 1 << 3,			// Place the window at the top of the screen (will be below the main menu bar if there's one)
	EditorImGuiFlags_PanBottom			= 1 << 4,			// Place the window at the bottom of the screen
	EditorImGuiFlags_PanLeftReference	= 1 << 5,			// Place the window directly left of the Reference
	EditorImGuiFlags_PanRightReference	= 1 << 6,			// Place the window directly right of the Reference
	EditorImGuiFlags_PanAboveReference	= 1 << 7,			// Place the window directly top of the reference
	EditorImGuiFlags_PanBelowReference	= 1 << 8,			// Place the window directly below the Reference
	EditorImGuiFlags_PanAutoSize		= 1 << 9,			// Auto fixes the window size (given the restriction of a reference and fixed position)
	EditorImGuiFlags_NoYRescale			= 1 << 10,			// No rescaling when the windows rescales
	EditorImGuiFlags_NoXRescale			= 1 << 11,			// No rescaling when the windows rescales
	EditorImGuiFlags_MatchReferenceXPos	= 1 << 12,			// Match the x-position of the Reference
	EditorImGuiFlags_MatchReferenceYPos = 1 << 13,			// Match the y-position of the Reference
	EditorImGuiFlags_TransparentWindow	= 1 << 14,			// Make the background transparent
};

struct GeneralWindowHolder
{
	std::vector<GuiWindow*> windows;
	float bar_size;
	Vector2 position, size;						// In terms of percentage
	Vector2 previous_size;						// For flag - Flags_NoYRescale
	std::string displayName;
	ImGuiWindowFlags flags;						// imgui's flags
	EditorImGuiFlags editor_flags;				// editor's custom flags for placement of the windows
	GeneralWindowHolder* reference;				// the position will be based on this reference's position and size

	GeneralWindowHolder(const std::vector<GuiWindow*>& win, const std::string& name, const Vector2& pos, const Vector2& sz, GeneralWindowHolder *_ref = nullptr)
		: windows{ win }, position{ pos }, size{ sz }, displayName{ name }, flags{ ImGuiWindowFlags_None },
		  editor_flags{ EditorImGuiFlags_None }, reference{ _ref }, bar_size{ 0 }
	{ }

	virtual void Initialize();
	virtual void Update()				{}
	virtual void Display(const std::vector<GuiWindow*>&) = 0;
	void DisplayWindow();
	virtual ~GeneralWindowHolder()		{}
	void UpdateResolution();
};

template<typename T1>
class WindowHolder : public GeneralWindowHolder
{
	T1& referenceItem;

public:
	WindowHolder(T1& edit, const std::vector<GuiWindow*>& win, const std::string& name, const Vector2& pos, const Vector2& sz, GeneralWindowHolder *_ref = nullptr)
		: GeneralWindowHolder{ win, name, pos, sz, _ref }, referenceItem(edit)
	{
		// displayName = windows[0]->displayName;
	}

	void Display(const std::vector<GuiWindow*>& displayWindows) override
	{
		std::string _str;

		if (displayWindows.size() > 1)
		{
			_str += "Windows";
			ImGui::BeginTabBar(_str.c_str());
			for (auto& item : displayWindows)
			{
				if (ImGui::BeginTabItem(item->displayName.c_str(), &item->toggleDisplay))
				{
					item->Display(referenceItem);
					ImGui::EndTabItem();
				}
			}
			ImGui::EndTabBar();
		}
		else
		{
			displayWindows[0]->Display(referenceItem);
		}
	}

	void Initialize() override
	{
		GeneralWindowHolder::Initialize();
	}

	void Update() override
	{
		for (auto& item : windows)
			item->Update(referenceItem);
	}
};

template <typename WINDOWTYPE>
class WindowSet final
{
	WINDOWTYPE& referenceItem;
	std::vector<std::unique_ptr<GeneralWindowHolder>> windowHolders;
	std::vector<std::unique_ptr<GuiWindow>> windows;
	bool displayWindows;

	template <typename... Ts>
	std::enable_if_t<sizeof...(Ts) == 0>
	CreateNewWindow(std::vector<GuiWindow*>&)
	{ }

	template <typename U, typename... Ts>
	void CreateNewWindow(std::vector<GuiWindow*>& win)
	{
		bool found = false;
		for (auto& item : windows)
		{
			if (U::windowName == item->windowName)
			{
				found = true;
				win.emplace_back(item.get());
				break;
			}
		}
		if (!found)
		{
			windows.emplace_back(std::make_unique<U>(referenceItem));
			win.emplace_back(windows.back().get());
		}
		CreateNewWindow<Ts...>(win);
	}
public:
	float bar_size = 0;
	WindowSet(WINDOWTYPE& refObj)
		: referenceItem{ refObj }, displayWindows(true)
	{}

	void ClearState()
	{
		windowHolders.clear();
	}

	template<typename T, typename ...Args>
	GeneralWindowHolder* AddWindow(const std::string& name, const Vector2& size = Vector2{}, const Vector2& position = Vector2{}, GeneralWindowHolder *_ref = nullptr)
	{
		std::vector<GuiWindow*> newWindows;
		CreateNewWindow<T, Args...>(newWindows);
		windowHolders.emplace_back(std::make_unique<WindowHolder<WINDOWTYPE>>(referenceItem, std::move(newWindows), name, position, size, _ref));
		GeneralWindowHolder *latest = windowHolders.back().get();
		latest->bar_size = bar_size;
		return latest;
	}

	void Display()
	{
		for (auto& item : windowHolders)
			item->Update();
		
		// Only display the window when its toggled
		if (displayWindows)
		{
			for (auto& item : windowHolders)
				item->DisplayWindow();
		}
	}

	void CreateMenuItems()
	{
		for (auto&item : windows)
		{
			std::stringstream str;
			str << "Show " << item->displayName;
			ImGui::MenuItem(str.str().c_str(), nullptr, &item->toggleDisplay);
		}
	}

	void Serialize(Serializer & xmlDoc, Serializer* pElement) const
	{
		for (auto&item : windows)
		{
			auto wElement = xmlDoc.NewElement(item->windowName.c_str());
			item->UpdateSerializables();
			item->Serialize(xmlDoc, wElement);
			pElement->NewElement(wElement);
		}
	}

	void ConvertSerializedData(Serializer* const layerElem)
	{
		if (!layerElem)
		{
			CONSOLE_ERROR("Could not find previous window data!");
			return;
		}
		for (auto&item : windows)
		{
			const Serializer* wElem = layerElem->FirstChildElement(item->windowName.c_str());
			item->ConvertSerializedData(wElem);
		}
	}

	void Initialize()
	{
		for (auto& item : windowHolders)
			item->Initialize();
		for (auto& item : windows)
			item->Initialize(referenceItem);
	}

	void ToggleDisplay()
	{
		displayWindows = !displayWindows;
	}

	inline const auto& GetAllWindows() const			{ return windows; }
	inline const auto& GetAllGeneralWindows() const		{ return windowHolders; }
};