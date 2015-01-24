/*
** File Name: attribute.hpp
** Author:    Aditya Ramesh
** Date:      01/19/2015
** Contact:   _@adityaramesh.com
*/

#ifndef ZF3EF39F0_BFDC_412B_9107_0706F4B3BE3D
#define ZF3EF39F0_BFDC_412B_9107_0706F4B3BE3D

namespace nd {

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

template <size_t Coord, class Dir, class UnrollPolicy, class TilingPolicy>
struct attrib
{
	static constexpr auto coord = Coord;
	using dir           = Dir;
	using unroll_policy = UnrollPolicy;
	using tile_policy   = TilingPolicy;
};

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
