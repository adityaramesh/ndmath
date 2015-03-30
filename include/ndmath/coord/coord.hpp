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
public:
	static constexpr auto allows_static_access = true;
	static constexpr auto is_constant = true;
	using integer = Integer;

	CC_ALWAYS_INLINE constexpr
	explicit const_coord() noexcept {}

	template <class Integer_ = int>
	CC_ALWAYS_INLINE constexpr
	static auto value(Integer_ = 0) noexcept
	{ return N; }
};

template <class Integer>
class coord final
{
public:
	static constexpr auto allows_static_access = false;
	static constexpr auto is_constant = true;
	using integer = std::decay_t<Integer>;
private:
	integer n{};
public:
	CC_ALWAYS_INLINE constexpr
	explicit coord() noexcept {}

	CC_ALWAYS_INLINE constexpr
	explicit coord(const integer n)
	noexcept : n{n} {}

	template <class Integer_ = int>
	CC_ALWAYS_INLINE
	auto& value(Integer_ = 0) noexcept
	{ return n; }

	template <class Integer_ = int>
	CC_ALWAYS_INLINE constexpr
	auto& value(Integer_ = 0) const noexcept
	{ return n; }
};

template <class Integer>
CC_ALWAYS_INLINE constexpr
auto make_coord(const Integer n) noexcept
{ return coord_wrapper<coord<Integer>>{in_place, n}; }

template <class Integer>
CC_ALWAYS_INLINE constexpr
auto make_c_coord(const Integer n) noexcept
{ return coord_wrapper<coord<const Integer>>{in_place, n}; }

template <class Integer, Integer N>
static constexpr auto basic_sc_coord =
coord_wrapper<const_coord<Integer, N>>{};

template <unsigned N>
static constexpr auto sc_coord =
basic_sc_coord<unsigned, N>;

template <class Coord, nd_enable_if(Coord::allows_static_access)>
CC_ALWAYS_INLINE constexpr
auto eval(const coord_wrapper<Coord>&) noexcept
{
	using wrapper = coord_wrapper<Coord>;
	using integer = typename wrapper::integer;
	return basic_sc_coord<integer, wrapper::value()>;
}

template <class Coord, nd_enable_if(!Coord::allows_static_access)>
CC_ALWAYS_INLINE constexpr
auto eval(const Coord& c) noexcept
{
	using wrapper = coord_wrapper<Coord>;
	return make_coord(c.value());
}

namespace tokens {

template <unsigned N>
static constexpr auto c = sc_coord<N>;

}

}

#endif
