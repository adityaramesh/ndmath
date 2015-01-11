/*
** File Name: composite_index.hpp
** Author:    Aditya Ramesh
** Date:      01/09/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z8F87A6F3_74EA_4A09_A3CD_6DCFFB754A07
#define Z8F87A6F3_74EA_4A09_A3CD_6DCFFB754A07

#include <ndmath/index/index_base.hpp>

namespace nd {

/*
** In this case, we allow both indices to be modified through the subindex view.
** So we must have `Value = size_t&` for both indices. On the other hand, it is
** possible that one index has `ConstValue = const size_t&` and the other has
** `ConstValue = size_t`. We can resolve this conflict by returning `size_t`
** from the const-overloaded functions. Thus we choose `ConstValue = size_t` for
** the subindex view itself.
*/
template <class Index1, class Index2>
class composite_index final :
public index_base<
	Index1::dims() + Index2::dims(),
	false,
	typename Index1::result,
	std::common_type_t<
		typename Index1::index_type,
		typename Index2::index_type
	>,
	composite_index<Index1, Index2
>>
{
	static constexpr auto dims1 = Index1::dims();
	static constexpr auto dims2 = Index2::dims();

	using t1 = typename Index1::index_type;
	using t2 = typename Index2::index_type;

	using result       = typename Index1::result;
	using const_result = std::common_type_t<t1, t2>;
	using self         = composite_index<Index1, Index2>;
	using base         = index_base<dims1 + dims2, false, result, const_result, self>;

	Index1& m_i1;
	Index2& m_i2;
public:
	using base::operator=;
	using base::operator();

	CC_ALWAYS_INLINE constexpr
	explicit composite_index(Index1& i1, Index2& i2)
	noexcept : m_i1{i1}, m_i2{i2} {}

	template <class T>
	CC_ALWAYS_INLINE result
	operator()(const T& n) noexcept
	{ return n < dims1 ? m_i1(n) : m_i2(n - dims1); }

	template <class T>
	CC_ALWAYS_INLINE const_result
	operator()(const T& n) const noexcept
	{ return n < dims1 ? m_i1(n) : m_i2(n - dims1); }
};

template <class Index1, class Index2>
class const_composite_index final :
public index_base<
	Index1::dims() + Index2::dims(),
	false,
	std::common_type_t<
		typename Index1::index_type,
		typename Index2::index_type
	>,
	std::common_type_t<
		typename Index1::index_type,
		typename Index2::index_type
	>,
	const_composite_index<Index1, Index2>
>
{
	static constexpr auto dims1 = Index1::dims();
	static constexpr auto dims2 = Index2::dims();

	using t1 = typename Index1::index_type;
	using t2 = typename Index2::index_type;

	using result = std::common_type_t<t1, t2>;
	using self   = const_composite_index<Index1, Index2>;
	using base   = index_base<dims1 + dims2, false, result, result, self>;

	const Index1& m_i1;
	const Index2& m_i2;
public:
	using base::operator();

	CC_ALWAYS_INLINE constexpr
	explicit const_composite_index(const Index1& i1, const Index2& i2)
	noexcept : m_i1{i1}, m_i2{i2} {}

	template <class T>
	CC_ALWAYS_INLINE result
	operator()(const T& n) noexcept
	{ return n < dims1 ? m_i1(n) : m_i2(n - dims1); }

	template <class T>
	CC_ALWAYS_INLINE result
	operator()(const T& n) const noexcept
	{ return n < dims1 ? m_i1(n) : m_i2(n - dims1); }
};

template <class Index1, class Index2>
class constexpr_composite_index final :
public index_base<
	Index1::dims() + Index2::dims(),
	true,
	std::common_type_t<
		typename Index1::index_type,
		typename Index2::index_type
	>,
	std::common_type_t<
		typename Index1::index_type,
		typename Index2::index_type
	>,
	constexpr_composite_index<Index1, Index2>
>
{
	static constexpr auto dims1 = Index1::dims();
	static constexpr auto dims2 = Index2::dims();

	using t1 = typename Index1::index_type;
	using t2 = typename Index2::index_type;

	using result = std::common_type_t<t1, t2>;
	using self   = constexpr_composite_index<Index1, Index2>;
	using base   = index_base<dims1 + dims2, true, result, result, self>;

	const Index1& m_i1;
	const Index2& m_i2;
public:
	using base::operator();

	CC_ALWAYS_INLINE constexpr
	explicit constexpr_composite_index(const Index1& i1, const Index2& i2)
	noexcept : m_i1{i1}, m_i2{i2} {}

	/*
	** Do we really need this?
	**
	** template <class T>
	** CC_ALWAYS_INLINE CC_CONST constexpr
	** value operator()(const T& n) noexcept
	** { return n < dims1 ? m_i1(n) : m_i2(n - dims1); }
	*/

	template <class T>
	CC_ALWAYS_INLINE CC_CONST constexpr
	result operator()(const T& n) const noexcept
	{ return n < dims1 ? m_i1(n) : m_i2(n - dims1); }
};

}

#endif
