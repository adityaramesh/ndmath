/*
** File Name: arithmetic.hpp
** Author:    Aditya Ramesh
** Date:      01/12/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z3C1D74E4_413C_46FC_856F_9957DBB6B4AF
#define Z3C1D74E4_413C_46FC_856F_9957DBB6B4AF

#include <ndmath/utility/arithmetic_functions.hpp>

namespace nd {

#define nd_define_arithmetic_ops(symbol, name)                                       \
                                                                                     \
template <class Loc1, class Loc2>                                                    \
CC_ALWAYS_INLINE constexpr                                                           \
auto operator symbol                                                                 \
(const location_wrapper<Loc1> l1, const location_wrapper<Loc2> l2)                   \
{                                                                                    \
	using loc1_t = location_wrapper<Loc1>;                                       \
	using loc2_t = location_wrapper<Loc2>;                                       \
	using expr = location_expr<name, loc1_t, loc2_t>;                            \
	using wrapper = location_wrapper<expr>;                                      \
	return wrapper{in_place, l1, l2};                                            \
}                                                                                    \
                                                                                     \
template <class Loc, class Integer>                                                  \
CC_ALWAYS_INLINE constexpr                                                           \
auto operator symbol (const location_wrapper<Loc> loc, const Integer n)              \
{                                                                                    \
	using loc1_t = location_wrapper<Loc>;                                        \
	using loc2_t = location_wrapper<location<const Integer>>;                    \
	using expr = location_expr<name, loc1_t, loc2_t>;                            \
	using wrapper = location_wrapper<expr>;                                      \
	return wrapper{in_place, loc, make_location(n)};                             \
}                                                                                    \
                                                                                     \
template <class Integer, class Loc>                                                  \
CC_ALWAYS_INLINE constexpr                                                           \
auto operator symbol (const Integer n, const location_wrapper<Loc> loc)              \
{                                                                                    \
	using loc1_t = location_wrapper<location<const Integer>>;                    \
	using loc2_t = location_wrapper<Loc>;                                        \
	using expr = location_expr<name, loc1_t, loc2_t>;                            \
	using wrapper = location_wrapper<expr>;                                      \
	return wrapper{in_place, make_location(n), loc};                             \
}

nd_define_arithmetic_ops(+, plus)
nd_define_arithmetic_ops(-, minus)
nd_define_arithmetic_ops(*, times)
nd_define_arithmetic_ops(/, divide)
nd_define_arithmetic_ops(%, modulus)

#undef nd_define_arithmetic_ops

}

#endif
