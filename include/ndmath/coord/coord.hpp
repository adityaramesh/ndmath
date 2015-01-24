/*
** File Name: coord.hpp
** Author:    Aditya Ramesh
** Date:      01/08/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z4F124366_CD62_43F3_AFA2_59A5A5650F55
#define Z4F124366_CD62_43F3_AFA2_59A5A5650F55

namespace nd {

template <class Integer, Integer N>
struct const_coord final
{
	static constexpr auto is_constant = true;
	static constexpr auto allows_static_access = true;

	CC_ALWAYS_INLINE CC_CONST constexpr
	explicit const_coord() noexcept {}

	CC_ALWAYS_INLINE CC_CONST constexpr
	static auto value() noexcept
	{ return N; }
};

template <class Integer>
class coord final
{
	using value_type = std::decay_t<Integer>;
public:
	static constexpr auto is_constant = true;
	static constexpr auto allows_static_access = false;
	using result = Integer;
	using const_result = const value_type&;
private:
	const Integer n;
public:
	CC_ALWAYS_INLINE constexpr
	explicit coord(const Integer n)
	noexcept : n{n} {}

	CC_ALWAYS_INLINE
	result value() noexcept
	{ return n; }

	CC_ALWAYS_INLINE constexpr
	const_result value() const noexcept
	{ return n; }
};

template <class Integer>
CC_ALWAYS_INLINE constexpr
auto make_coord(const Integer n) noexcept
{ return coord_wrapper<coord<const Integer>>{in_place, n}; }

template <class Integer, Integer N>
static constexpr auto basic_ccoord =
coord_wrapper<const_coord<Integer, N>>{};

template <uint_fast32_t N>
static constexpr auto ccoord =
basic_ccoord<uint_fast32_t, N>;

namespace tokens {

template <uint_fast32_t N>
static constexpr auto c = ccoord<N>;

}

}

#endif
