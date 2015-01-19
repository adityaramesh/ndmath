/*
** File Name: location.hpp
** Author:    Aditya Ramesh
** Date:      01/08/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z4F124366_CD62_43F3_AFA2_59A5A5650F55
#define Z4F124366_CD62_43F3_AFA2_59A5A5650F55

namespace nd {

template <size_t N>
struct const_location final
{
	static constexpr auto allows_static_access = true;

	CC_ALWAYS_INLINE CC_CONST constexpr
	explicit const_location() noexcept {}

	template <class Integer>
	CC_ALWAYS_INLINE CC_CONST constexpr
	static auto eval(Integer) noexcept
	{ return Integer(N); }
};

class location final
{
public:
	static constexpr auto allows_static_access = false;
private:
	const size_t n;
public:
	CC_ALWAYS_INLINE constexpr
	explicit location(const size_t n)
	noexcept : n{n} {}

	template <class Integer>
	CC_ALWAYS_INLINE CC_CONST constexpr
	auto eval(Integer) const noexcept
	{ return Integer(n); }
};

CC_ALWAYS_INLINE constexpr
auto make_location(const size_t n) noexcept
{ return location_wrapper<location>{n}; }

namespace tokens {

template <size_t N>
static constexpr auto c =
location_wrapper<const_location<N>>{};

}

}

#endif
