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
	static constexpr auto is_true  = mpl::at_c<0, List>::value == 't';
	static constexpr auto is_false = mpl::at_c<0, List>::value == 'f';

	static_assert(
		is_true || is_false,
		"Input does not start with 't' or 'f'."
	);

	using type = std::conditional_t<
		is_true, mpl::bool_<true>, mpl::bool_<false>
	>;

	/*
	** We accept either 't' or 'true' for true, and 'f' or 'false' for
	** false.
	*/
	using count = mpl::if_<
		mpl::starts_with<detail::true_list, List>,
		mpl::list_index_c<4>,
		mpl::if_<
			mpl::starts_with<detail::false_list, List>,
			mpl::list_index_c<5>,
			mpl::list_index_c<1>
		>
	>;

	using tail = mpl::erase_front_n<count, List>;
};

}

#endif
