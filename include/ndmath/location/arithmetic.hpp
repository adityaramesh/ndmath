/*
** File Name: arithmetic.hpp
** Author:    Aditya Ramesh
** Date:      01/12/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z3C1D74E4_413C_46FC_856F_9957DBB6B4AF
#define Z3C1D74E4_413C_46FC_856F_9957DBB6B4AF

#include <ndmath/arithmetic_functions.hpp>
#include <ndmath/location/location_wrapper.hpp>
#include <ndmath/location/location.hpp>
#include <ndmath/location/location_expr.hpp>

namespace nd {

#define nd_define_arithmetic_ops(symbol, name)                         \
                                                                       \
template <class Loc1, class Loc2>                                      \
CC_ALWAYS_INLINE CC_CONST constexpr                                    \
auto operator symbol                                                   \
(const location_wrapper<Loc1> l1, const location_wrapper<Loc2> l2)     \
{                                                                      \
	using w1 = location_wrapper<Loc1>;                             \
	using w2 = location_wrapper<Loc2>;                             \
	using expr = location_expr<name, w1, w2>;                      \
	using wrapper = location_wrapper<expr>;                        \
	return wrapper{l1, l2};                                        \
}                                                                      \
                                                                       \
template <class Loc>                                                   \
CC_ALWAYS_INLINE CC_CONST constexpr                                    \
auto operator symbol (const location_wrapper<Loc> loc, const size_t n) \
{                                                                      \
	using w1 = location_wrapper<Loc>;                              \
	using w2 = location_wrapper<location>;                         \
	using expr = location_expr<name, w1, w2>;                      \
	using wrapper = location_wrapper<expr>;                        \
	return wrapper{loc, w2{n}};                                    \
}                                                                      \
                                                                       \
template <class Loc>                                                   \
CC_ALWAYS_INLINE CC_CONST constexpr                                    \
auto operator symbol (const size_t n, const location_wrapper<Loc> loc) \
{                                                                      \
	using w1 = location_wrapper<location>;                         \
	using w2 = location_wrapper<Loc>;                              \
	using expr = location_expr<name, w1, w2>;                      \
	using wrapper = location_wrapper<expr>;                        \
	return wrapper{w1{n}, loc};                                    \
}

nd_define_arithmetic_ops(+, plus)
nd_define_arithmetic_ops(-, minus)
nd_define_arithmetic_ops(*, times)
nd_define_arithmetic_ops(/, divide)

#undef nd_define_arithmetic_ops

}

#endif
