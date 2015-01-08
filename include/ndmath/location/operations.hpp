/*
** File Name: operations.hpp
** Author:    Aditya Ramesh
** Date:      01/08/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z0801DF89_4E7C_4FA7_83F3_A43D34D22E18
#define Z0801DF89_4E7C_4FA7_83F3_A43D34D22E18

#include <cstddef>
#include <ccbase/platform.hpp>

namespace nd {

#define ND_DEFINE_OPERATION(symbol, name)                         \
	struct name final                                         \
	{                                                         \
		CC_ALWAYS_INLINE CC_CONST constexpr               \
		auto operator()(const size_t& a, const size_t& b) \
		const noexcept { return a symbol b; }             \
	};                                                        \

ND_DEFINE_OPERATION(+, plus)
ND_DEFINE_OPERATION(-, minus)
ND_DEFINE_OPERATION(*, times)
ND_DEFINE_OPERATION(/, divide)
ND_DEFINE_OPERATION(%, mod)

#undef ND_DEFINE_OPERATION

}

#endif
