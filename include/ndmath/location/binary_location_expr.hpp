/*
** File Name: binary_location_expr.hpp
** Author:    Aditya Ramesh
** Date:      01/08/2015
** Contact:   _@adityaramesh.com
*/

#ifndef ZCB531135_D750_44CD_8E3C_8A1CA47831F5
#define ZCB531135_D750_44CD_8E3C_8A1CA47831F5

#include <ndmath/location/location_base.hpp>

namespace nd {

template <class Op, class Loc1, class Loc2>
class binary_location_expr :
public location_base<binary_location_expr<Op, Loc1, Loc2>>
{
public:
	using location_base<binary_location_expr<Op, Loc1, Loc2>>::operator();

	CC_ALWAYS_INLINE CC_CONST
	static constexpr auto eval(const size_t& n)
	noexcept { return Op::apply(Loc1::eval(n), Loc2::eval(n)); }
};

}

#endif
