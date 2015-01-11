/*
** File Name: constant_index.hpp
** Author:    Aditya Ramesh
** Date:      01/08/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z3AA81C53_00C1_4028_9A17_85048FB3164F
#define Z3AA81C53_00C1_4028_9A17_85048FB3164F

#include <array>
#include <ndmath/index/index_base.hpp>
#include <ccbase/utility/sequence_operations.hpp>

namespace nd {

template <class Integer, Integer... Ts>
class constant_index final :
public index_base<
	sizeof...(Ts), true, Integer, Integer,
	constant_index<Integer, Ts...>
>
{
	using self = constant_index<Integer, Ts...>;
	using base = index_base<sizeof...(Ts), true, Integer, Integer, self>;

	static constexpr std::array<Integer, sizeof...(Ts)>
	m_indices{{Ts...}};
public:
	using base::operator();

	template <class T>
	CC_ALWAYS_INLINE CC_CONST
	constexpr auto operator()(const T& n)
	const noexcept { return m_indices[n]; }
};

template <class Integer, Integer... Ts>
constexpr std::array<Integer, sizeof...(Ts)>
constant_index<Integer, Ts...>::m_indices;

template <class Integer, Integer... Ts>
static constexpr auto basic_cindex = constant_index<Integer, Ts...>{};

template <uint_fast32_t... Ts>
static constexpr auto cindex = constant_index<uint_fast32_t, Ts...>{};

namespace detail {

template <class Sequence>
struct basic_cindex_n_helper;

template <class Integer, Integer... Ts>
struct basic_cindex_n_helper<std::integer_sequence<Integer, Ts...>>
{
	CC_ALWAYS_INLINE CC_CONST constexpr
	static auto apply() noexcept
	{ return basic_cindex<Integer, Ts...>; }
};

}

template <class Integer, size_t N, size_t Value>
static constexpr auto basic_cindex_n =
detail::basic_cindex_n_helper<cc::constant_sequence<Integer, N, Value>>::apply();

template <size_t N, size_t Value>
static constexpr auto cindex_n =
basic_cindex_n<uint_fast32_t, N, Value>;

}

#endif
