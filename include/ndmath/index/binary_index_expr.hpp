/*
** File Name: binary_index_expr.hpp
** Author:    Aditya Ramesh
** Date:      01/09/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z50063F86_53D4_490D_9E66_9A944BF570EE
#define Z50063F86_53D4_490D_9E66_9A944BF570EE

#include <ndmath/index/index_base.hpp>

namespace nd {

template <class Op, class Index1, class Index2>
class binary_index_expr final :
public index_base<
	Index1::dims(),
	Index1::is_constexpr && Index2::is_constexpr,
	std::common_type_t<
		typename Index1::index_type,
		typename Index2::index_type
	>,
	std::common_type_t<
		typename Index1::index_type,
		typename Index2::index_type
	>,
	binary_index_expr<Op, Index1, Index2>
>
{
	static constexpr auto is_constexpr =
	Index1::is_constexpr && Index2::is_constexpr;

	using t1     = typename Index1::index_type;
	using t2     = typename Index2::index_type;
	using result = std::common_type_t<t1, t2>;
	using self   = binary_index_expr<Op, Index1, Index2>;
	using base   = index_base<Index1::dims(), is_constexpr, result, result, self>;

	const Index1& m_i1;
	const Index2& m_i2;
public:
	using base::operator();

	CC_ALWAYS_INLINE constexpr
	explicit binary_index_expr(const Index1& i1, const Index2& i2)
	noexcept : m_i1{i1}, m_i2{i2} {}

	template <class T>
	CC_ALWAYS_INLINE result
	operator()(const T& n) noexcept
	{ return Op::apply(m_i1(n), m_i2(n)); }

	template <class T>
	CC_ALWAYS_INLINE result
	operator()(const T& n) const noexcept
	{ return Op::apply(m_i1(n), m_i2(n)); }
};

/*
** Note: we need to use a specialization instead of using enable_if because
** `operator()` fails to compile for some reason involving the dummy template
** parameter that I don't want to research.
*/
template <
	class Op,
	size_t Dims,
	class Value1,
	class Value2,
	class ConstValue1,
	class ConstValue2,
	class Derived1,
	class Derived2
>
class binary_index_expr<
	Op,
	index_base<Dims, true, Value1, ConstValue1, Derived1>,
	index_base<Dims, true, Value2, ConstValue2, Derived2>
> final : public index_base<
	Dims,
	true,
	std::common_type_t<
		typename index_base<Dims, true, Value1, ConstValue1, Derived1>::index_type,
		typename index_base<Dims, true, Value2, ConstValue2, Derived1>::index_type
	>,
	std::common_type_t<
		typename index_base<Dims, true, Value1, ConstValue1, Derived1>::index_type,
		typename index_base<Dims, true, Value2, ConstValue2, Derived1>::index_type
	>,
	binary_index_expr<
		Op,
		index_base<Dims, true, Value1, ConstValue1, Derived1>,
		index_base<Dims, true, Value2, ConstValue2, Derived2>
	>
>
{
	using index1 = index_base<Dims, true, Value1, ConstValue1, Derived1>;
	using index2 = index_base<Dims, true, Value2, ConstValue2, Derived2>;
	using t1     = typename index1::index_type;
	using t2     = typename index2::index_type;
	using result = std::common_type_t<t1, t2>;
	using self   = binary_index_expr<Op, index1, index2>;
	using base   = index_base<index1::dims(), true, result, result, self>;

	const index1& m_i1;
	const index2& m_i2;
public:
	using base::operator();

	CC_ALWAYS_INLINE constexpr
	explicit binary_index_expr(const index1& i1, const index2& i2)
	noexcept : m_i1{i1}, m_i2{i2} {}

	template <class T>
	CC_ALWAYS_INLINE CC_CONST constexpr
	result operator()(const T& n) const noexcept
	{ return Op::apply(m_i1(n), m_i2(n)); }
};

}

#endif
