/*
** File Name: index_expr.hpp
** Author:    Aditya Ramesh
** Date:      01/09/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z50063F86_53D4_490D_9E66_9A944BF570EE
#define Z50063F86_53D4_490D_9E66_9A944BF570EE

#include <ndmath/arithmetic_functions.hpp>
#include <ndmath/index/index_wrapper.hpp>

namespace nd {

template <class Op, class Index1, class Index2>
class index_expr final
{
public:
	using result = std::common_type_t<
		typename Index1::result,
		typename Index2::result
	>;
	using const_result = result;

	static constexpr auto dims = Index1::dims();
private:
	const Index1& m_i1;
	const Index2& m_i2;
public:
	CC_ALWAYS_INLINE constexpr
	explicit index_expr(const Index1& i1, const Index2& i2)
	noexcept : m_i1{i1}, m_i2{i2} {}

	template <class Integer>
	CC_ALWAYS_INLINE constexpr
	result operator()(const Integer n) noexcept
	{ return Op::apply(m_i1(n), m_i2(n)); }

	template <class Integer>
	CC_ALWAYS_INLINE constexpr
	const_result operator()(const Integer n) const noexcept
	{ return Op::apply(m_i1(n), m_i2(n)); }
};

#define nd_define_arithmetic_op(symbol, name)         \
	                                              \
template <class Index1, class Index2>                 \
CC_ALWAYS_INLINE constexpr                            \
auto operator symbol (                                \
	const index_wrapper<Index1>& i1,              \
	const index_wrapper<Index2>& i2               \
) noexcept                                            \
{                                                     \
	using w1 = index_wrapper<Index1>;             \
	using w2 = index_wrapper<Index2>;             \
	static_assert(                                \
		w1::dims() == w2::dims(),             \
		"Indices have unequal dimensions."    \
	);                                            \
	                                              \
	using index_type = index_expr<name, w1, w2>;  \
	using w3 = index_wrapper<index_type>;         \
	return w3{i1, i2};                            \
}

nd_define_arithmetic_op(+, plus)
nd_define_arithmetic_op(-, minus)
nd_define_arithmetic_op(*, times)
nd_define_arithmetic_op(/, divide)
nd_define_arithmetic_op(%, mod)

#undef nd_define_arithmetic_op

}

#endif
