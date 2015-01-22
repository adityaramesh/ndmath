/*
** File Name: location_expr.hpp
** Author:    Aditya Ramesh
** Date:      01/08/2015
** Contact:   _@adityaramesh.com
*/

#ifndef ZCB531135_D750_44CD_8E3C_8A1CA47831F5
#define ZCB531135_D750_44CD_8E3C_8A1CA47831F5

namespace nd {

template <class Op, class Loc1, class Loc2>
class location_expr final
{
public:
	static constexpr auto is_constant =
	Loc1::is_constant &&
	Loc2::is_constant;

	static constexpr auto allows_static_access =
	Loc1::allows_static_access &&
	Loc2::allows_static_access;
private:
	const Loc1 m_l1;
	const Loc2 m_l2;
public:
	CC_ALWAYS_INLINE constexpr
	explicit location_expr(const Loc1 l1, const Loc2 l2)
	noexcept : m_l1{l1}, m_l2{l2} {}

	template <class Integer, nd_enable_if(
		allows_static_access && is_constant
	)>
	CC_ALWAYS_INLINE CC_CONST constexpr
	static auto value(Integer) noexcept
	{ return Op::apply(Loc1::value(), Loc2::value()); }

	template <class Integer, nd_enable_if(
		allows_static_access && !is_constant
	)>
	CC_ALWAYS_INLINE CC_CONST constexpr
	static auto value(const Integer n) noexcept
	{ return Op::apply(Loc1::value(n), Loc2::value(n)); }

	template <class Integer, nd_enable_if(
		!allows_static_access && is_constant
	)>
	CC_ALWAYS_INLINE constexpr
	auto value(Integer) const noexcept
	{ return Op::apply(m_l1.value(), m_l2.value()); }

	template <class Integer, nd_enable_if(
		!allows_static_access && !is_constant
	)>
	CC_ALWAYS_INLINE constexpr
	auto value(const Integer n) const noexcept
	{ return Op::apply(m_l1.value(n), m_l2.value(n)); }
};

}

#endif
