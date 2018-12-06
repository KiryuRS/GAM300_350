#pragma once

namespace MySTL
{
	template<typename, typename T>
	struct contains_inequality {
		static_assert(
			std::integral_constant<T, false>::value,
			"Second template parameter needs to be of function type.");
	};

	// specialization that does the checking

	template<typename C, typename Ret, typename... Args>
	struct contains_inequality<C, Ret(Args...)> {
	private:
		template<typename T>
		static constexpr auto check(T*)
			-> typename
			std::is_same<
			decltype(std::declval<T>().operator!=(std::declval<Args>()...)),
			Ret>::type; 

		template<typename>
		static constexpr std::false_type check(...);

		typedef decltype(check<C>(0)) type;

	public:
		static constexpr bool value = type::value;
	};
	// a class that contains a class of types
	template<class...>
	struct types
	{
		using type = types;
	};

	// Extended version of the struct types
	template <typename ... Args>
	using types_t = typename types<Args...>::type;

	// a type that does nothing but carry a type around without being the type
	template <typename T>
	struct tag
	{
		using type = T;
	};

	// Extended version of struct tag
	template<class Tag>
	using tag_t = typename Tag::type;

#pragma region TEMPLATE_INDEX
	// a shorter way to say `std::integral_constant<size_t, x>`:
	template <size_t i>
	struct index : std::integral_constant<size_t, i>
	{ };

	template <typename... >
	struct typeIndex;

	// found it
	template <typename T, typename... R>
	struct typeIndex<T, T, R...>
		: std::integral_constant<size_t, 0>
	{ };

	// still looking
	template <typename T, typename F, typename... R>
	struct typeIndex<T, F, R...>
		: std::integral_constant<size_t, 1 + typeIndex<T, R...>::value>
	{ };
	
	template <typename Search, typename ... Args>
	struct ContainType;

	template <typename Search, typename T, typename ... Args>
	struct ContainType<Search, T, Args...>
	{
		static constexpr bool value = std::is_same<Search, T>::value ? true :
									  ContainType<Search, Args...>::value;
	};

	template <typename Search, typename T>
	struct ContainType<Search, T>
	{
		static constexpr bool value = std::is_same<Search, T>::value;
	};

#pragma endregion index grabber

#pragma region OVERLOAD_FUNCTIONS

	// https://dev.to/tmr232/that-overloaded-trick-overloading-lambdas-in-c17

	template <typename ... Functors>
	struct overloaded_functions : Functors...	{ using Functors::operator()...; };

	template <typename ... Functors>
	overloaded_functions(Functors...) -> overloaded_functions<Functors...>;

#pragma endregion Allow functions to execute after one another

#pragma region FUNCTION_TRAITS
	// Generic type - use the result of operator() signature
	template <typename T>
	struct function_traits : function_traits< decltype(&T::operator()) >
	{ };

	// Specialize for pointers to const member functions
	template <typename ClassType, typename ReturnType, typename ... Args>
	struct function_traits< ReturnType(ClassType::*)(Args...) const>
	{
		// Number of arguments
		enum { arity = sizeof...(Args) };

		using result_type = ReturnType;

		// the i-th argument is equivalent to the i-th tuple element of a tuple
		// composed of those arguments.
		template <size_t i>
		struct GetArg
		{
			using type = std::tuple_element_t<i, std::tuple<Args...>>;
		};
	};

	// Specialize for pointers to non const member functions
	template <typename ClassType, typename ReturnType, typename ... Args>
	struct function_traits< ReturnType(ClassType::*)(Args...) >
	{
		// Number of arguments
		enum { arity = sizeof...(Args) };

		using result_type = ReturnType;

		// the i-th argument is equivalent to the i-th tuple element of a tuple
		// composed of those arguments.
		template <size_t i>
		struct GetArg
		{
			using type = std::tuple_element_t<i, std::tuple<Args...>>;
		};
	};
#pragma endregion Grab Details of a function

	// Finding the largest type for all of the argument types passed in
	template <typename T, typename ... Args>
	struct largest_type
	{
		using type =
		std::conditional_t
		<
			(sizeof(T) > sizeof(typename largest_type<Args...>::type)),
			T,
			typename largest_type<Args...>::type
		>;
	};

	// Base case
	template <typename T>
	struct largest_type<T>
	{
		using type = T;
	};

	template <typename T, typename ... Args>
	using largest_type_t = typename largest_type<T, Args...>::type;

#pragma region TYPE_NAME
	template <typename T>
	constexpr std::string_view type_name(T&&)
	{
		return type_name<T>();
	}

	template <typename T>
	constexpr std::string_view type_name(void)
	{
#if defined(__GNUC__)
		std::string_view p{ __PRETTY_FUNCTION__ };
		return std::string_view{ p.data() + 49, p.find(';', 49) - 49 };
#elif defined(_MSC_VER)
		std::string_view p{ __FUNCSIG__ };
		return std::string_view{ p.data() + 84, p.size() - 84 - 7 };
#endif
	}
#pragma endregion Get the name of a type (without typeid)
	
	// Generic class that holds any type
	class generic_t
	{
		void *data;

	public:
		generic_t()
			: data{ nullptr }
		{ }

		template <typename T, typename NCT = std::remove_const_t<T>>
		generic_t(T& _data)
			: data{ static_cast<void*>(const_cast<NCT*>(&_data)) }
		{ }

		template <typename T>
		T& GetData()
		{
			return *static_cast<T*>(data);
		}
	};
}