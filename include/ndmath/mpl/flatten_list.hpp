/*
** File Name: flatten_list.hpp
** Author:    Aditya Ramesh
** Date:      08/01/2015
** Contact:   _@adityaramesh.com
*/

#ifndef ZAB8647FE_3AA2_4F59_BC1B_ACECDEC17417
#define ZAB8647FE_3AA2_4F59_BC1B_ACECDEC17417

#include <ndmath/mpl/common.hpp>

namespace nd {
namespace detail {

template <class T>
struct flatten_list
{ using type = mpl::list<T>; };

template <>
struct flatten_list<mpl::list<>>
{ using type = mpl::list<>; };

template <class... Ts>
struct flatten_list<mpl::list<Ts...>>
{
	using type = mpl::join<mpl::list<
		typename flatten_list<Ts>::type...
	>>;
};

template <class Lists>
using flatten_list_helper = mpl::foldl<
	Lists,
	mpl::list<>,
	mpl::compose<
		mpl::quote<mpl::list>,
		mpl::uncurry<mpl::combine<
			mpl::quote_trait<mpl::id>,
			mpl::quote_trait<detail::flatten_list>
		>>,
		mpl::uncurry<mpl::quote<mpl::cat>>
	>
>;

}

template <class Lists>
using flatten_list = typename detail::flatten_list_helper<Lists>::type;

}

#endif
