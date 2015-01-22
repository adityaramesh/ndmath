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

static constexpr auto full_unroll = size_t{0};

struct full
{
	static constexpr auto factor = full_unroll;
	static constexpr auto has_rem = false;
};

struct none
{
	static constexpr auto factor = size_t{1};
	static constexpr auto has_rem = false;
};

template <size_t N, bool HasRem>
struct contiguous
{
	static constexpr auto factor = N;
	static constexpr auto has_rem = HasRem;
};

template <size_t N, bool HasRem>
struct split
{
	static constexpr auto factor = N;
	static constexpr auto has_rem = HasRem;
};

template <size_t N, bool HasRem>
struct tile_policy
{
	static constexpr auto factor = N;
	static constexpr auto has_rem = HasRem;
};

template <size_t Coord, class Dir, class UnrollPolicy, class TilingPolicy>
struct attrib
{
	static constexpr auto coord = Coord;
	using dir = Dir;
	using unroll_policy = UnrollPolicy;
	using tile_policy = TilingPolicy;
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
