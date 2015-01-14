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
	template <class Integer>
	CC_ALWAYS_INLINE CC_CONST constexpr
	static auto eval(const Integer n) noexcept
	{ return n; }
};

namespace tokens {

static constexpr auto end = const_location_wrapper<end_location>{};

}

}

#endif
