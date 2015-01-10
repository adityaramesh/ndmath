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
	size_t&,
	size_t,
	composite_index<Index1, Index2
>>
{
	static constexpr auto dims1 = Index1::dims();
	static constexpr auto dims2 = Index2::dims();

	using value       = size_t&;
	using const_value = size_t;
	using self        = composite_index<Index1, Index2>;
	using base        = index_base<dims1 + dims2, false, value, const_value, self>;

	Index1& m_i1;
	Index2& m_i2;
public:
	using base::operator=;
	using base::operator();

	CC_ALWAYS_INLINE constexpr
	explicit composite_index(Index1& i1, Index2& i2)
	noexcept : m_i1{i1}, m_i2{i2} {}

	CC_ALWAYS_INLINE value
	operator()(const size_t& n) noexcept
	{ return n < dims1 ? m_i1(n) : m_i2(n - dims1); }

	CC_ALWAYS_INLINE const_value
	operator()(const size_t& n) const noexcept
	{ return n < dims1 ? m_i1(n) : m_i2(n - dims1); }
};

template <class Index1, class Index2>
class const_composite_index final :
public index_base<
	Index1::dims() + Index2::dims(),
	false,
	size_t,
	size_t,
	const_composite_index<Index1, Index2>
>
{
	static constexpr auto dims1 = Index1::dims();
	static constexpr auto dims2 = Index2::dims();

	using value = size_t;
	using self  = const_composite_index<Index1, Index2>;
	using base  = index_base<dims1 + dims2, false, value, value, self>;

	const Index1& m_i1;
	const Index2& m_i2;
public:
	using base::operator();

	CC_ALWAYS_INLINE constexpr
	explicit const_composite_index(const Index1& i1, const Index2& i2)
	noexcept : m_i1{i1}, m_i2{i2} {}

	CC_ALWAYS_INLINE value
	operator()(const size_t& n) noexcept
	{ return n < dims1 ? m_i1(n) : m_i2(n - dims1); }

	CC_ALWAYS_INLINE value
	operator()(const size_t& n) const noexcept
	{ return n < dims1 ? m_i1(n) : m_i2(n - dims1); }
};

template <class Index1, class Index2>
class constexpr_composite_index final :
public index_base<
	Index1::dims() + Index2::dims(),
	true,
	size_t,
	size_t,
	constexpr_composite_index<Index1, Index2>
>
{
	static constexpr auto dims1 = Index1::dims();
	static constexpr auto dims2 = Index2::dims();

	using value = size_t;
	using self = constexpr_composite_index<Index1, Index2>;
	using base = index_base<dims1 + dims2, true, value, value, self>;

	const Index1& m_i1;
	const Index2& m_i2;
public:
	using base::operator();

	CC_ALWAYS_INLINE constexpr
	explicit constexpr_composite_index(const Index1& i1, const Index2& i2)
	noexcept : m_i1{i1}, m_i2{i2} {}

	CC_ALWAYS_INLINE CC_CONST constexpr
	value operator()(const size_t& n) noexcept
	{ return n < dims1 ? m_i1(n) : m_i2(n - dims1); }

	CC_ALWAYS_INLINE CC_CONST constexpr
	value operator()(const size_t& n) const noexcept
	{ return n < dims1 ? m_i1(n) : m_i2(n - dims1); }
};

}

#endif
