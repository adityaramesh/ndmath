/*
** File Name: array_literal.hpp
** Author:    Aditya Ramesh
** Date:      08/01/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z5DB3281C_001D_4110_87BF_82896F9D2F8F
#define Z5DB3281C_001D_4110_87BF_82896F9D2F8F

#include <limits>
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

template <class List>
struct pack_array_helper
{ using type = List; };

template <bool... Ts>
struct pack_array_helper<mpl::list<std::integral_constant<bool, Ts>...>>
{
	using list = mpl::list<std::integral_constant<bool, Ts>...>;
	using underlying_type = underlying_type<bool>;
	using type = nd::pack_bool_list<underlying_type, list>;
};

template <class List>
using pack_array = mpl::_t<pack_array_helper<List>>;

template <bool IsBoolType, bool IsFPType, class Array>
struct deduce_scalar_type_helper;

template <class Array>
struct deduce_scalar_type_helper<true, false, Array>
{ using type = bool; };

template <class Array>
struct deduce_scalar_type_helper<false, true, Array>
{ using type = float; };

template <class Ratio>
using get_num = mpl::intmax_t<Ratio::num>;

template <class Array>
struct deduce_scalar_type_helper<false, false, Array>
{
	using min = mpl::apply_list<
		mpl::quote<mpl::min>,
		mpl::transform<Array, mpl::quote_trait<get_num>>
	>;

	using max = mpl::apply_list<
		mpl::quote<mpl::max>,
		mpl::transform<Array, mpl::quote_trait<get_num>>
	>;

	static constexpr auto use_signed = min::value < 0;
	using small_scalar = std::conditional_t<use_signed, int, unsigned>;
	using large_scalar = std::conditional_t<use_signed, long, unsigned long>;

	static_assert(
		max::value <= std::numeric_limits<large_scalar>::max(),
		"Range of integral values in the array cannot be represented "
		"by a fundamental integral type."
	);
	
	using type = std::conditional_t<
		max::value <= std::numeric_limits<small_scalar>::max(),
		small_scalar, large_scalar
	>;
};

template <bool IsBoolType, bool IsFPType, class Array>
using deduce_scalar_type = mpl::_t<deduce_scalar_type_helper<
	IsBoolType, IsFPType, Array>>;

}

template <class Char, Char... Ts>
CC_ALWAYS_INLINE constexpr
auto operator"" _array() noexcept
{
	using tf_list = mpl::list<mpl::char_<'t'>, mpl::char_<'f'>>;

	using is_tf = mpl::compose<
		mpl::bind_back<mpl::quote<mpl::find>, tf_list>,
		mpl::bind_back<mpl::quote<mpl::not_equal_to>, mpl::no_match>
	>;

	using input = mpl::list<mpl::char_<Ts>...>;

	constexpr auto is_bool_type = !std::is_same<
		mpl::find_if<is_tf, input>, mpl::no_match
	>::value;

	constexpr auto is_fp_type = !std::is_same<
		mpl::find<mpl::char_<'.'>, input>, mpl::no_match
	>::value;

	using scalar_parser = std::conditional_t<
		is_bool_type,
		mpl::quote<parse_bool>,
		mpl::quote<parse_decimal>
	>;

	using parser = nd::parse_array<scalar_parser, input>;
	using state  = typename parser::type;
	using lists  = typename state::lists;

	/*
	** `lists` is a list of lists, where sublist `i` is used to store the
	** results at level `i` (from 0 to n - 1, where n is the number of
	** dimensions). Each time a `]` is encountered, the list from level
	** `i + 1` is appended to the list at level `i`. So at the end of
	** parsing, the array is the first sublist in `lists`.
	*/
	using array           = mpl::at_c<0, lists>;
	using flattened_array = flatten_list<array>;
	using packed_array    = detail::pack_array<flattened_array>;

	using scalar = detail::deduce_scalar_type<
		is_bool_type, is_fp_type, packed_array
	>;

	constexpr auto extents = detail::list_to_range<typename state::extents>;

	return make_sarray<scalar>(packed_array{}, extents);
}

}

using nd::operator"" _array;

#define nd_array(x) #x ## _array

// TODO: for the version of the macro that accepts the data type, define literal
// operators for _X_array, for X in {int8_t, int16_t, int32_t, int64_t, ...}.

#endif
