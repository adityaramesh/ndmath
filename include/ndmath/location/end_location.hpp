/*
** File Name: end_location.hpp
** Author:    Aditya Ramesh
** Date:      01/08/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z2B161C0A_2370_45EE_A9D9_37D5AF216993
#define Z2B161C0A_2370_45EE_A9D9_37D5AF216993

#include <ndmath/location/location_base.hpp>

namespace nd {

class end_location final :
public location_base<end_location>
{
public:
	using location_base<end_location>::operator();

	CC_ALWAYS_INLINE CC_CONST
	static constexpr auto eval(const size_t& n)
	noexcept { return n; }
};

namespace tokens {

static constexpr auto end = end_location{};

}

}

#endif
