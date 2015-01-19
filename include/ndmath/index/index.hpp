/*
** File Name: index.hpp
** Author:    Aditya Ramesh
** Date:      01/08/2015
** Contact:   _@adityaramesh.com
*/

#ifndef ZC3DF0134_42DB_48EA_B8DC_8B2AFB58D2CD
#define ZC3DF0134_42DB_48EA_B8DC_8B2AFB58D2CD

#include <array>

namespace nd {
namespace detail {

template <class Seq>
struct array_from_seq;

template <class Integer, Integer... Ts>
struct array_from_seq<std::integer_sequence<Integer, Ts...>> final
{
	CC_ALWAYS_INLINE constexpr
	static auto make() noexcept
	{ return std::array<Integer, sizeof...(Ts)>{{Ts...}}; }

	template <class Index>
	CC_ALWAYS_INLINE constexpr
	static auto make_from_index(const index_wrapper<Index> w) noexcept
	{ return std::array<Integer, sizeof...(Ts)>{{w(Ts)...}}; }
};

}

template <class Seq>
class const_index final
{
private:
	using integer = typename Seq::value_type;
public:
	using result = integer;
	using const_result = integer;
	static constexpr auto dims = Seq::size();
	static constexpr auto allows_static_access = true;
private:
	using helper = detail::array_from_seq<Seq>;

	/*
	** We use `Seq::size()` instead of `dims` and avoid using `auto` so that
	** providing the out-of-class definition becomes easier. Failing to
	** provide an out-of-class declaration can result in linker errors.
	*/
	static constexpr std::array<integer, Seq::size()>
	m_indices = helper::make();
public:
	CC_ALWAYS_INLINE CC_CONST constexpr
	explicit const_index() noexcept {}

	template <class Integer>
	CC_ALWAYS_INLINE constexpr
	static const_result at(const Integer n) noexcept
	{ return m_indices[n]; }
};

template <class Seq>
constexpr std::array<typename Seq::value_type, Seq::size()>
const_index<Seq>::m_indices;

template <class Integer, size_t Dims>
class index_t final
{
public:
	using result = Integer&;
	using const_result = Integer;
	static constexpr auto dims = Dims;
	static constexpr auto allows_static_access = false;
private:
	using seq = std::make_integer_sequence<Integer, Dims>;
	using helper = detail::array_from_seq<seq>;
	using index_list = std::initializer_list<Integer>;

	std::array<Integer, Dims> m_indices;
public:
	template <Integer... Indices>
	CC_ALWAYS_INLINE constexpr
	explicit index_t(std::integer_sequence<Integer, Indices...>)
	noexcept : m_indices{{Indices...}} {}

	template <class Index>
	CC_ALWAYS_INLINE constexpr
	explicit index_t(const index_wrapper<Index> w)
	noexcept : m_indices(helper::make_from_index(w)) {}

	CC_ALWAYS_INLINE
	explicit index_t(const index_list indices)
	noexcept { boost::copy(indices, m_indices.begin()); }

	template <class Integer_>
	CC_ALWAYS_INLINE
	result at(const Integer_ n) noexcept
	{ return m_indices[n]; }

	template <class Integer_>
	CC_ALWAYS_INLINE constexpr
	const_result at(const Integer_ n) const noexcept
	{ return m_indices[n]; }
};

/*
** Utilities for creating `const_index` objects.
*/

template <class Integer, Integer... Ts>
static constexpr auto basic_cindex =
index_wrapper<const_index<std::integer_sequence<Integer, Ts...>>>{};

template <uint_fast32_t... Ts>
static constexpr auto cindex =
basic_cindex<uint_fast32_t, Ts...>;

template <class Integer, size_t Length, size_t Value>
static constexpr auto basic_cindex_cn =
index_wrapper<const_index<
	mpl::to_values<mpl::repeat_nc<
		Length, std::integral_constant<Integer, Value>
	>>
>>{};

template <size_t Length, size_t Value>
static constexpr auto cindex_cn =
basic_cindex_cn<uint_fast32_t, Length, Value>;

/*
** Utilities for creating `index` objects.
*/

template <class Integer = uint_fast32_t, class... Ts>
CC_ALWAYS_INLINE
auto make_index(const Ts... ts) noexcept
{
	using index_type = index_t<Integer, sizeof...(Ts)>;
	using index_list = std::initializer_list<Integer>;
	using wrapper = index_wrapper<index_type>;
	return wrapper{index_list{((Integer)ts)...}};
}

template <class Integer, Integer... Ts>
static constexpr auto basic_index =
index_wrapper<index_t<Integer, sizeof...(Ts)>>
{std::integer_sequence<Integer, Ts...>{}};

template <uint_fast32_t... Ts>
static constexpr auto index =
basic_index<uint_fast32_t, Ts...>;

template <class Integer, size_t Length, size_t Value>
static constexpr auto basic_index_cn =
index_wrapper<index_t<Integer, Length>>
{mpl::to_values<mpl::repeat_nc<Length, std::integral_constant<Integer, Value>>>{}};

template <size_t Length, uint_fast32_t Value>
static constexpr auto index_cn =
basic_index_cn<uint_fast32_t, Length, Value>;

}

#endif
