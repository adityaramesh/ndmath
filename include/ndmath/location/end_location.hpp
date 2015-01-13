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

class end_location final
{
public:
	template <class Integer>
	CC_ALWAYS_INLINE CC_CONST
	constexpr auto operator()(const Integer n)
	const noexcept { return n; }
};

namespace tokens {

static constexpr auto end = location_wrapper<end_location>{};

}

}

#endif
