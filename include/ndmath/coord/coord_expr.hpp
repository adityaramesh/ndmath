/*
** File Name: coord_expr.hpp
** Author:    Aditya Ramesh
** Date:      01/08/2015
** Contact:   _@adityaramesh.com
*/

#ifndef ZCB531135_D750_44CD_8E3C_8A1CA47831F5
#define ZCB531135_D750_44CD_8E3C_8A1CA47831F5

namespace nd {
namespace detail {

template <class Coord1, class Coord2, class Func, bool Enable>
struct coord_traits;

template <class Coord1, class Coord2, class Func>
struct coord_traits<Coord1, Coord2, Func, false>
{ using integer = void; };

template <class Func, class Coord1, class Coord2>
struct coord_traits<Coord1, Coord2, Func, true>
{
	using integer = std::result_of_t<Func(
		typename Coord1::integer,
		typename Coord2::integer
	)>;
};

}

template <class Coord1, class Coord2, class Func>
class coord_expr final
{
public:
	static constexpr auto allows_static_access =
	Coord1::allows_static_access &&
	Coord2::allows_static_access;

	static constexpr auto is_constant =
	Coord1::is_constant &&
	Coord2::is_constant;
private:
	using traits = detail::coord_traits<Coord1, Coord2, Func, is_constant>;
public:
	using integer = typename traits::integer;
private:
	Coord1 m_l1{};
	Coord2 m_l2{};
public:
	CC_ALWAYS_INLINE constexpr
	explicit coord_expr() noexcept {}

	CC_ALWAYS_INLINE constexpr
	explicit coord_expr(const Coord1 l1, const Coord2 l2)
	noexcept : m_l1{l1}, m_l2{l2} {}

	template <class Integer, nd_enable_if(allows_static_access)>
	CC_ALWAYS_INLINE constexpr
	static auto value(const Integer n) noexcept
	{ return Func{}(Coord1::value(n), Coord2::value(n)); }

	template <class Integer, nd_enable_if(!allows_static_access)>
	CC_ALWAYS_INLINE constexpr
	auto value(const Integer n) const noexcept
	{ return Func{}(m_l1.value(n), m_l2.value(n)); }
};

}

#endif
