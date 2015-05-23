/*
** File Name: metaparse.hpp
** Author:    Aditya Ramesh
** Date:      05/23/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z32EEA35E_F66C_4BCC_9ADD_14ED1DA13DE0
#define Z32EEA35E_F66C_4BCC_9ADD_14ED1DA13DE0

#include <ndmath/common.hpp>

namespace nd {

using parse_nonneg_int = mpl::bind_back<
	mpl::quote<mpl::foldl>,
	mpl::int_<0>,
	mpl::compose<
		mpl::quote<mpl::list>,
		mpl::uncurry<mpl::combine<
			mpl::bind_front<mpl::quote<mpl::multiplies>, mpl::int_<10>>,
			mpl::bind_back<mpl::quote<mpl::minus>, mpl::char_<'0'>>
		>>,
		mpl::uncurry<mpl::quote<mpl::plus>>
	>
>;

template <char... Ts>
static constexpr auto parse_nonneg_int_c =
mpl::apply<
	parse_nonneg_int,
	mpl::to_types<std::integer_sequence<char, Ts...>>
>::value;

}

#endif
