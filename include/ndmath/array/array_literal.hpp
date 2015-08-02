/*
** File Name: array_literal.hpp
** Author:    Aditya Ramesh
** Date:      08/01/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z5DB3281C_001D_4110_87BF_82896F9D2F8F
#define Z5DB3281C_001D_4110_87BF_82896F9D2F8F

#include <ndmath/array/dense_storage.hpp>
#include <ndmath/mpl/parse_bool.hpp>
#include <ndmath/mpl/parse_decimal.hpp>
#include <ndmath/mpl/parse_array.hpp>
#include <ndmath/mpl/flatten_list.hpp>
#include <ndmath/mpl/pack_bools.hpp>

namespace nd {
namespace detail {

template <class List>
struct list_to_range_helper;

template <class... Ts>
struct list_to_range_helper<mpl::list<Ts...>>
{
	static constexpr auto value =
	sc_range<Ts::value...>;
};

template <class List>
static constexpr auto list_to_range =
list_to_range_helper<List>::value;

}

template <class Char, Char... Ts>
CC_ALWAYS_INLINE constexpr
auto operator"" _array() noexcept
{
	using input        = mpl::list<mpl::char_<Ts>...>;
	using parser       = nd::parse_array<mpl::quote<parse_decimal>, input>;
	using state        = typename parser::type;
	using list         = flatten_list<typename state::lists>;
	using extents_list = typename state::extents;

	constexpr auto extents =
	detail::list_to_range<extents_list>;

	return make_sarray<float>(list{}, extents);
}

}

using nd::operator"" _array;

#define nd_array(x) #x ## _array

#endif
