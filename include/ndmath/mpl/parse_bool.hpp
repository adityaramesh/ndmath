/*
** File Name: parse_bool.hpp
** Author:    Aditya Ramesh
** Date:      08/01/2015
** Contact:   _@adityaramesh.com
*/

#ifndef ZFEB456D1_84AD_43DD_9D9E_449C5528DB22
#define ZFEB456D1_84AD_43DD_9D9E_449C5528DB22

#include <ndmath/mpl/common.hpp>

namespace nd {

template <class List>
struct parse_bool
{
	static constexpr auto is_true =
	mpl::starts_with<detail::true_list, List>::value;

	static constexpr auto is_false =
	mpl::starts_with<detail::false_list, List>::value;

	static_assert(
		is_true || is_false,
		"Input does not start with 'true' or 'false'."
	);

	using type = std::conditional_t<
		is_true, mpl::bool_<true>, mpl::bool_<false>
	>;

	/*
	** We can't use `slice` to extract the characters after the string
	** 'true' or 'false', since we would get an index out of bounds error if
	** the input matches 'true' or 'false' exactly.
	*/
	using actions = std::conditional_t<
		is_true,
		mpl::repeat_nc<4, mpl::quote<mpl::erase_front>>,
		mpl::repeat_nc<5, mpl::quote<mpl::erase_front>>
	>;

	using tail = mpl::fold<actions, List, mpl::reverse_args<mpl::quote<mpl::apply>>>;
};

}

#endif
