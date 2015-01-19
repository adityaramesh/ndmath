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
	static constexpr auto allows_static_access =
	Loc1::allows_static_access &&
	Loc2::allows_static_access;
private:
	const Loc1 m_l1;
	const Loc2 m_l2;
public:
	CC_ALWAYS_INLINE constexpr
	explicit location_expr(const Loc1 l1, const Loc2 l2)
	noexcept : m_l1(l1), m_l2(l2) {}

	template <class Integer, nd_enable_if(allows_static_access)>
	CC_ALWAYS_INLINE CC_CONST constexpr
	static auto eval(const Integer n) noexcept
	{ return Op::apply(Loc1::eval(n), Loc2::eval(n)); }

	template <class Integer, nd_enable_if(!allows_static_access)>
	CC_ALWAYS_INLINE CC_CONST constexpr
	auto eval(const Integer n) const noexcept
	{ return Op::apply(m_l1.eval(n), m_l2.eval(n)); }
};

}

#endif
