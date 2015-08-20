/*
** File Name: arithmetic.hpp
** Author:    Aditya Ramesh
** Date:      01/12/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z3C1D74E4_413C_46FC_856F_9957DBB6B4AF
#define Z3C1D74E4_413C_46FC_856F_9957DBB6B4AF

#include <ndmath/utility/operations.hpp>

namespace nd {

#define nd_define_arithmetic_exprs(symbol, name)                      \
                                                                      \
template <class Coord1, class Coord2>                                 \
CC_ALWAYS_INLINE constexpr                                            \
auto operator symbol                                                  \
(const coord_wrapper<Coord1> c1, const coord_wrapper<Coord2>& c2)     \
{                                                                     \
	using coord1_t = coord_wrapper<Coord1>;                       \
	using coord2_t = coord_wrapper<Coord2>;                       \
	using expr     = coord_expr<coord1_t, coord2_t, name>;        \
	using wrapper  = coord_wrapper<expr>;                         \
	return wrapper{in_place, c1, c2};                             \
}                                                                     \
                                                                      \
template <class Coord, class Integer>                                 \
CC_ALWAYS_INLINE constexpr                                            \
auto operator symbol (const coord_wrapper<Coord>& c, const Integer n) \
{                                                                     \
	using coord1_t = coord_wrapper<Coord>;                        \
	using coord2_t = coord_wrapper<coord<const Integer>>;         \
	using expr     = coord_expr<coord1_t, coord2_t, name>;        \
	using wrapper  = coord_wrapper<expr>;                         \
	return wrapper{in_place, c, make_c_coord(n)};                 \
}                                                                     \
                                                                      \
template <class Integer, class Coord>                                 \
CC_ALWAYS_INLINE constexpr                                            \
auto operator symbol (const Integer n, const coord_wrapper<Coord>& c) \
{                                                                     \
	using coord1_t = coord_wrapper<coord<const Integer>>;         \
	using coord2_t = coord_wrapper<Coord>;                        \
	using expr     = coord_expr<coord1_t, coord2_t, name>;        \
	using wrapper  = coord_wrapper<expr>;                         \
	return wrapper{in_place, make_c_coord(n), c};                 \
}

nd_define_arithmetic_exprs(+, detail::plus)
nd_define_arithmetic_exprs(-, detail::minus)
nd_define_arithmetic_exprs(*, detail::multiplies)
nd_define_arithmetic_exprs(/, detail::divides)
nd_define_arithmetic_exprs(%, detail::modulus)

#undef nd_define_arithmetic_ops

}

#endif
