/*
** File Name: operations.hpp
** Author:    Aditya Ramesh
** Date:      01/08/2015
** Contact:   _@adityaramesh.com
**
** TODO: unary operations.
*/

#ifndef Z0801DF89_4E7C_4FA7_83F3_A43D34D22E18
#define Z0801DF89_4E7C_4FA7_83F3_A43D34D22E18

namespace nd {
namespace detail {

#define nd_define_unary_op(symbol, name)                            \
	struct name final                                           \
	{                                                           \
		template <class T, nd_enable_if((                   \
			std::is_same<                               \
				decltype(symbol std::declval<T>()), \
				decltype(symbol std::declval<T>())  \
			>::value                                    \
		))>                                                 \
		CC_ALWAYS_INLINE constexpr                          \
		auto operator()(const T& u)                         \
		const noexcept { return symbol u; }                 \
	};

nd_define_unary_op(+, unary_plus)
nd_define_unary_op(-, unary_minus)
nd_define_unary_op(~, bit_not)
nd_define_unary_op(!, logical_not)

#define nd_define_binary_op(symbol, name)                                             \
	struct name final                                                             \
	{                                                                             \
		template <class T, class U, nd_enable_if((                            \
			std::is_same<                                                 \
				decltype(std::declval<T>() symbol std::declval<U>()), \
				decltype(std::declval<T>() symbol std::declval<U>())  \
			>::value                                                      \
		))>                                                                   \
		CC_ALWAYS_INLINE constexpr                                            \
		auto operator()(const T& u, const U& v)                               \
		const noexcept { return u symbol v; }                                 \
	};

// Arithmetic operations.
nd_define_binary_op(+, plus)
nd_define_binary_op(-, minus)
nd_define_binary_op(*, multiplies)
nd_define_binary_op(/, divides)
nd_define_binary_op(%, modulus)

// Relational operations.
nd_define_binary_op(==, equal_to)
nd_define_binary_op(!=, not_equal_to)
nd_define_binary_op(>, greater)
nd_define_binary_op(<, less)
nd_define_binary_op(>=, greater_equal)
nd_define_binary_op(<=, less_equal)

// Logical operations.
nd_define_binary_op(&&, logical_and)
nd_define_binary_op(||, logical_or)

// Bitwise operations.
nd_define_binary_op(&, bit_and)
nd_define_binary_op(|, bit_or)
nd_define_binary_op(^, bit_xor)
nd_define_binary_op(<<, left_shift)
nd_define_binary_op(>>, right_shift)

#undef nd_define_binary_op

}}

#endif
