/*
** File Name: index.hpp
** Author:    Aditya Ramesh
** Date:      01/08/2015
** Contact:   _@adityaramesh.com
*/

#ifndef ZC3DF0134_42DB_48EA_B8DC_8B2AFB58D2CD
#define ZC3DF0134_42DB_48EA_B8DC_8B2AFB58D2CD

#include <array>
#include <ccbase/mpl.hpp>
#include <ndmath/index/index_wrapper.hpp>

namespace nd {
namespace detail {

template <class Integer, class Seq>
struct array_init_helper;

template <class Integer, size_t... Ts>
struct array_init_helper<Integer, std::index_sequence<Ts...>> final
{
	template <class Index>
	CC_ALWAYS_INLINE CC_CONST
	static constexpr auto
	apply(const index_wrapper<Index> w) noexcept
	{ return std::array<Integer, sizeof...(Ts)>{{w(Ts)...}}; }
};

}

template <class Integer, size_t Dims>
class index final
{
public:
	using result = Integer&;
	using const_result = Integer;
	static constexpr auto dims = Dims;
private:
	using seq = std::make_index_sequence<Dims>;
	using helper = detail::array_init_helper<Integer, seq>;
	using index_list = std::initializer_list<Integer>;

	std::array<Integer, Dims> m_indices;
public:
	template <Integer... Indices>
	CC_ALWAYS_INLINE constexpr
	explicit index(std::integer_sequence<Integer, Indices...>)
	noexcept : m_indices{{Indices...}} {}

	CC_ALWAYS_INLINE
	explicit index(const index_list indices)
	noexcept { boost::copy(indices, m_indices.begin()); }

	template <class Index>
	CC_ALWAYS_INLINE constexpr
	explicit index(const index_wrapper<Index> w)
	noexcept : m_indices(helper::apply(w)) {}

	template <class Integer_>
	CC_ALWAYS_INLINE
	result operator()(const Integer_ n) noexcept
	{ return m_indices[n]; }

	template <class Integer_>
	CC_ALWAYS_INLINE constexpr
	const_result operator()(const Integer_ n) const noexcept
	{ return m_indices[n]; }
};

template <class Integer = uint_fast32_t, class... Ts>
CC_ALWAYS_INLINE
auto make_index(const Ts... ts) noexcept
{
	using index_type = index<Integer, sizeof...(Ts)>;
	using index_list = std::initializer_list<Integer>;
	using wrapper = index_wrapper<index_type>;
	return wrapper{index_list{((Integer)ts)...}};
}

template <class Integer, Integer... Ts>
static constexpr auto basic_cindex =
index_wrapper<index<Integer, sizeof...(Ts)>>
{std::integer_sequence<Integer, Ts...>{}};

template <uint_fast32_t... Ts>
static constexpr auto cindex =
basic_cindex<uint_fast32_t, Ts...>;

template <class Integer, size_t Length, size_t Value>
static constexpr auto basic_cindex_n =
index_wrapper<index<Integer, Length>>
{cc::value_seq<cc::cseq<Length, cc::c<Integer, Value>>>{}};

template <size_t Length, uint_fast32_t Value>
static constexpr auto cindex_n =
basic_cindex_n<uint_fast32_t, Length, Value>;

}

#endif
