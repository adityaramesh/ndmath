/*
** File Name: expression_forward.hpp
** Author:    Aditya Ramesh
** Date:      01/08/2015
** Contact:   _@adityaramesh.com
*/

#ifndef ZC9BB3DD3_7099_474A_B255_9C79D1009DE7
#define ZC9BB3DD3_7099_474A_B255_9C79D1009DE7

#include <cstddef>
#include <ccbase/platform.hpp>
#include <ndmath/operation.hpp>

namespace nd {

/*
** subindex
*/

template <size_t A, size_t B, class Index>
class subindex;

template <size_t A, size_t B, class Index>
class const_subindex;

template <size_t A, size_t B, class Index>
class constexpr_subindex;

template <
	size_t Dims, bool IsConstexpr,
	class Value, class ConstValue,
	class Derived
>
class index_base;

template <
	size_t A,
	size_t B,
	size_t Dims,
	class Value,
	class ConstValue,
	class Derived
>
CC_ALWAYS_INLINE CC_CONST constexpr
auto make_subindex(index_base<Dims, false, Value, ConstValue, Derived>& i)
noexcept
{
	using index_type = index_base<Dims, false, Value, ConstValue, Derived>;
	return subindex<A, B, index_type>{i};
}

template <
	size_t A,
	size_t B,
	size_t Dims,
	class Value,
	class ConstValue,
	class Derived
>
CC_ALWAYS_INLINE CC_CONST constexpr
auto make_subindex(const index_base<Dims, false, Value, ConstValue, Derived>& i)
noexcept
{
	using index_type = index_base<Dims, false, Value, ConstValue, Derived>;
	return const_subindex<A, B, index_type>{i};
}

template <
	size_t A,
	size_t B,
	size_t Dims,
	class Value,
	class ConstValue,
	class Derived
>
CC_ALWAYS_INLINE CC_CONST constexpr
auto make_subindex(const index_base<Dims, true, Value, ConstValue, Derived>& i)
noexcept
{
	using index_type = index_base<Dims, true, Value, ConstValue, Derived>;
	return constexpr_subindex<A, B, index_type>{i};
}

/*
** composite_index
*/

template <class Index1, class Index2>
class composite_index;

template <class Index1, class Index2>
class const_composite_index;

template <class Index1, class Index2>
class constexpr_composite_index;

template <
	size_t Dims1,
	size_t Dims2,
	bool IsConstexpr1,
	bool IsConstexpr2,
	class Value,
	class ConstValue1,
	class ConstValue2,
	class Derived1,
	class Derived2
>
CC_ALWAYS_INLINE CC_CONST constexpr
auto operator,(
	index_base<Dims1, IsConstexpr1, Value, ConstValue1, Derived1>& lhs,
	index_base<Dims2, IsConstexpr2, Value, ConstValue2, Derived2>& rhs
) noexcept
{
	return composite_index<
		index_base<Dims1, IsConstexpr1, Value, ConstValue1, Derived1>,
		index_base<Dims2, IsConstexpr2, Value, ConstValue2, Derived2>
	>{lhs, rhs};
}

template <
	size_t Dims1,
	size_t Dims2,
	bool IsConstexpr1,
	bool IsConstexpr2,
	class Value1,
	class Value2,
	class ConstValue1,
	class ConstValue2,
	class Derived1,
	class Derived2
>
CC_ALWAYS_INLINE CC_CONST constexpr
auto operator,(
	const index_base<Dims1, IsConstexpr1, Value1, ConstValue1, Derived1>& lhs,
	const index_base<Dims2, IsConstexpr2, Value2, ConstValue2, Derived2>& rhs
) noexcept
{
	return const_composite_index<
		index_base<Dims1, IsConstexpr1, Value1, ConstValue1, Derived1>,
		index_base<Dims2, IsConstexpr2, Value2, ConstValue2, Derived2>
	>{lhs, rhs};
}

template <
	size_t Dims1,
	size_t Dims2,
	class Value1,
	class Value2,
	class ConstValue1,
	class ConstValue2,
	class Derived1,
	class Derived2
>
CC_ALWAYS_INLINE CC_CONST constexpr
auto operator,(
	const index_base<Dims1, true, Value1, ConstValue1, Derived1>& lhs,
	const index_base<Dims2, true, Value2, ConstValue2, Derived2>& rhs
) noexcept
{
	return constexpr_composite_index<
		index_base<Dims1, true, Value1, ConstValue1, Derived1>,
		index_base<Dims2, true, Value2, ConstValue2, Derived2>
	>{lhs, rhs};
}

/*
** arithmetic expressions
*/

template <class Op, class Index1, class Index2>
class binary_index_expr;

#define nd_define_arithmetic_op(symbol, name)                                            \
	template <                                                                       \
		size_t Dims,                                                             \
		bool IsConstexpr1,                                                       \
		bool IsConstexpr2,                                                       \
		class Value1,                                                            \
		class Value2,                                                            \
		class ConstValue1,                                                       \
		class ConstValue2,                                                       \
		class Derived1,                                                          \
		class Derived2                                                           \
	>                                                                                \
	CC_ALWAYS_INLINE CC_CONST constexpr                                              \
	auto operator symbol (                                                           \
		const index_base<Dims, IsConstexpr1, Value1, ConstValue1, Derived1>& i1, \
		const index_base<Dims, IsConstexpr2, Value2, ConstValue2, Derived2>& i2  \
	) noexcept                                                                       \
	{                                                                                \
		return binary_index_expr<                                                \
			name,                                                            \
			index_base<Dims, IsConstexpr1, Value1, ConstValue1, Derived1>,   \
			index_base<Dims, IsConstexpr2, Value2, ConstValue2, Derived2>    \
		>{i1, i2};                                                               \
	}                                                                                \

nd_define_arithmetic_op(+, plus)
nd_define_arithmetic_op(-, minus)
nd_define_arithmetic_op(*, times)
nd_define_arithmetic_op(/, divide)
nd_define_arithmetic_op(%, mod)

#undef nd_define_arithmetic_op

}

#endif
