#pragma once
#include <string>
#include <glew.h>
#include <memory>
#include <vector>
#include <atomic>
#include <future>
#include "Vector.h"

class Editor;

class ResourceObject
{
public:
	bool loaded, needsLoading;
	int sortID;
	std::string filename;
	std::string pathname;
	std::string extension;
	Texture* texture;
	ResourceObject(const std::string& str);
	std::string CreateButton(float size, Editor&,  int);
	void ButtonClick(const std::string&, Editor&);
};