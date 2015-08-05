/*
** File Name: loop_transformation.hpp
** Author:    Aditya Ramesh
** Date:      01/19/2015
** Contact:   _@adityaramesh.com
**
** Definitions of the loop transformation functions used for loop optimization.
*/

#ifndef Z4591CB1E_C8EE_424D_BDBB_B0625A28BB62
#define Z4591CB1E_C8EE_424D_BDBB_B0625A28BB62

#include <ndmath/range/loop_attribute.hpp>

namespace nd {
namespace detail {

/*
** Given a permutation in cycle notation (e.g. (1 4 3 5)), returns a
** decomposition into two-cycles. Note that we assume permutations are applied
** from left to right. So (1 4 3 5) decomposes into (5 3)(3 4)(4 1), *not* (1
** 4)(4 3)(3 5)!
*/
template <class Perms>
using two_cycles =
mpl::zip<
	mpl::reverse<mpl::erase_front<Perms>>,
	mpl::reverse<mpl::erase_back<Perms>>
>;

template <class Loop, class Attribs>
struct reverse_loop
{
	using attrib = mpl::at<Loop, Attribs>;
	using new_dir = std::conditional_t<
		std::is_same<typename attrib::dir, forward>::value,
		backward, forward
	>;
	using new_attrib = set_dir<new_dir, attrib>;
	using type = mpl::set_at<Loop, new_attrib, Attribs>;
};

}

template <class Loops, class Attribs>
using reverse_loops =
mpl::fold<
	Loops, Attribs,
	mpl::reverse_args<mpl::quote_trait<detail::reverse_loop>>
>;

namespace detail {

template <class Trans, class Attribs>
struct apply_trans
{
	using i = mpl::at_c<0, Trans>;
	using j = mpl::at_c<1, Trans>;
	using a = mpl::at<i, Attribs>;
	using b = mpl::at<j, Attribs>;
	using type = mpl::set_at<i, b,
		mpl::set_at<j, a, Attribs>
	>;
};

}

template <class Perm, class Attribs>
using apply_perm =
mpl::fold<
	detail::two_cycles<Perm>, Attribs,
	mpl::reverse_args<mpl::quote_trait<detail::apply_trans>>
>;

template <size_t Loop, class Policy, class Attribs>
using set_loop_unroll_policy =
mpl::set_at_c<
	Loop,
	set_unroll_policy<Policy, mpl::at_c<Loop, Attribs>>,
	Attribs
>;

template <size_t Loop, class Policy, class Attribs>
using set_loop_tile_policy =
mpl::set_at_c<
	Loop,
	set_tile_policy<Policy, mpl::at_c<Loop, Attribs>>,
	Attribs
>;

}

#endif
