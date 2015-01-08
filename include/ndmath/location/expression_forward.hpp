/*
** File Name: expression_forward.hpp
** Author:    Aditya Ramesh
** Date:      01/08/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z65B25ADD_6DD9_4438_9276_49809AB6369B
#define Z65B25ADD_6DD9_4438_9276_49809AB6369B

#include <ndmath/location/operations.hpp>

namespace nd {

template <class Op, class Loc1, class Loc2>
class binary_location_expr;

template <class Derived>
class location_base;

#define ND_DEFINE_OPERATOR(symbol, name)                       \
	template <class Derived1, class Derived2>              \
	CC_ALWAYS_INLINE CC_CONST                              \
	constexpr auto operator symbol (                       \
		const location_base<Derived1>& d1,             \
		const location_base<Derived2>& d2              \
	) noexcept                                             \
	{                                                      \
		return binary_location_expr<                   \
			name,                                  \
			std::remove_reference_t<decltype(d1)>, \
			std::remove_reference_t<decltype(d2)>  \
		>{d1, d2};                                     \
	}

ND_DEFINE_OPERATOR(+, plus)
ND_DEFINE_OPERATOR(-, minus)
ND_DEFINE_OPERATOR(*, times)
ND_DEFINE_OPERATOR(/, divide)
ND_DEFINE_OPERATOR(%, mod)

#undef ND_DEFINE_OPERATOR

}

#endif
