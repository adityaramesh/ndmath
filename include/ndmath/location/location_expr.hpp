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
	const Loc1 m_l1;
	const Loc2 m_l2;
public:
	CC_ALWAYS_INLINE CC_CONST constexpr
	explicit location_expr(const Loc1 l1, const Loc2 l2)
	noexcept : m_l1(l1), m_l2(l2) {}

	template <class Integer>
	CC_ALWAYS_INLINE CC_CONST
	constexpr auto operator()(const Integer& n)
	const noexcept { return Op::apply(m_l1(n), m_l2(n)); }
};

}

#endif
