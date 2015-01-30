/*
** File Name: coord_expr.hpp
** Author:    Aditya Ramesh
** Date:      01/08/2015
** Contact:   _@adityaramesh.com
*/

#ifndef ZCB531135_D750_44CD_8E3C_8A1CA47831F5
#define ZCB531135_D750_44CD_8E3C_8A1CA47831F5

namespace nd {

template <class Op, class Coord1, class Coord2>
class coord_expr final
{
public:
	static constexpr auto allows_static_access =
	Coord1::allows_static_access &&
	Coord2::allows_static_access;
private:
	const Coord1 m_l1;
	const Coord2 m_l2;
public:
	CC_ALWAYS_INLINE constexpr
	explicit coord_expr(const Coord1 l1, const Coord2 l2)
	noexcept : m_l1{l1}, m_l2{l2} {}

	template <class Integer, nd_enable_if(
		allows_static_access)>
	CC_ALWAYS_INLINE CC_CONST constexpr
	static auto value(const Integer n) noexcept
	{ return Op::apply(Coord1::value(n), Coord2::value(n)); }

	template <class Integer, nd_enable_if(
		!allows_static_access)>
	CC_ALWAYS_INLINE constexpr
	auto value(const Integer n) const noexcept
	{ return Op::apply(m_l1.value(n), m_l2.value(n)); }
};

}

#endif
