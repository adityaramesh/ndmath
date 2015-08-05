/*
** File Name: parse_decimal.hpp
** Author:    Aditya Ramesh
** Date:      07/27/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z1DD730BE_54FA_4BB4_9F58_BD7608DB07B0
#define Z1DD730BE_54FA_4BB4_9F58_BD7608DB07B0

#include <ratio>
#include <ndmath/mpl/parse_integer.hpp>

namespace nd {
namespace detail {

template <class T>
CC_ALWAYS_INLINE constexpr
auto abs(const T x) noexcept
{ return x >= 0 ? x : -x; }

template <class T>
CC_ALWAYS_INLINE constexpr
auto sign(const T x) noexcept
{ return x >= 0 ? 1 : -1; }

template <class I1, class I2>
CC_ALWAYS_INLINE constexpr
auto pow(const I1 z, const I2 p) noexcept
{
	if (p == 0) return 1;

	auto v = z;
	for (auto i = I2{0}; i != p - 1; ++i) {
		v *= z;
	}
	return v;
}

template <class List>
struct has_frac_part_helper :
std::false_type {};

template <class T, class... Ts>
struct has_frac_part_helper<mpl::list<mpl::char_<'.'>, T, Ts...>>
{
	static constexpr auto value =
	mpl::apply<detail::is_digit, T>::value;
};

template <class... Ts>
struct has_frac_part_helper<mpl::list<mpl::char_<'e'>, Ts...>> :
std::false_type {};

template <class... Ts>
struct has_frac_part_helper<mpl::list<mpl::char_<'E'>, Ts...>> :
std::false_type {};

template <class List>
struct has_exp_part_helper :
std::false_type {};

template <class... Ts>
struct has_exp_part_helper<mpl::list<mpl::char_<'e'>, Ts...>> :
std::true_type {};

template <class... Ts>
struct has_exp_part_helper<mpl::list<mpl::char_<'E'>, Ts...>> :
std::true_type {};

template <bool HasWholePart, class List>
struct parse_decimal_helper_1;

template <bool HasFracPart, class List>
struct parse_decimal_helper_2;

template <bool HasExpPart, class List>
struct parse_decimal_helper_3;

template <class List>
struct parse_decimal_helper_3<true, List>
{
	using seq_no_e = mpl::erase_front<List>;

	static_assert(
		seq_no_e::size != 0,
		"Unexpected end of sequence."
	);

	using parser = parse_integer<seq_no_e>;
	using exp    = typename parser::type;
	using tail   = typename parser::tail;

	static constexpr auto sign = nd::detail::sign(exp::value);
	static constexpr auto mag  = pow(10, abs(exp::value));

	using exp_part = std::conditional_t<
		sign == 1, std::ratio<mag, 1>, std::ratio<1, mag>
	>;
};

template <class List>
struct parse_decimal_helper_3<false, List>
{
	using tail     = List;
	using exp_part = std::ratio<1, 1>;
};

template <class List>
struct parse_decimal_helper_2<true, List>
{
	using seq_no_dot = mpl::erase_front<List>;
	using parser     = parse_integer<seq_no_dot>;
	using result     = typename parser::type;
	using num        = std::ratio<result::value, 1>;
	using rest       = typename parser::tail;

	using length    = mpl::minus<mpl::size<seq_no_dot>, mpl::size<rest>>;
	using den       = std::ratio<1, pow(10, length::value)>;
	using frac_part = std::ratio_multiply<num, den>;

	static constexpr auto has_exp_part =
	has_exp_part_helper<rest>::value;

	using helper    = parse_decimal_helper_3<has_exp_part, rest>;
	using tail      = typename helper::tail;
	using exp_part  = typename helper::exp_part;
};

template <class List>
struct remove_dot_helper
{ using type = List; };

template <class... Ts>
struct remove_dot_helper<mpl::list<mpl::char_<'.'>, Ts...>>
{ using type = mpl::list<Ts...>; };

template <class List>
struct parse_decimal_helper_2<false, List>
{
	using seq_no_dot = typename remove_dot_helper<List>::type;

	static constexpr auto has_exp_part =
	has_exp_part_helper<seq_no_dot>::value;

	using frac_part = std::ratio<0, 1>;
	using helper    = parse_decimal_helper_3<has_exp_part, seq_no_dot>;
	using tail      = typename helper::tail;
	using exp_part  = typename helper::exp_part;
};

template <class List>
struct parse_decimal_helper_1<true, List>
{
	using parser     = parse_integer<List>;
	using result     = typename parser::type;
	using whole_part = std::ratio<result::value, 1>;
	using rest       = typename parser::tail;

	static constexpr auto has_frac_part =
	has_frac_part_helper<rest>::value;

	using helper    = parse_decimal_helper_2<has_frac_part, rest>;
	using tail      = typename helper::tail;
	using frac_part = typename helper::frac_part;
	using exp_part  = typename helper::exp_part;

	using type = std::ratio_multiply<
		std::ratio_add<whole_part, frac_part>, exp_part
	>;
};

template <class List>
struct parse_decimal_helper_1<false, List>
{
	static constexpr auto has_frac_part =
	has_frac_part_helper<List>::value;

	using whole_part = std::ratio<0, 1>;
	using helper     = parse_decimal_helper_2<has_frac_part, List>;
	using tail       = typename helper::tail;
	using frac_part  = typename helper::frac_part;
	using exp_part   = typename helper::exp_part;
	using type       = std::ratio_multiply<frac_part, exp_part>;
};

}

template <class List>
struct parse_decimal
{
	/*
	** Let E := [Ee][+-]?\d+, W := \d+, and F := \.\d+. This struct parses a
	** decimal represented by a string whose head matches one of the
	** following regexes:
	** 1. [+-]?{W}{F}?{E}?
	** 2. [+-]?{W}?{F}{E}?
	** 3. [+-]?{W}{F}{E}?
	*/

	using valid_init_chars = mpl::cat<detail::digits, mpl::to_types<
		std::integer_sequence<char, '+', '-', '.'>>>;

	static_assert(List::size != 0, "Sequence is empty.");

	static_assert(!std::is_same<
		mpl::find<mpl::at_c<0, List>, valid_init_chars>,
		mpl::no_match
	>::value, "Expected '+', '-', '.', or a digit.");

	using sign = std::conditional_t<
		mpl::at_c<0, List>::value == '-',
		std::ratio<-1, 1>, std::ratio<1, 1>
	>;

	/*
	** We perform some initial syntax checking, and extract the sign of the
	** sequence.
	*/

	using start = std::conditional_t<
		mpl::at_c<0, List>::value == '+' ||
		mpl::at_c<0, List>::value == '-',
		mpl::list_index_c<1>, mpl::list_index_c<0>
	>;

	// If the first token is a plus or minus sign, then the length of the
	// sequence must be greater than one.
	static_assert(
		!(start::value == 1 && List::size == 1),
		"Expected digit or '.'."
	);

	// If the first token was a plus or minus sign, then the succeeding
	// character must be a digit or '.'.
	static_assert(
		mpl::apply<detail::is_digit, mpl::at<start, List>>::value ||
		mpl::at<start, List>::value == '.',
		"Expected digit or '.'."
	);

	using seq_no_sign = mpl::erase_front_n<start, List>;

	/*
	** Unlike the helper structs that parse the fractional and exponent
	** parts of the decimal, we don't need a separate utility struct to
	** check whether the decimal has a whole part. This is because `start`
	** cannot be out of bounds, provided that the sequence is a well-formed
	** decimal as defined by our grammar. So there is no risk of triggering
	** a compile-time error here.
	*/

	static constexpr auto has_whole_part = mpl::at<start, List>::value != '.';
	using helper = detail::parse_decimal_helper_1<has_whole_part, seq_no_sign>;

	using unsigned_type = typename helper::type;
	using type          = std::ratio_multiply<sign, unsigned_type>;
	using tail          = typename helper::tail;

	using exp_part = typename helper::exp_part;
};

}

#endif
