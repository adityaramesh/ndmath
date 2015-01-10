/*
** File Name: subindex.hpp
** Author:    Aditya Ramesh
** Date:      01/09/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z31A9D82F_DBFB_4B6A_BD89_544A77E8D805
#define Z31A9D82F_DBFB_4B6A_BD89_544A77E8D805

#include <ndmath/index/index_base.hpp>

namespace nd {

template <size_t A, size_t B, class Index>
class subindex final :
public index_base<
	B - A + 1,
	false,
	typename Index::value,
	typename Index::const_value,
	subindex<A, B, Index>
>
{
	using self         = subindex<A, B, Index>;
	using result       = typename Index::value;
	using const_result = typename Index::const_value;
	using base         = index_base<B - A + 1, false, result, const_result, self>;

	Index& m_index;
public:
	using base::operator=;
	using base::operator();

	CC_ALWAYS_INLINE constexpr
	explicit subindex(Index& index)
	noexcept : m_index{index} {}

	CC_ALWAYS_INLINE result
	operator()(const size_t& n) noexcept
	{ return m_index(n + A); }

	CC_ALWAYS_INLINE const_result
	operator()(const size_t& n) const noexcept
	{ return m_index(n + A); }
};

template <size_t A, size_t B, class Index>
class const_subindex final :
public index_base<
	B - A + 1,
	false,
	typename Index::const_value,
	typename Index::const_value,
	const_subindex<A, B, Index>
>
{
	using self         = const_subindex<A, B, Index>;
	using const_result = typename Index::const_value;
	using base         = index_base<B - A + 1, false, const_result, const_result, self>;

	const Index& m_index;
public:
	using base::operator();

	CC_ALWAYS_INLINE constexpr
	explicit const_subindex(const Index& index)
	noexcept : m_index{index} {}

	CC_ALWAYS_INLINE const_result
	operator()(const size_t& n) noexcept
	{ return m_index(n + A); }

	CC_ALWAYS_INLINE const_result
	operator()(const size_t& n) const noexcept
	{ return m_index(n + A); }
};

template <size_t A, size_t B, class Index>
class constexpr_subindex final :
public index_base<
	B - A + 1,
	true,
	typename Index::const_value,
	typename Index::const_value,
	constexpr_subindex<A, B, Index>
>
{
	using self         = constexpr_subindex<A, B, Index>;
	using const_result = typename Index::const_value;
	using base         = index_base<B - A + 1, true, const_result, const_result, self>;

	const Index& m_index;
public:
	using base::operator();

	CC_ALWAYS_INLINE constexpr
	explicit constexpr_subindex(const Index& index)
	noexcept : m_index{index} {}

	/*
	CC_ALWAYS_INLINE constexpr
	auto operator()(const size_t& n)
	noexcept { return m_index(n + A); }
	*/

	CC_ALWAYS_INLINE constexpr
	const_result operator()(const size_t& n)
	const noexcept { return m_index(n + A); }
};

}

#endif
