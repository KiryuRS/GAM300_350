#pragma once
#include "Common.h"

/*
	Capture ALL Variant Type(s) - Include in the type of the Variant if you want that type to be part of the Variant type class.
	Refer to Variant.cpp for more information on how to use it

	TLDR: Ensure that the type supports:
	1) Default Ctor
	2) operator== overloading
*/
#define VARIANTTYPES \
bool, char, short, int, long, long long, size_t, \
unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long, \
float, double, std::string, Vector2, Vector3, Vector4

namespace
{
	// Storing the pre-defined types in a tuple
	std::tuple<VARIANTTYPES> var_tuple;

	// Get the index of the type based from a tuple
	template <typename Search, typename ... Args, size_t ... N>
	int GetSearchedIndex(const std::tuple<Args...>& tup, std::index_sequence<N...>)
	{
		// Create the array of boolean comparing the type passed in
		std::array<bool, sizeof...(Args)> arr
		{
			(typeid(Search) == typeid(std::get<N>(tup))) ...
		};
		// Search for a boolean with value "true" (should have only one)
		auto iter = std::find(arr.begin(), arr.end(), true);
		// Return the index
		if (iter != arr.end())
			return iter - arr.begin();
		// Or indicate that the type isn't found
		return -1;
	}

	// Get the type in type_index form based from a tuple
	template <typename ... Args, size_t ... N>
	std::type_index GetTypeIndex(int index, const std::tuple<Args...>& tup, std::index_sequence<N...>)
	{
		std::array<std::type_index, sizeof...(Args)> arr
		{
			(typeid(std::get<N>(tup)))...
		};

		return arr[index];
	}

	// Helper function for checking if Variant supports the type
	template <typename T, typename ... Args, size_t ... Is>
	bool LegalVariantHelper(T&& arg, const std::tuple<Args...>& tup, std::index_sequence<Is...>)
	{
		return [](const auto& a0, const auto& ... rest)
		{
			return ((typeid(a0) == typeid(rest)) || ...);
		}(arg, std::get<Is>(tup)...);
	}

	// Function to "visit" the required element in the tuple
	template <typename Tuple, typename Functor, size_t N>
	std::invoke_result_t<Functor, decltype(std::get<N>(std::declval<Tuple>()))>	// or just use decltype(auto)
	visit_one(Tuple& tup, Functor func)
	{
		return func(std::get<N>(tup));
	}

	// Generic function that creates a vast of function pointers for tuple accessment
	template <typename Tuple, typename Functor, size_t ... Is>
	decltype(auto) visit_helper(Tuple& tup, size_t index, Functor func, std::index_sequence<Is...>)
	{
		// Create the instances of the function pointers
		using type = std::common_type_t<decltype(visit_one<Tuple, Functor, Is>(tup, func))...>;
		using FT = type(*)(Tuple&, Functor);
		static constexpr std::array<FT, std::tuple_size<Tuple>::value> arr =
		{
			&visit_one<Tuple, Functor, Is>...
		};
		// Access the current type in runtime through a function
		return arr[index](tup, func);
	}
}

// User's function
template <typename Functor, typename ... Args>
decltype(auto) visit(std::tuple<Args...>& tup, size_t index, Functor func)
{
	return visit_helper(tup, index, func, std::index_sequence_for<Args...>{});
}

// Function to determine if Variant supports the current type
template <typename T, typename U = std::decay_t<T>>
bool LegalVariant(T&& arg)
{
	// Check if its a const char* or char*
	if constexpr (std::is_same<char*, U>::value || std::is_same<const char*, U>::value)
		return true;
	else
		// Otherwise execute the helper to aid in checking against all of the available types
		return LegalVariantHelper(std::forward<T>(arg), var_tuple, std::index_sequence_for<VARIANTTYPES>{});
}

class Variant final
{
	struct AbstractImpl
	{
		virtual AbstractImpl& operator=(const AbstractImpl&) = default;
		virtual ~AbstractImpl()
		{ }
	};

	template <typename T>
	struct VariantImpl : public AbstractImpl
	{
		T _data;

		VariantImpl(const T& value)
			: _data{ value }
		{ }

	};

	std::unique_ptr<AbstractImpl> data;				// our current stored data
	int type_id = -1;								// current index based off the tuple

public:
	Variant() = default;
	Variant(const Variant& rhs);
	Variant(Variant&& rhs);

	template <typename T, typename U = std::decay_t<T>>
	Variant(const T& value)
		: data{ },
		  type_id{ }
	{
		// First, determine if its a legal variant
		TOOLS::Assert(LegalVariant(value), "Unable to convert this type into a Variant!");

		// Checking if it's a const char* or char* (we will convert it into a std::string)
		constexpr bool char_star_check = std::is_same<char*, U>::value || std::is_same<const char*, U>::value;
		if constexpr (char_star_check)
		{
			type_id = GetSearchedIndex<std::string>(var_tuple, std::index_sequence_for<VARIANTTYPES>{});
			data.reset(new VariantImpl<std::string>(value));
		}
		// Otherwise it is defaulted to its own type
		else
		{
			type_id = GetSearchedIndex<T>(var_tuple, std::index_sequence_for<VARIANTTYPES>{});
			data.reset(new VariantImpl<U>(value));
		}
	}

	Variant& operator=(const Variant& rhs);
	Variant& operator=(Variant&& rhs);

	template <typename T>
	Variant& operator=(const T& value)
	{
		*this = std::move(Variant{ value });
		return *this;
	}

	template <typename T>
	T* get_pointer()
	{
		auto* ptr = dynamic_cast<VariantImpl<T>*>(data.get());
		return (!ptr) ? nullptr : &ptr->_data;
	}

	template <typename T>
	const T* get_pointer() const
	{
		const auto* ptr = dynamic_cast<const VariantImpl<T>*>(data.get());
		return (!ptr) ? nullptr : &ptr->_data;
	}

	template <typename T, typename U = std::decay_t<T>>
	constexpr U& get_value()
	{
		return static_cast<VariantImpl<U>&>(*data.get())._data;
	}

	template <typename T, typename U = std::decay_t<T>>
	constexpr const U& get_value() const
	{
		return static_cast<VariantImpl<U>&>(*data.get())._data;
	}

	template <typename T>
	void set_value(const T& value)
	{
		operator=(value);
	}

	bool has_value() const;
	unsigned index() const;
	operator bool() const;

	template <typename T, typename U = std::decay_t<T>>
	constexpr operator T() const
	{
		return static_cast<VariantImpl<U>&>(*data.get())._data;
	}

	std::type_index type() const;

	template <typename T>
	bool operator==(const T& value) const
	{
		if (same_type(value))
			return get_value<T>() == value;
		return false;
	}

	template <typename T>
	bool operator!=(const T& value) const
	{
		return !operator==(value);
	}

	bool operator==(const Variant& rhs) const;
	bool operator!=(const Variant& rhs) const;

	template <typename T>
	bool same_type(const T& value) const
	{
		if (type_id == -1)
			return false;
		return GetTypeIndex(type_id, var_tuple, std::index_sequence_for<VARIANTTYPES>{}) == typeid(value);
	}
};