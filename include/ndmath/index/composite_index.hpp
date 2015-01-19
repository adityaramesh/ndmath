/*
** File Name: composite_index.hpp
** Author:    Aditya Ramesh
** Date:      01/09/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z8F87A6F3_74EA_4A09_A3CD_6DCFFB754A07
#define Z8F87A6F3_74EA_4A09_A3CD_6DCFFB754A07

#include <ndmath/index/index_wrapper.hpp>

namespace nd {

template <class Index1, class Index2>
class composite_index final
{
	using i1 = std::remove_const_t<Index1>;
	using i2 = std::remove_const_t<Index2>;

	/*
	** If either one of the indices is non-const, then this view should be
	** non-const.
	*/
	static constexpr auto is_const =
	std::is_const<Index1>::value ||
	std::is_const<Index2>::value;

	using r1 = std::conditional_t<
		is_const,
		typename i1::const_result,
		typename i1::result
	>;
	using r2 = std::conditional_t<
		is_const,
		typename i2::const_result,
		typename i2::result
	>;

	static constexpr auto dims1 = Index1::dims();
	static constexpr auto dims2 = Index2::dims();
public:
	static constexpr auto allows_static_access =
	i1::allows_static_access && i2::allows_static_access;

	static constexpr auto dims = dims1 + dims2;
	using const_result = std::common_type_t<r1, r2>;

	/*
	** The only case in which this view allows modifiations is when both
	** indices return lvalue references to the same integral type.
	** Otherwise, we make this view non-modifiable, and alias `result` to
	** `const_result`.
	*/
	using result = std::conditional_t<
		!is_const && std::is_same<r1, r2>::value,
		r1, const_result
	>;
private:
	using index1 = std::conditional_t<
		is_const, const i1, i1
	>;
	using index2 = std::conditional_t<
		is_const, const i2, i2
	>;

	index1& m_i1;
	index2& m_i2;
public:
	CC_ALWAYS_INLINE constexpr
	explicit composite_index(index1& i1, index2& i2)
	noexcept : m_i1{i1}, m_i2{i2} {}

	template <class Integer, nd_enable_if(allows_static_access)>
	CC_ALWAYS_INLINE CC_CONST constexpr
	static const_result at(const Integer n) noexcept
	{ return n < dims1 ? i1::at(n) : i2::at(n - dims1); }

	template <class Integer, nd_enable_if(!allows_static_access && !is_const)>
	CC_ALWAYS_INLINE
	result at(const Integer n) noexcept
	{ return n < dims1 ? m_i1(n) : m_i2(n - dims1); }

	template <class Integer, nd_enable_if(!allows_static_access)>
	CC_ALWAYS_INLINE constexpr
	const_result at(const Integer n) const noexcept
	{ return n < dims1 ? m_i1(n) : m_i2(n - dims1); }
};

template <class Index1, class Index2>
CC_ALWAYS_INLINE constexpr
auto operator,(index_wrapper<Index1>& i1, index_wrapper<Index2>& i2)
noexcept
{
	using w1 = index_wrapper<Index1>;
	using w2 = index_wrapper<Index2>;
	using index_type = composite_index<w1, w2>;
	using w3 = index_wrapper<index_type>;
	return w3{i1, i2};
}

template <class Index1, class Index2>
CC_ALWAYS_INLINE constexpr
auto operator,(const index_wrapper<Index1>& i1, const index_wrapper<Index2>& i2)
noexcept
{
	using w1 = const index_wrapper<Index1>;
	using w2 = const index_wrapper<Index2>;
	using index_type = composite_index<w1, w2>;
	using w3 = index_wrapper<index_type>;
	return w3{i1, i2};
}

}

#endif
