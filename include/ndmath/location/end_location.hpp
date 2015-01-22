/*
** File Name: end_location.hpp
** Author:    Aditya Ramesh
** Date:      01/08/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z2B161C0A_2370_45EE_A9D9_37D5AF216993
#define Z2B161C0A_2370_45EE_A9D9_37D5AF216993

#include <ndmath/location/location_wrapper.hpp>

namespace nd {

struct end_location final
{
	static constexpr auto is_constant = false;
	static constexpr auto allows_static_access = true;

	template <class Integer>
	CC_ALWAYS_INLINE CC_CONST constexpr
	static auto value(const Integer n) noexcept
	{ return Integer(n); }
};

namespace tokens {

static constexpr auto end =
location_wrapper<end_location>{in_place};

}

}

#endif
