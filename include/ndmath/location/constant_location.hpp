/*
** File Name: constant_location.hpp
** Author:    Aditya Ramesh
** Date:      01/08/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z4F124366_CD62_43F3_AFA2_59A5A5650F55
#define Z4F124366_CD62_43F3_AFA2_59A5A5650F55

#include <ndmath/location/location_base.hpp>

namespace nd {

template <size_t N>
class constant_location final :
public location_base<constant_location<N>>
{
public:
	using location_base<constant_location<N>>::operator();

	CC_ALWAYS_INLINE CC_CONST
	static constexpr auto eval(const size_t&)
	noexcept { return N; }
};

template <size_t N>
static constexpr auto c = constant_location<N>{};

}

#endif
