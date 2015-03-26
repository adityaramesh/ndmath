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
	static constexpr auto is_const =
	std::is_const<Index>::value;
public:
	static constexpr auto dims = B - A + 1;
private:
	Index& m_index;
public:
	CC_ALWAYS_INLINE constexpr
	explicit subindex(Index& index)
	noexcept : m_index{index} {}

	/*
	** We should only enable this function if `Index` is not `const`;
	** otherwise, we run into the following conflict. If `Index` is `const`,
	** then this function should be marked `constexpr`. But this function
	** cannot be marked `constexpr` if `Index` is not `const`, since
	** `m_index` may return a non-const reference. So we choose not to mark
	** the function `constexpr`, and disable it in the case that `Index` is
	** not `const`.
	**/
	template <unsigned N, nd_enable_if(!is_const)>
	CC_ALWAYS_INLINE
	auto get() noexcept ->
	decltype(std::declval<Index>().at_c(tokens::c<N + A>))
	{
		using tokens::c;
		return m_index.at_c(c<N + A>);
	}

	template <unsigned N>
	CC_ALWAYS_INLINE constexpr
	auto get() const noexcept ->
	const decltype(std::declval<const Index>().at_c(tokens::c<N + A>))
	{
		using tokens::c;
		return m_index.at_c(c<N + A>);
	}
};

template <unsigned A, unsigned B, class Index>
CC_ALWAYS_INLINE constexpr
auto make_subindex(index_wrapper<Index>& w) noexcept
{
	using w1 = index_wrapper<Index>;
	using index_type = subindex<A, B, w1>;
	using w2 = index_wrapper<index_type>;
	return w2{in_place, w};
}

template <unsigned A, unsigned B, class Index>
CC_ALWAYS_INLINE constexpr
auto make_const_subindex(const index_wrapper<Index>& w) noexcept
{
	using w1 = const index_wrapper<Index>;
	using index_type = subindex<A, B, w1>;
	using w2 = index_wrapper<index_type>;
	return w2{in_place, w};
}

}

#endif
