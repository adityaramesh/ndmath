/*
** File Name: index_expr.hpp
** Author:    Aditya Ramesh
** Date:      01/09/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z50063F86_53D4_490D_9E66_9A944BF570EE
#define Z50063F86_53D4_490D_9E66_9A944BF570EE

namespace nd {

template <class Index1, class Index2, class Func>
class index_expr final
{
public:
	static constexpr auto dims = Index1::dims();
private:
	Index1 m_i1{};
	Index2 m_i2{};
public:
	CC_ALWAYS_INLINE constexpr
	explicit index_expr() noexcept {}

	CC_ALWAYS_INLINE constexpr
	explicit index_expr(const Index1& i1, const Index2& i2)
	noexcept : m_i1{i1}, m_i2{i2} {}

	template <unsigned N>
	CC_ALWAYS_INLINE constexpr
	auto get() const noexcept
	{ return Func{}(m_i1.at_c(sc_coord<N>), m_i2.at_c(sc_coord<N>)); }
};

#define nd_define_arithmetic_op(symbol, name)        \
	                                             \
template <class Index1, class Index2>                \
CC_ALWAYS_INLINE constexpr                           \
auto operator symbol (                               \
	const index_wrapper<Index1>& i1,             \
	const index_wrapper<Index2>& i2              \
) noexcept                                           \
{                                                    \
	using w1 = index_wrapper<Index1>;            \
	using w2 = index_wrapper<Index2>;            \
	static_assert(                               \
		w1::dims() == w2::dims(),            \
		"Indices have unequal dimensions."   \
	);                                           \
	                                             \
	using index_type = index_expr<w1, w2, name>; \
	using w3 = index_wrapper<index_type>;        \
	return w3{in_place, i1, i2};                 \
}

nd_define_arithmetic_op(+, detail::plus)
nd_define_arithmetic_op(-, detail::minus)
nd_define_arithmetic_op(*, detail::multiplies)
nd_define_arithmetic_op(/, detail::divides)
nd_define_arithmetic_op(%, detail::modulus)

#undef nd_define_arithmetic_op

}

#endif
