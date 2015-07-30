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

template <class Seq>
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

template <class Seq>
struct has_exp_part_helper :
std::false_type {};

template <class... Ts>
struct has_exp_part_helper<mpl::list<mpl::char_<'e'>, Ts...>> :
std::true_type {};

template <class... Ts>
struct has_exp_part_helper<mpl::list<mpl::char_<'E'>, Ts...>> :
std::true_type {};

template <bool HasWholePart, class Seq>
struct parse_decimal_helper_1;

template <bool HasFracPart, class Seq>
struct parse_decimal_helper_2;

template <bool HasExpPart, class Seq>
struct parse_decimal_helper_3;

template <class Seq>
struct parse_decimal_helper_3<true, Seq>
{
	using seq_no_e = mpl::erase_front<Seq>;

	static_assert(
		seq_no_e::size() != 0,
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

template <class Seq>
struct parse_decimal_helper_3<false, Seq>
{
	using tail     = Seq;
	using exp_part = std::ratio<1, 1>;
};

template <class Seq>
struct parse_decimal_helper_2<true, Seq>
{
	using seq_no_dot = mpl::erase_front<Seq>;
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

template <class Seq>
struct remove_dot_helper
{ using type = Seq; };

template <class... Ts>
struct remove_dot_helper<mpl::list<mpl::char_<'.'>, Ts...>>
{ using type = mpl::list<Ts...>; };

template <class Seq>
struct parse_decimal_helper_2<false, Seq>
{
	using seq_no_dot = typename remove_dot_helper<Seq>::type;

	static constexpr auto has_exp_part =
	has_exp_part_helper<seq_no_dot>::value;

	using frac_part = std::ratio<0, 1>;
	using helper    = parse_decimal_helper_3<has_exp_part, seq_no_dot>;
	using tail      = typename helper::tail;
	using exp_part  = typename helper::exp_part;
};

template <class Seq>
struct parse_decimal_helper_1<true, Seq>
{
	using parser     = parse_integer<Seq>;
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

template <class Seq>
struct parse_decimal_helper_1<false, Seq>
{
	static constexpr auto has_frac_part =
	has_frac_part_helper<Seq>::value;

	using whole_part = std::ratio<0, 1>;
	using helper     = parse_decimal_helper_2<has_frac_part, Seq>;
	using tail       = typename helper::tail;
	using frac_part  = typename helper::frac_part;
	using exp_part   = typename helper::exp_part;
	using type       = std::ratio_multiply<frac_part, exp_part>;
};

}

template <class Seq>
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

	static_assert(Seq::size() != 0, "Sequence is empty.");

	static_assert(!std::is_same<
		mpl::find_first<mpl::at_c<0, Seq>, valid_init_chars>,
		mpl::no_match
	>::value, "Expected '+', '-', '.', or a digit.");

	using sign = std::conditional_t<
		mpl::at_c<0, Seq>::value == '-',
		std::ratio<-1, 1>, std::ratio<1, 1>
	>;

	/*
	** We perform some initial syntax checking, and extract the sign of the
	** sequence.
	*/

	using start = std::conditional_t<
		mpl::at_c<0, Seq>::value == '+' ||
		mpl::at_c<0, Seq>::value == '-',
		mpl::size_t<1>, mpl::size_t<0>
	>;

	// If the first token is a plus or minus sign, then the length of the
	// sequence must be greater than one.
	static_assert(
		!(start::value == 1 && Seq::size() == 1),
		"Expected digit or '.'."
	);

	// If the first token was a plus or minus sign, then the succeeding
	// character must be a digit or '.'.
	static_assert(
		mpl::apply<detail::is_digit, mpl::at<start, Seq>>::value ||
		mpl::at<start, Seq>::value == '.',
		"Expected digit or '.'."
	);

	using seq_no_sign = mpl::slice_c<start::value, Seq::size() - 1, Seq>;

	/*
	** Unlike the helper structs that parse the fractional and exponent
	** parts of the decimal, we don't need a separate utility struct to
	** check whether the decimal has a whole part. This is because `start`
	** cannot be out of bounds, provided that the sequence is a well-formed
	** decimal as defined by our grammar. So there is no risk of triggering
	** a compile-time error here.
	*/

	static constexpr auto has_whole_part = mpl::at<start, Seq>::value != '.';
	using helper = detail::parse_decimal_helper_1<has_whole_part, seq_no_sign>;

	using unsigned_type = typename helper::type;
	using type          = std::ratio_multiply<sign, unsigned_type>;
	using tail          = typename helper::tail;

	using exp_part = typename helper::exp_part;
};

}

#endif
