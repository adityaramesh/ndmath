/*
** File Name: arithmetic.hpp
** Author:    Aditya Ramesh
** Date:      01/12/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z3C1D74E4_413C_46FC_856F_9957DBB6B4AF
#define Z3C1D74E4_413C_46FC_856F_9957DBB6B4AF

#include <type_traits>
#include <ndmath/arithmetic_functions.hpp>
#include <ndmath/location/location_wrapper.hpp>
#include <ndmath/location/location.hpp>
#include <ndmath/location/location_expr.hpp>

namespace nd {

#define nd_define_arithmetic_ops(symbol, name)                                  \
                                                                                \
template <class Loc1, class Loc2>                                               \
CC_ALWAYS_INLINE CC_CONST constexpr                                             \
auto operator symbol                                                            \
(const location_wrapper<Loc1> l1, const location_wrapper<Loc2> l2)              \
{                                                                               \
	using expr = location_expr<name, Loc1, Loc2>;                           \
	using wrapper = location_wrapper<expr>;                                 \
	return wrapper{l1, l2};                                                 \
}                                                                               \
                                                                                \
template <class Loc>                                                            \
CC_ALWAYS_INLINE CC_CONST constexpr                                             \
auto operator symbol (const location_wrapper<Loc> loc, const size_t n)          \
{                                                                               \
	using expr = location_expr<name, Loc, location>;                        \
	using wrapper = location_wrapper<expr>;                                 \
	return wrapper{loc.value(), location{n}};                               \
}                                                                               \
                                                                                \
template <class Loc>                                                            \
CC_ALWAYS_INLINE CC_CONST constexpr                                             \
auto operator symbol (const size_t n, const location_wrapper<Loc> loc)          \
{                                                                               \
	using expr = location_expr<name, location, Loc>;                        \
	using wrapper = location_wrapper<expr>;                                 \
	return wrapper{location{n}, loc.value()};                               \
}                                                                               \
                                                                                \
template <class Loc>                                                            \
CC_ALWAYS_INLINE CC_CONST constexpr                                             \
auto operator symbol (const const_location_wrapper<Loc> loc, const size_t n)    \
{                                                                               \
	using w1 = std::decay_t<decltype(loc)>;                                 \
	using expr = location_expr<name, w1, location>;                         \
	using w2 = location_wrapper<expr>;                                      \
	return w2{loc, location{n}};                                            \
}                                                                               \
                                                                                \
template <class Loc>                                                            \
CC_ALWAYS_INLINE CC_CONST constexpr                                             \
auto operator symbol (const size_t n, const const_location_wrapper<Loc> loc)    \
{                                                                               \
	using w1 = std::decay_t<decltype(loc)>;                                 \
	using expr = location_expr<name, location, w1>;                         \
	using w2 = location_wrapper<expr>;                                      \
	return w2{location{n}, loc};                                            \
}                                                                               \
                                                                                \
template <class Loc1, class Loc2>                                               \
CC_ALWAYS_INLINE CC_CONST constexpr                                             \
auto operator symbol                                                            \
(const const_location_wrapper<Loc1>, const const_location_wrapper<Loc2>)        \
{                                                                               \
	using expr = const_location_expr<name, Loc1, Loc2>;                     \
	return const_location_wrapper<expr>{};                                  \
}

nd_define_arithmetic_ops(+, plus)
nd_define_arithmetic_ops(-, minus)
nd_define_arithmetic_ops(*, times)
nd_define_arithmetic_ops(/, divide)

#undef nd_define_arithmetic_ops

}

#endif
