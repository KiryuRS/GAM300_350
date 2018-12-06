#pragma once
#include <vector>
#include <string>
#include "GameShared.h"

struct StringList : std::vector< std::string >
{
	// Adding new variables to the container for our LUA container

	void SetSize(size_t size)
	{
		resize(size);
	}

	void PushBack(const std::string& str)
	{
		// Should call on emplace_back instead
		emplace_back(str);
	}

	void Erase(size_t pos)
	{
		if (size() <= pos)
			throw std::exception{ "Trying to set an out of bounds string list!" };
		auto iter = begin();
		std::advance(iter, pos);
		erase(iter);
	}

	std::string GetElem(size_t pos)
	{
		if (size() <= pos)
			throw std::exception{ "Trying to set an out of bounds string list!" };

		return operator[](pos);
	}

	void SetElem(size_t pos, const std::string& str)
	{
		if (size() <= pos)
			throw std::exception{ "Trying to set an out of bounds string list!" };
		operator[](pos) = str;
	}

	void RotateLeft()
	{
		// Simple rotation to the left
		std::rotate(begin(), begin() + 1, end());
	}

	void RotateRight()
	{
		// Simple rotation to the right
		std::rotate(rbegin(), rbegin() + 1, rend());
	}
};

struct PlantList : std::vector< Plant >
{
	// Customizing for the data structure

	void SetSize(size_t size)
	{
		resize(size);
	}

	void PushBack(const std::string& plantName, const std::string& filename, int amt)
	{
		emplace_back(plantName, filename, amt);
	}

	std::string GetPlantName(size_t pos)
	{
		if (size() <= pos)
			throw std::exception{ "Trying to set an out of bounds string list!" };

		return operator[](pos).name;
	}

	std::string GetFilename(size_t pos)
	{
		if (size() <= pos)
			throw std::exception{ "Trying to set an out of bounds string list!" };

		return operator[](pos).fileName;
	}

	int GetAmount(size_t pos)
	{
		if (size() <= pos)
			throw std::exception{ "Trying to set an out of bounds plant list!" };

		return operator[](pos).amount;
	}

	void SetPlantName(size_t pos, const std::string& plantName)
	{
		if (size() <= pos)
			throw std::exception{ "Trying to set an out of bounds plant list!" };
		operator[](pos).name = plantName;
	}

	void SetFilename(size_t pos, const std::string& filename)
	{
		if (size() <= pos)
			throw std::exception{ "Trying to set an out of bounds plant list!" };
		operator[](pos).fileName = filename;
	}

	void SetAmount(size_t pos, int amount)
	{
		if (size() <= pos)
			throw std::exception{ "Trying to set an out of bounds plant list!" };
		operator[](pos).amount = amount;
	}

	void Erase(size_t pos)
	{
		if (size() <= pos)
			throw std::exception{ "Trying to set an out of bounds plant list!" };
		auto iter = begin();
		std::advance(iter, pos);
		erase(iter);
	}

	void RotateLeft()
	{
		// Simple rotation to the left
		std::rotate(begin(), begin() + 1, end());
	}

	void RotateRight()
	{
		// Simple rotation to the right
		std::rotate(rbegin(), rbegin() + 1, rend());
	}
};