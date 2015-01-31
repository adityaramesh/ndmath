/*
** File Name: arithmetic_functions.hpp
** Author:    Aditya Ramesh
** Date:      01/08/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z0801DF89_4E7C_4FA7_83F3_A43D34D22E18
#define Z0801DF89_4E7C_4FA7_83F3_A43D34D22E18

#include <cstddef>
#include <ccbase/platform.hpp>

namespace nd {

#define nd_define_operation(symbol, name)          \
	struct name final                          \
	{                                          \
		template <class T, class U>        \
		CC_ALWAYS_INLINE CC_CONST          \
		static constexpr                   \
		auto apply(const T& a, const U& b) \
		noexcept { return a symbol b; }    \
	};

nd_define_operation(+, plus)
nd_define_operation(-, minus)
nd_define_operation(*, times)
nd_define_operation(/, divide)
nd_define_operation(%, modulus)

#undef nd_define_operation

}

#endif
