/*
** File Name: loop_attribute.hpp
** Author:    Aditya Ramesh
** Date:      01/19/2015
** Contact:   _@adityaramesh.com
**
** A loop nest is associated with a sequence of loops, and each loop is
** described by a loop attribute. Each attribute is associated with a direction,
** unroll policy, and tile policy.
*/

#ifndef ZF3EF39F0_BFDC_412B_9107_0706F4B3BE3D
#define ZF3EF39F0_BFDC_412B_9107_0706F4B3BE3D

namespace nd {

/*
** Policies for each loop attribute.
*/

class forward;
class backward;

template <size_t N, bool HasRem = true>
struct contiguous
{
	static constexpr auto factor = N;
	static constexpr auto has_rem = HasRem;
};

template <size_t N, bool HasRem = true>
struct split
{
	static constexpr auto factor = N;
	static constexpr auto has_rem = HasRem;
};

static constexpr auto full_unroll = size_t{0};
using full = contiguous<full_unroll, false>;
using none = contiguous<1, false>;

template <size_t N, bool HasRem>
struct tile_policy
{
	static constexpr auto factor = N;
	static constexpr auto has_rem = HasRem;
};

/*
** Definition of loop attribute.
*/
template <size_t Coord, class Dir, class UnrollPolicy, class TilingPolicy>
struct attrib
{
	static constexpr auto coord = Coord;
	using dir           = Dir;
	using unroll_policy = UnrollPolicy;
	using tile_policy   = TilingPolicy;
};

namespace detail {

template <class Loop>
struct default_attrib
{ using type = attrib<Loop::type::value, forward, none, none>; };

}

/*
** These are the default attributes that are used when a range is created.
*/
template <size_t Dims>
using default_attribs =
mpl::transform<
	mpl::range_c<size_t, 0, Dims - 1>,
	mpl::quote_trait<detail::default_attrib>
>;

/*
** Functions to modify an unroll or tile policy.
*/

namespace detail {

template <size_t N, bool HasRem, class Policy>
struct modify_policy_helper;

template <size_t N, bool HasRem, size_t OldN, size_t OldHasRem>
struct modify_policy_helper<N, HasRem, contiguous<OldN, OldHasRem>>
{ using type = contiguous<N, HasRem>; };

template <size_t N, bool HasRem, size_t OldN, size_t OldHasRem>
struct modify_policy_helper<N, HasRem, split<OldN, OldHasRem>>
{ using type = split<N, HasRem>; };

template <size_t N, bool HasRem, size_t OldN, size_t OldHasRem>
struct modify_policy_helper<N, HasRem, tile_policy<OldN, OldHasRem>>
{ using type = tile_policy<N, HasRem>; };

}

template <size_t N, bool HasRem, class Policy>
using modify_policy =
typename detail::modify_policy_helper<N, HasRem, Policy>::type;

template <class Coord, class Attrib>
using set_coord = attrib<
	Coord::type::value,
	typename Attrib::dir,
	typename Attrib::unroll_policy,
	typename Attrib::tile_policy
>;

template <class Dir, class Attrib>
using set_dir = attrib<
	Attrib::coord,
	Dir,
	typename Attrib::unroll_policy,
	typename Attrib::tile_policy
>;

template <class Policy, class Attrib>
using set_unroll_policy = attrib<
	Attrib::coord,
	typename Attrib::dir,
	Policy,
	typename Attrib::tile_policy
>;

template <class Policy, class Attrib>
using set_tile_policy = attrib<
	Attrib::coord,
	typename Attrib::dir,
	typename Attrib::unroll_policy,
	Policy
>;

}

#endif
