/*
** File Name: attribute_traits.hpp
** Author:    Aditya Ramesh
** Date:      01/19/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z4591CB1E_C8EE_424D_BDBB_B0625A28BB62
#define Z4591CB1E_C8EE_424D_BDBB_B0625A28BB62

#include <ndmath/range/attribute.hpp>

namespace nd {
namespace detail {

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
mpl::foldl<
	Loops,
	Attribs,
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
mpl::foldl<
	mpl::two_cycles<Perm>,
	Attribs,
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

namespace detail {

template <class Loop>
struct default_attrib
{
	using type = attrib<Loop::type::value, forward, none, none>;
};

}

template <size_t Dims>
using default_attribs =
mpl::transform<
	mpl::range_c<size_t, 0, Dims - 1>,
	mpl::quote_trait<detail::default_attrib>
>;

}

#endif
