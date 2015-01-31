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
namespace detail {

/*
** Note: Using plain auto return types for the `get` member function will cause
** the reference to be stripped from the inferred return type. To prevent this
** from happening, we need to use a traits struct.
*/

template <uint_fast32_t N, uint_fast32_t Dims1, class I1, class I2, bool First>
struct composite_index_traits;

template <uint_fast32_t N, uint_fast32_t Dims1, class I1, class I2>
struct composite_index_traits<N, Dims1, I1, I2, true>
{ using type = decltype(std::declval<I1>().at(nd::tokens::c<N>)); };

template <uint_fast32_t N, uint_fast32_t Dims1, class I1, class I2>
struct composite_index_traits<N, Dims1, I1, I2, false>
{ using type = decltype(std::declval<I2>().at(nd::tokens::c<N - Dims1>)); };

template <uint_fast32_t N, uint_fast32_t Dims1, bool First>
struct composite_index_helper;

template <uint_fast32_t N, uint_fast32_t Dims1>
struct composite_index_helper<N, Dims1, true>
{
	template <class I1, class I2>
	CC_ALWAYS_INLINE 
	static auto get(I1& i1, I2) noexcept ->
	decltype(std::declval<I1>().at(nd::tokens::c<N>))
	{
		using tokens::c;
		return i1(c<N>);
	}

	template <class I1, class I2>
	CC_ALWAYS_INLINE constexpr
	static auto get_const(I1& i1, I2) noexcept ->
	decltype(std::declval<I1>().at(nd::tokens::c<N>))
	{
		using tokens::c;
		return i1(c<N>);
	}
};

template <uint_fast32_t N, uint_fast32_t Dims1>
struct composite_index_helper<N, Dims1, false>
{
	template <class I1, class I2>
	CC_ALWAYS_INLINE 
	static auto get(I1, I2& i2) noexcept ->
	decltype(std::declval<I2>().at(nd::tokens::c<N - Dims1>))
	{
		using tokens::c;
		return i2(c<N - Dims1>);
	}

	template <class I1, class I2>
	CC_ALWAYS_INLINE constexpr
	static auto get_const(I1, I2& i2) noexcept ->
	decltype(std::declval<I2>().at(nd::tokens::c<N - Dims1>))
	{
		using tokens::c;
		return i2(c<N - Dims1>);
	}
};

}

template <class Index1, class Index2>
class composite_index final
{
	using i1 = std::decay_t<Index1>;
	using i2 = std::decay_t<Index2>;
	static constexpr auto dims1 = i1::dims().value();
	static constexpr auto dims2 = i2::dims().value();
public:
	static constexpr auto dims = dims1 + dims2;
private:
	Index1& m_i1;
	Index2& m_i2;
public:
	CC_ALWAYS_INLINE constexpr
	explicit composite_index(Index1& i1, Index2& i2)
	noexcept : m_i1{i1}, m_i2{i2} {}

	template <uint_fast32_t N>
	CC_ALWAYS_INLINE
	auto get() noexcept ->
	typename detail::composite_index_traits<
		N, dims1, Index1, Index2, N < dims1
	>::type
	{
		using helper = detail::composite_index_helper<N, dims1, N < dims1>;
		return helper::get(m_i1, m_i2);
	}

	template <uint_fast32_t N>
	CC_ALWAYS_INLINE constexpr
	auto get() const noexcept ->
	typename detail::composite_index_traits<
		N, dims1, Index1, Index2, N < dims1
	>::type
	{
		using helper = detail::composite_index_helper<N, dims1, N < dims1>;
		return helper::get_const(m_i1, m_i2);
	}
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
	return w3{in_place, i1, i2};
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
	return w3{in_place, i1, i2};
}

}

#endif
