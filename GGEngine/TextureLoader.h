#pragma once
#include <vector>
#include <unordered_map>
#include "Texture.h"
#include <windef.h>
#include <future>
#include <optional>
#include <memory>

class TextureLoader
{
	const std::string defaultTex = "Resources\\Sprites\\Editor\\Invisible.dds";
	std::vector<std::string> texLoadQueue;
	std::unordered_map<std::string, Texture> m_textureManager;
	HGLRC mainContext, loaderContext;
	HDC windowDC;
	std::thread loadResources;
	std::packaged_task<void(std::string ss)> task;
	std::future<void> future;
	int w, h, n;
	unsigned int mipmapCount,mipmapFormat;
	unsigned char* image;
	void BindTexture();
	void LoadNewTexture();
public:
	std::optional<std::string> loadingMesh;
	TextureLoader(HDC context);
	void Update();
	void JoinThread();
	Texture* GetTexture(const std::string& path);
	void RecompileFile(const std::string & str);

	~TextureLoader();
};