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
	static constexpr auto dims = unsigned(sizeof...(Args));
private:
	std::tuple<Args...> m_args{};
public:
	CC_ALWAYS_INLINE constexpr
	explicit index_t() noexcept {}

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
	const auto& get() const noexcept
	{ return std::get<N>(m_args); }
};

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
	static auto make_index() noexcept
	{ return index_type{in_place, basic_sc_coord<Integer, Ts>...}; }
};

}

/*
** Utilities for creating static constexpr indices.
*/

template <class Integer, Integer... Ts>
static constexpr auto basic_sc_index =
detail::seq_to_index<std::integer_sequence<Integer, Ts...>>::make_index();

template <unsigned... Ts>
static constexpr auto sc_index =
basic_sc_index<unsigned, Ts...>;

template <class Integer, size_t Length, size_t Value>
static constexpr auto basic_sc_index_n =
detail::seq_to_index<
	mpl::to_values<mpl::repeat_nc<
		Length, std::integral_constant<Integer, Value>
	>>
>::make_index();

template <size_t Length, size_t Value>
static constexpr auto sc_index_n =
basic_sc_index_n<unsigned, Length, Value>;

/*
** Utilities for creating non-static indices.
*/

namespace detail {

template <class Integer>
struct make_index_helper
{
	template <class T, nd_enable_if(std::is_integral<T>::value)>
	CC_ALWAYS_INLINE constexpr
	static auto make_coord(const T& t) noexcept
	{ return nd::make_coord((Integer)t); }

	template <class T, nd_enable_if(std::is_integral<T>::value)>
	CC_ALWAYS_INLINE constexpr
	static auto make_const_coord(const T& t) noexcept
	{ return nd::make_c_coord((Integer)t); }

	template <class Coord>
	CC_ALWAYS_INLINE constexpr
	static auto make_coord(const coord_wrapper<Coord>& c) noexcept
	{ return eval(c); }

	template <class Coord>
	CC_ALWAYS_INLINE constexpr
	static auto make_const_coord(const coord_wrapper<Coord>& c) noexcept
	{ return eval(c); }
};

}

template <class Integer = unsigned, class... Ts>
CC_ALWAYS_INLINE constexpr
auto index(const Ts... ts) noexcept
{
	using helper     = detail::make_index_helper<Integer>;
	using index_type = index_t<decltype(helper::make_coord(ts))...>;
	using wrapper    = index_wrapper<index_type>;
	return wrapper{in_place, helper::make_coord(ts)...};
}

template <class Integer = unsigned, class... Ts>
CC_ALWAYS_INLINE constexpr
auto c_index(const Ts... ts) noexcept
{
	using helper     = detail::make_index_helper<Integer>;
	using index_type = index_t<decltype(helper::make_const_coord(ts))...>;
	using wrapper    = index_wrapper<index_type>;
	return wrapper{in_place, helper::make_const_coord(ts)...};
}

namespace detail {

template <size_t Cur, size_t Last>
struct seq_expand_helper
{
	using next = seq_expand_helper<Cur + 1, Last>;

	template <class T, class... Ts>
	CC_ALWAYS_INLINE constexpr
	static auto make_index(const T t, const Ts... ts) noexcept
	{ return next::make_index(t, ts..., t); }

	template <class T, class... Ts>
	CC_ALWAYS_INLINE constexpr
	static auto make_const_index(const T t, const Ts... ts) noexcept
	{ return next::make_const_index(t, ts..., t); }
};

template <size_t Last>
struct seq_expand_helper<Last, Last>
{
	template <class... Ts>
	CC_ALWAYS_INLINE constexpr
	static auto make_index(const Ts... ts) noexcept
	{ return index(ts...); }

	template <class... Ts>
	CC_ALWAYS_INLINE constexpr
	static auto make_const_index(const Ts... ts) noexcept
	{ return c_index(ts...); }
};

}

template <size_t N, class Integer = unsigned, class T>
CC_ALWAYS_INLINE constexpr
auto index_n(const T v) noexcept
{
	using helper = detail::seq_expand_helper<0, N - 1>;
	return helper::make_index((Integer)v);
}

template <size_t N, class Integer = unsigned, class T>
CC_ALWAYS_INLINE constexpr
auto c_index_n(const T v) noexcept
{
	using helper = detail::seq_expand_helper<0, N - 1>;
	return helper::make_const_index((Integer)v);
}

/*
** Utilities for turning an index expression into a concrete index type.
*/

namespace detail {

template <class Seq>
struct index_eval_helper;

template <size_t... Ts>
struct index_eval_helper<std::index_sequence<Ts...>>
{
	template <class Index>
	CC_ALWAYS_INLINE constexpr
	static auto apply(const Index& i) noexcept
	{
		using tokens::c;
		return index(i.at_c(c<Ts>)...);
	}
};

}

template <class Index>
CC_ALWAYS_INLINE constexpr
auto eval(const index_wrapper<Index>& i) noexcept
{
	using wrapper = index_wrapper<Index>;
	using seq     = std::make_index_sequence<wrapper::dims()>;
	using helper  = detail::index_eval_helper<seq>;
	return helper::apply(i);
}

}

#endif
