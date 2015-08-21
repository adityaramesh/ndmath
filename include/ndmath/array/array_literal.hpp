/*
** File Name: array_literal.hpp
** Author:    Aditya Ramesh
** Date:      08/01/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z5DB3281C_001D_4110_87BF_82896F9D2F8F
#define Z5DB3281C_001D_4110_87BF_82896F9D2F8F

#if defined(__clang__)
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wgnu-string-literal-operator-template"
#endif

#include <limits>
#include <boost/preprocessor/variadic/size.hpp>
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
	sc_range<(Ts::value - 1)...>;
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
	using list            = mpl::list<std::integral_constant<bool, Ts>...>;
	using underlying_type = underlying_type<bool>;

	static_assert(
		mpl::is_specialization_of<
			boolean_storage, underlying_type
		>::value, ""
	);

	using storage_type = typename underlying_type::storage_type;
	using type         = nd::pack_bool_list<storage_type, list>;
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

template <class String, class... Args>
struct match_front_helper
{
	using args = mpl::list<Args...>;

	using list = mpl::transform<
		mpl::apply_list<
			mpl::bind_front<
				mpl::quote<mpl::select>,
				mpl::zip<
					mpl::erase_back<args>,
					mpl::erase_front<args>
				>
			>,
			mpl::range_c<mpl::list_index, 0, args::size - 2, 2>
		>,
		mpl::compose<
			mpl::uncurry<mpl::make_list<
				mpl::bind_back<
					mpl::quote<mpl::starts_with>,
					String
				>,
				mpl::quote_trait<mpl::id>
			>>,
			mpl::uncurry<
				mpl::bind_back<mpl::quote<mpl::if_>, void>
			>
		>
	>;

	using match = mpl::find_if<
		mpl::compose<
			mpl::bind_back<mpl::quote<mpl::is_same>, void>,
			mpl::quote<mpl::not_>
		>,
		list
	>;

	using index = mpl::if_<
		mpl::equal_to<match, mpl::no_match>,
		mpl::list_index_c<0>,
		match
	>;

	using type = mpl::at<index, list>;
};

template <class String, class... Args>
using match_front = mpl::_t<match_front_helper<String, Args...>>;

template <class String>
using parse_scalar_type = match_front<
	String,
	mpl_string("char"),               char,
	mpl_string("signed char"),        signed char,
	mpl_string("unsigned char"),      unsigned char,
	mpl_string("short"),              short,
	mpl_string("signed short"),       signed short,
	mpl_string("unsigned short"),     unsigned short,
	mpl_string("int"),                int,
	mpl_string("signed"),             signed,
	mpl_string("signed int"),         signed int,
	mpl_string("unsigned"),           unsigned,
	mpl_string("unsigned int"),       unsigned int,
	mpl_string("long"),               long,
	mpl_string("signed long"),        signed long,
	mpl_string("unsigned long"),      unsigned long,
	mpl_string("long long"),          long long,
	mpl_string("signed long long"),   signed long long,
	mpl_string("unsigned long long"), unsigned long long,
	mpl_string("int8_t"),             int8_t,
	mpl_string("uint8_t"),            uint8_t,
	mpl_string("int_fast8_t"),        int_fast8_t,
	mpl_string("uint_fast8_t"),       uint_fast8_t,
	mpl_string("int16_t"),            int16_t,
	mpl_string("uint16_t"),           uint16_t,
	mpl_string("int_fast16_t"),       int_fast16_t,
	mpl_string("uint_fast16_t"),      uint_fast16_t,
	mpl_string("int32_t"),            int32_t,
	mpl_string("uint32_t"),           uint32_t,
	mpl_string("int_fast32_t"),       int_fast32_t,
	mpl_string("uint_fast32_t"),      uint_fast32_t,
	mpl_string("int64_t"),            int64_t,
	mpl_string("uint64_t"),           uint64_t,
	mpl_string("int_fast64_t"),       int_fast64_t,
	mpl_string("uint_fast64_t"),      uint_fast64_t,
	mpl_string("intmax_t"),           intmax_t,
	mpl_string("uintmax_t"),          uintmax_t,
	mpl_string("intptr_t"),           intptr_t,
	mpl_string("uintptr_t"),          uintptr_t,
	mpl_string("float"),              float,
	mpl_string("double"),             double,
	mpl_string("long double"),        long double
>;

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

	using parser = parse_array<scalar_parser, input>;
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

template <class Char, Char... Ts>
CC_ALWAYS_INLINE constexpr
auto operator"" _array_explicit() noexcept
{
	using input = mpl::list<mpl::char_<Ts>...>;
	using index = mpl::find<mpl::char_<'['>, input>;

	static_assert(!std::is_same<index, mpl::no_match>::value, "Expected '['.");
	static_assert(index::value != 0, "Expected scalar type before '['.");

	using scalar_str = mpl::slice_c<0, index::value - 1, input>;
	using array_str  = mpl::erase_front_n<index, input>;

	using parser = nd::parse_array<mpl::quote<nd::parse_decimal>, array_str>;
	using state  = typename parser::type;
	using lists  = typename state::lists;
										
	using array            = mpl::at_c<0, lists>;
	using flattened_array  = flatten_list<array>;
	constexpr auto extents = detail::list_to_range<typename state::extents>;

	using scalar = detail::parse_scalar_type<scalar_str>;
	static_assert(!std::is_same<scalar, void>::value, "Unknown scalar type.");

	return make_sarray<scalar>(flattened_array{}, extents);
}

}

using nd::operator"" _array;
using nd::operator"" _array_explicit;

#define nd_array_1(x) #x ## _array
#define nd_array_2(scalar, x) #scalar #x ## _array_explicit

#define nd_array_helper_2(n, ...) nd_array_ ## n(__VA_ARGS__)
#define nd_array_helper_1(n, ...) nd_array_helper_2(n, __VA_ARGS__)
#define nd_array(...) nd_array_helper_1(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), __VA_ARGS__)

#define nd_darray(...) ::nd::make_darray(nd_array(__VA_ARGS__))

#if defined(__clang__)
	#pragma GCC diagnostic pop
#endif

#endif
