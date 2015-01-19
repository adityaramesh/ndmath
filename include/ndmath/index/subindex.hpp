/*
** File Name: subindex.hpp
** Author:    Aditya Ramesh
** Date:      01/09/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z31A9D82F_DBFB_4B6A_BD89_544A77E8D805
#define Z31A9D82F_DBFB_4B6A_BD89_544A77E8D805

namespace nd {

template <size_t A, size_t B, class Index>
class subindex final
{
	using index_type = std::remove_const_t<Index>;
	static constexpr auto is_const = std::is_const<Index>::value;
public:
	using result = std::conditional_t<
		is_const,
		typename index_type::const_result,
		typename index_type::result
	>;
	using const_result = typename index_type::const_result;

	static constexpr auto allows_static_access =
	Index::allows_static_access;

	static constexpr auto dims = B - A + 1;
private:
	Index& m_index;
public:
	CC_ALWAYS_INLINE constexpr
	explicit subindex(Index& index)
	noexcept : m_index{index} {}

	template <class Integer, nd_enable_if(allows_static_access)>
	CC_ALWAYS_INLINE CC_CONST constexpr
	static const_result at(const Integer n) noexcept
	{ return index_type::at(n + A); }

	/*
	** We should only enable this function if `Index` is not `const`;
	** otherwise, we run into the following conflict. If `Index` is `const`,
	** then this function should be marked `constexpr`. But this function
	** cannot be marked `constexpr` if `Index` is not `const`, since
	** `m_index` may return a non-const reference. So we choose not to mark
	** the function `constexpr`, and disable it in the case that `Index` is
	** not `const`.
	*/
	template <class Integer, nd_enable_if(!allows_static_access && !is_const)>
	CC_ALWAYS_INLINE
	result at(const Integer n) noexcept
	{ return m_index(n + A); }

	template <class Integer, nd_enable_if(!allows_static_access)>
	CC_ALWAYS_INLINE constexpr
	const_result at(const Integer n) const noexcept
	{ return m_index(n + A); }
};

template <size_t A, size_t B, class Index>
CC_ALWAYS_INLINE constexpr
auto make_subindex(index_wrapper<Index>& w) noexcept
{
	using w1 = index_wrapper<Index>;
	using index_type = subindex<A, B, w1>;
	using w2 = index_wrapper<index_type>;
	return w2{w};
}

template <size_t A, size_t B, class Index>
CC_ALWAYS_INLINE constexpr
auto make_const_subindex(const index_wrapper<Index>& w) noexcept
{
	using w1 = const index_wrapper<Index>;
	using index_type = subindex<A, B, w1>;
	using w2 = index_wrapper<index_type>;
	return w2{w};
}

}

#endif
