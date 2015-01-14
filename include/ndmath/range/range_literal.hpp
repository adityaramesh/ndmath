/*
** File Name: range_literal.hpp
** Author:    Aditya Ramesh
** Date:      01/11/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z907E6A89_90C3_485B_92BB_E694162D5D39
#define Z907E6A89_90C3_485B_92BB_E694162D5D39

#include <ccbase/mpl.hpp>
#include <ndmath/range/range.hpp>

namespace nd {

/*
** Possible cases for the syntax supported:
** - "0 0 0; 50 50 50; 1 1 1_range (bases, extents, strides)
** - "0 0 0; 50 50 50"_range (bases, extents, default strides)
** - "50 50 50"_range (default bases, extents, default strides)
** - "; 50 50 50; "_range (default bases, extents, default strides)
** - "0 0 0; 50 50 50; "_range (bases, extents, default strides)
** - "; 50 50 50; 1 1 1"_range (default bases, extents, default strides)
** - "; 50 50 50;"_range (default bases, extents, default strides)
*/

namespace detail {

template <class Seq>
struct range_literal_helper;

template <uintmax_t... Ts>
struct range_literal_helper<cc::seq<
	std::integer_sequence<uintmax_t, Ts...>
>>
{
	static constexpr auto dims = sizeof...(Ts);
	static constexpr auto extents = cindex<Ts...>;
	static constexpr auto bases = cindex_n<dims, 0>;
	static constexpr auto strides = cindex_n<dims, 1>;
};

template <uintmax_t... Ts, uintmax_t... Us>
struct range_literal_helper<cc::seq<
	std::integer_sequence<uintmax_t, Ts...>,
	std::integer_sequence<uintmax_t, Us...>
>>
{
	static constexpr auto dims = sizeof...(Ts);
	static_assert(sizeof...(Us) == dims, "");

	static constexpr auto bases = cindex<Ts...>;
	static constexpr auto extents = cindex<Us...>;
	static constexpr auto strides = cindex_n<dims, 1>;
};

template <uintmax_t... Ts, uintmax_t... Us, uintmax_t... Vs>
struct range_literal_helper<cc::seq<
	std::integer_sequence<uintmax_t, Ts...>,
	std::integer_sequence<uintmax_t, Us...>,
	std::integer_sequence<uintmax_t, Vs...>
>>
{
	static constexpr auto dims = sizeof...(Ts);
	static_assert(sizeof...(Us) == dims, "");
	static_assert(sizeof...(Vs) == dims, "");

	static constexpr auto bases = cindex<Ts...>;
	static constexpr auto extents = cindex<Us...>;
	static constexpr auto strides = cindex<Vs...>;
};

template <uintmax_t... Us>
struct range_literal_helper<cc::seq<
	std::integer_sequence<uintmax_t>,
	std::integer_sequence<uintmax_t, Us...>,
	std::integer_sequence<uintmax_t>
>>
{
	static constexpr auto dims = sizeof...(Us);
	static constexpr auto bases = cindex_n<dims, 0>;
	static constexpr auto extents = cindex<Us...>;
	static constexpr auto strides = cindex_n<dims, 1>;
};

template <uintmax_t... Ts, uintmax_t... Us>
struct range_literal_helper<cc::seq<
	std::integer_sequence<uintmax_t, Ts...>,
	std::integer_sequence<uintmax_t, Us...>,
	std::integer_sequence<uintmax_t>
>>
{
	static constexpr auto dims = sizeof...(Ts);
	static_assert(sizeof...(Us) == dims, "");

	static constexpr auto bases = cindex<Ts...>;
	static constexpr auto extents = cindex<Us...>;
	static constexpr auto strides = cindex_n<dims, 1>;
};

template <uintmax_t... Us, uintmax_t... Vs>
struct range_literal_helper<cc::seq<
	std::integer_sequence<uintmax_t>,
	std::integer_sequence<uintmax_t, Us...>,
	std::integer_sequence<uintmax_t, Vs...>
>>
{
	static constexpr auto dims = sizeof...(Us);
	static_assert(sizeof...(Vs) == dims, "");

	static constexpr auto bases = cindex_n<dims, 0>;
	static constexpr auto extents = cindex<Us...>;
	static constexpr auto strides = cindex<Vs...>;
};

}}

template <class Char, Char... Ts>
CC_ALWAYS_INLINE CC_CONST constexpr
auto operator"" _range() noexcept
{
	using seq = std::integer_sequence<Char, Ts...>;
	using result = cc::value_seqs<uintmax_t,
		cc::parse_wsv<uintmax_t, cc::parse_natural, ';', seq>>;
	static_assert(result::size() <= 3, "Too many semicolons.");

	using helper = nd::detail::range_literal_helper<result>;
	return make_range(helper::bases, helper::extents, helper::strides);
};

#endif
