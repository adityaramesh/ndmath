/*
** File Name: common.hpp
** Author:    Aditya Ramesh
** Date:      07/29/2015
** Contact:   _@adityaramesh.com
*/

#ifndef ZA4CA8234_6F78_4751_B97A_32C9985EA66D
#define ZA4CA8234_6F78_4751_B97A_32C9985EA66D

#include <ndmath/common.hpp>

namespace nd {
namespace detail {

using digits = mpl::range_c<char, '0', '9'>;

using is_digit = mpl::compose<
	mpl::bind_back<mpl::quote<mpl::find_first>, digits>,
	mpl::bind_back<mpl::quote<mpl::not_equal_to>, mpl::no_match>
>;

using is_non_digit = mpl::compose<
	mpl::bind_back<mpl::quote<mpl::find_first>, digits>,
	mpl::bind_back<mpl::quote<mpl::equal_to>, mpl::no_match>
>;

using whitespace = mpl::to_types<std::integer_sequence<char, ' ', '\t', '\n',
      '\v', '\f', '\v'>>;

using is_whitespace = mpl::compose<
	mpl::bind_back<mpl::quote<mpl::find_first>, whitespace>,
	mpl::bind_back<mpl::quote<mpl::not_equal_to>, mpl::no_match>
>;

using is_non_whitespace = mpl::compose<
	mpl::bind_back<mpl::quote<mpl::find_first>, whitespace>,
	mpl::bind_back<mpl::quote<mpl::not_equal_to>, mpl::no_match>
>;

}}

#endif
