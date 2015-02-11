/*
** File Name: end_coord.hpp
** Author:    Aditya Ramesh
** Date:      01/08/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z2B161C0A_2370_45EE_A9D9_37D5AF216993
#define Z2B161C0A_2370_45EE_A9D9_37D5AF216993

#include <ndmath/coord/coord_wrapper.hpp>

namespace nd {

struct end_coord final
{
	static constexpr auto allows_static_access = true;
	static constexpr auto is_constant = false;
	using integer = void;

	template <class Integer>
	CC_ALWAYS_INLINE constexpr
	static auto value(const Integer n) noexcept
	{ return Integer(n); }
};

namespace tokens {

static constexpr auto end =
coord_wrapper<end_coord>{in_place};

}

}

#endif
