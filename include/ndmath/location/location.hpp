/*
** File Name: location.hpp
** Author:    Aditya Ramesh
** Date:      01/08/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z4F124366_CD62_43F3_AFA2_59A5A5650F55
#define Z4F124366_CD62_43F3_AFA2_59A5A5650F55

#include <ndmath/location/location_wrapper.hpp>

namespace nd {

class location final
{
	const size_t n;
public:
	CC_ALWAYS_INLINE constexpr
	explicit location(const size_t n)
	noexcept : n{n} {}

	template <class Integer>
	CC_ALWAYS_INLINE CC_CONST
	constexpr auto operator()(Integer)
	const noexcept { return Integer(n); }
};

template <size_t N>
struct const_location final
{
	CC_ALWAYS_INLINE CC_CONST constexpr
	explicit const_location() noexcept {}

	template <class Integer>
	CC_ALWAYS_INLINE CC_CONST constexpr
	static auto eval(Integer)
	noexcept { return Integer(N); }
};

CC_ALWAYS_INLINE constexpr
auto make_location(const size_t n) noexcept
{ return location_wrapper<location>{n}; }

namespace tokens {

template <size_t N>
static constexpr auto c =
const_location_wrapper<const_location<N>>{};

}

}

#endif
