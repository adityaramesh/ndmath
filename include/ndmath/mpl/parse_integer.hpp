/*
** File Name: parse_integer.hpp
** Author:    Aditya Ramesh
** Date:      07/27/2015
** Contact:   _@adityaramesh.com
*/

#ifndef ZDC849833_AE7B_4119_AF66_FA85854175D8
#define ZDC849833_AE7B_4119_AF66_FA85854175D8

#include <ndmath/mpl/common.hpp>

namespace nd {

template <class List>
struct parse_integer
{
	/*
	** Parses an integer represented by a string whose head matches the
	** regex "[+-]\d+".
	*/

	using valid_init_chars = mpl::cat<detail::digits, mpl::to_types<
		std::integer_sequence<char, '+', '-'>>>;

	static_assert(List::size != 0, "Sequence is empty.");

	static_assert(!std::is_same<
		mpl::find<mpl::at_c<0, List>, valid_init_chars>,
		mpl::no_match
	>::value, "Expected '+', '-', or a digit.");

	using sign = std::conditional_t<
		mpl::at_c<0, List>::value == '-',
		mpl::intmax_t<-1>, mpl::intmax_t<1>
	>;

	/*
	** Extract the part of the sequence containing the integer.
	*/

	using start = std::conditional_t<
		mpl::at_c<0, List>::value == '+' ||
		mpl::at_c<0, List>::value == '-',
		mpl::list_index_c<1>, mpl::list_index_c<0>
	>;

	using seq_no_sign = mpl::erase_front_n<start, List>;

	using match = mpl::find_if<detail::is_non_digit, seq_no_sign>;

	using end = std::conditional_t<
		std::is_same<match, mpl::no_match>::value,
		mpl::minus<mpl::size<List>, mpl::list_index_c<1>>,
		mpl::minus<mpl::plus<match, start>, mpl::list_index_c<1>>
	>;

	using string = mpl::slice<start, end, List>;

	using parser = mpl::bind_back<
		mpl::quote<mpl::fold>,
		mpl::int_<0>,
		mpl::compose<
			mpl::quote<mpl::list>,
			mpl::uncurry<mpl::make_list<
				mpl::bind_front<mpl::quote<mpl::multiplies>, mpl::int_<10>>,
				mpl::bind_back<mpl::quote<mpl::minus>, mpl::char_<'0'>>
			>>,
			mpl::uncurry<mpl::quote<mpl::plus>>
		>
	>;

	using unsigned_type = mpl::apply<parser, string>;
	using type = mpl::multiplies<sign, unsigned_type>;
	using tail = mpl::erase_front_n<mpl::inc<end>, List>;
};

template <class List>
static constexpr auto parse_integer_c =
mpl::apply<mpl::quote_trait<parse_integer>, List>::value;

}

#endif
