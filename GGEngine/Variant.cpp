#include "stdafx.h"
#include "Variant.h"

/*
	SANITY CHECK
	------------
	1)  Ensure that the type that you want Variant to incorporate is included under 'VARIANTTYPES' in Common.h


	USE CASE EXAMPLES
	-----------------
	Variant v1{ "Hello World" };				// v1 is of type std::string (converts const char* / char* to std::string)
	v1 = 5;										// v1 is now int
	v1.set_value(3.1412);						// v1 is now double
	v1.get_value<float>();						// "silent error" but actually just returns nothing
	v1.get_pointer<float>();					// returns nullptr
	Variant v2{ v1 };							// v2 is now also double
	v1 == v2;									// returns true
	if (v1);									// if statement executes


	DOCUMENTATION
	-------------
	1) ALL Built-in types by default can be supported by Variant

	2) UDT that wants to be supported by Variant have to include the following functions:
		- Default Constructor
		- operator==(...) overloading

	3) get_value function DOES NOT not have generic function. It is required to have the explicit type out
	   e.g.
			Variant v1{ 5 };
			if (v1.type() == typeid(int))
				v1.get_value<int>();						// returns the reference to int 5
		Also, to note that it is illegal to do the following
			v1.get_value<float>();						// throws an error of std::bad_cast

	4) Able to call for LegalVariant() with a variable of a specific type to determine if Variant supports the type

	5) To check if the conversion is successful, consider using the get_pointer version instead (since it returns a nullptr)
 */

Variant::Variant(const Variant & rhs)
	: data{ }, type_id{ rhs.type_id }
{
	if (!rhs)
		return;

	auto func = [&rhs, this](auto&& elem)
	{
		using decayed = std::decay_t<decltype(elem)>;
		auto& rhs_elem = rhs.get_value<decayed>();
		data.reset(new VariantImpl<decayed>(rhs_elem));
	};
	visit(var_tuple, type_id, func);
}

Variant::Variant(Variant && rhs)
	: data{ std::move(rhs.data) }, type_id{ rhs.type_id }
{
	rhs.type_id = -1;
	rhs.data.reset();
}

Variant & Variant::operator=(const Variant & rhs)
{
	type_id = rhs.type_id;
	auto func = [&rhs, this](auto&& elem)
	{
		using decayed = std::decay_t<decltype(elem)>;
		auto& rhs_elem = rhs.get_value<decayed>();
		data.reset(new VariantImpl<decayed>(rhs_elem));
	};
	visit(var_tuple, type_id, func);
	return *this;
}

Variant & Variant::operator=(Variant && rhs)
{
	std::swap(type_id, rhs.type_id);
	data.swap(rhs.data);
	return *this;
}

bool Variant::has_value() const
{
	return !*this;
}

unsigned Variant::index() const
{
	return static_cast<unsigned>(type_id);
}

Variant::operator bool() const
{
	return data.get() != nullptr && type_id != -1;
}

std::type_index Variant::type() const
{
	return GetTypeIndex(type_id, var_tuple, std::index_sequence_for<VARIANTTYPES>{});
}

bool Variant::operator==(const Variant & rhs) const
{
	auto func = [&rhs, this](auto&& elem) -> bool
	{
		using decayed = std::decay_t<decltype(elem)>;
		const auto* ptr_lhs = dynamic_cast<const VariantImpl<decayed>*>(data.get());
		const auto* ptr_rhs = dynamic_cast<const VariantImpl<decayed>*>(rhs.data.get());
		if (!ptr_lhs || !ptr_rhs)
			return false;
		return ptr_lhs->_data == ptr_rhs->_data;
	};
	return visit(var_tuple, type_id, func);
}

bool Variant::operator!=(const Variant & rhs) const
{
	return !operator==(rhs);
}