/*
** File Name: index.hpp
** Author:    Aditya Ramesh
** Date:      01/08/2015
** Contact:   _@adityaramesh.com
*/

#ifndef ZC3DF0134_42DB_48EA_B8DC_8B2AFB58D2CD
#define ZC3DF0134_42DB_48EA_B8DC_8B2AFB58D2CD

namespace nd {

template <class... Args>
class index_t final
{
public:
	static constexpr auto dims = uint_fast32_t(sizeof...(Args));
private:
	std::tuple<Args...> m_args;
public:
	template <class... Args_>
	CC_ALWAYS_INLINE constexpr
	explicit index_t(Args_&&... args)
	noexcept : m_args{std::forward<Args_>(args)...} {}

	template <size_t N>
	CC_ALWAYS_INLINE
	auto& get() noexcept
	{ return std::get<N>(m_args); }

	template <size_t N>
	CC_ALWAYS_INLINE constexpr
	auto& get() const noexcept
	{ return std::get<N>(m_args); }
};

/*
** Utilities for creating non-static indices.
*/

template <class Integer = uint_fast32_t, class... Ts>
CC_ALWAYS_INLINE constexpr
auto make_index(const Ts... ts) noexcept
{
	using coord_type = coord_wrapper<coord<Integer>>;
	using index_type = index_t<std::conditional_t<
		std::is_same<decltype(ts), decltype(ts)>::value,
		coord_type, void
	>...>;
	using wrapper = index_wrapper<index_type>;
	return wrapper{in_place, make_coord(Integer(ts))...};
}

template <class Integer = uint_fast32_t, class... Ts>
CC_ALWAYS_INLINE constexpr
auto make_c_index(const Ts... ts) noexcept
{
	using coord_type = coord_wrapper<coord<const Integer>>;
	using index_type = index_t<std::conditional_t<
		std::is_same<decltype(ts), decltype(ts)>::value,
		coord_type, void
	>...>;
	using wrapper = index_wrapper<index_type>;
	return wrapper{in_place, make_c_coord(Integer(ts))...};
}

namespace detail {

template <class Seq>
struct seq_to_index;

template <class Integer, Integer... Ts>
struct seq_to_index<std::integer_sequence<Integer, Ts...>> final
{
	using index_type = index_wrapper<index_t<
		std::decay_t<decltype(basic_sc_coord<Integer, Ts>)>...
	>>;

	CC_ALWAYS_INLINE constexpr
	static auto make_sc() noexcept
	{ return index_type{in_place, basic_sc_coord<Integer, Ts>...}; }

	CC_ALWAYS_INLINE constexpr
	static auto make_c() noexcept
	{ return make_c_index(Ts...); }

	CC_ALWAYS_INLINE constexpr
	static auto make() noexcept
	{ return make_index(Ts...); }
};

}

/*
** Utilities for creating static constexpr indices.
*/

template <class Integer, Integer... Ts>
static constexpr auto basic_sc_index =
detail::seq_to_index<std::integer_sequence<Integer, Ts...>>::make_sc();

template <uint_fast32_t... Ts>
static constexpr auto sc_index =
basic_sc_index<uint_fast32_t, Ts...>;

template <class Integer, size_t Length, size_t Value>
static constexpr auto basic_sc_index_n =
detail::seq_to_index<
	mpl::to_values<mpl::repeat_nc<
		Length, std::integral_constant<Integer, Value>
	>>
>::make_sc();

template <size_t Length, size_t Value>
static constexpr auto sc_index_n =
basic_sc_index_n<uint_fast32_t, Length, Value>;

/*
** Shorthand notation for creating non-static const indices.
*/

template <class Integer, Integer... Ts>
static constexpr auto basic_index =
detail::seq_to_index<std::integer_sequence<Integer, Ts...>>::make();

template <uint_fast32_t... Ts>
static constexpr auto index =
basic_index<uint_fast32_t, Ts...>;

template <class Integer, size_t Length, size_t Value>
static constexpr auto basic_index_n =
detail::seq_to_index<mpl::to_values<
	mpl::repeat_nc<Length, std::integral_constant<Integer, Value>>
>>::make();

template <size_t Length, uint_fast32_t Value>
static constexpr auto index_n =
basic_index_n<uint_fast32_t, Length, Value>;

/*
** Shorthand notation for creating non-static non-const indices.
*/

template <class Integer, Integer... Ts>
static constexpr auto basic_c_index =
detail::seq_to_index<std::integer_sequence<Integer, Ts...>>::make_c();

template <uint_fast32_t... Ts>
static constexpr auto c_index =
basic_c_index<uint_fast32_t, Ts...>;

template <class Integer, size_t Length, size_t Value>
static constexpr auto basic_c_index_n =
detail::seq_to_index<mpl::to_values<
	mpl::repeat_nc<Length, std::integral_constant<Integer, Value>>
>>::make_c();

template <size_t Length, uint_fast32_t Value>
static constexpr auto c_index_n =
basic_c_index_n<uint_fast32_t, Length, Value>;

}

#endif
