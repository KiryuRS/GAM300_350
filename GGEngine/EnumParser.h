#pragma once
#include <string>
#include <vector>
#include <map>

template <typename Enum>
struct EnumConversions;

template <typename Enum>
class EnumManager
{
	friend struct EnumConversions<Enum>;

	// Alias
	using ENUM_TO_STRING = std::map<Enum, std::string>;
	using STRING_TO_ENUM = std::map<std::string, Enum>;
	using ENUMSTRING_PAIR = std::pair<Enum, std::string>;

	ENUM_TO_STRING ets_map;
	STRING_TO_ENUM ste_map;

	void InitEnumToStringMap(const std::vector<ENUMSTRING_PAIR> &vector)
	{
		for (auto& elem : vector)
			ets_map[elem.first] = elem.second;
	}

	void InitStringToEnumMap(const std::vector<ENUMSTRING_PAIR> &vector)
	{
		for (auto& elem : vector)
			ste_map[elem.second] = elem.first;
	}

protected:
	void Init(const std::vector<ENUMSTRING_PAIR> &vector)
	{
		InitStringToEnumMap(vector);
		InitEnumToStringMap(vector);
	}

public:
	std::string GetString(const Enum& en) const
	{
		auto iter = ets_map.find(en);
		if (iter != ets_map.end())
			return iter->second;

		return "Error: [ENUM NOT FOUND!]";
	}

	Enum GetEnum(const std::string& str)
	{
		auto iter = ste_map.find(str);

		if (iter == ste_map.end())
		{
			// CONSOLE_ERROR("ENUM not found!");
			return static_cast<Enum>(0);
		}

		return iter->second;
	}
};

template <typename Enum>
struct EnumConversions : public EnumManager<Enum>
{
	// Alias
	using ENUM_TO_STRING = std::map<Enum, std::string>;
	using STRING_TO_ENUM = std::map<std::string, Enum>;
	using ENUMSTRING_PAIR = std::pair<Enum, std::string>;
	std::vector<ENUMSTRING_PAIR> ets_vector;

	// Iterator Constructor
	template <typename InputIt>
	EnumConversions(InputIt begin, InputIt end)
	{
		while (begin != end)
			ets_vector.emplace_back(*begin++);
	}

public:
	// Default Constructor
	EnumConversions() {};

	// List Initialization
	EnumConversions(std::initializer_list<ENUMSTRING_PAIR> ll)
		: EnumConversions{ ll.begin(), ll.end() }
	{
		// Initialize our manager through the vector
		EnumManager<Enum>::Init(ets_vector);
	}

	ENUM_TO_STRING GetPairing() const
	{
		return EnumManager<Enum>::ets_map;
	}

	STRING_TO_ENUM GetAlternatePairing() const
	{
		return EnumManager<Enum>::ste_map;
	}
};

/*
Test with the following:

enum X { MON, TUE, WED, THU, FRI };
X test = WED;
EnumConversions<X> ec{ {MON, "Monday"}, {TUE, "Tuesday"}, {WED, "Wednesday"}, {THU, "Thursday"}, {FRI, "Friday"} };
std::cout << ec.GetString(test) << std::endl;
std::cout << ec.GetEnum("Monday") << std::endl;

// Print out is Wednesday

*/