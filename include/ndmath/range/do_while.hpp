/*
** File Name: do_while.hpp
** Author:    Aditya Ramesh
** Date:      07/19/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z69A569F7_1502_431F_8D01_F893B3221436
#define Z69A569F7_1502_431F_8D01_F893B3221436

#include <ndmath/range/loop_optimization.hpp>

namespace nd {
namespace detail {
namespace do_while {

template <
	size_t Cur,
	size_t End,
	size_t Factor,
	class Policy,
	class Dir,
	bool Noexcept
>
struct unroll_helper;

/*
** There are two types of unrolling: contiguous and split. Contiguous unrolling
** requires the offset (i.e. the start of the range), index, and stride; split
** unrolling requires the index, offset, and extent of the outer loop.
** Consequently, the `apply` function takes all four inputs as parameters, along
** with the function to be invoked at each iteration.
*/

/*
** Special case where the unroll factor is one, i.e. no unrolling is performed.
*/
template <
	size_t Cur,
	size_t End,
	class Policy,
	class Dir,
	bool Noexcept
>
struct unroll_helper<Cur, End, 1, Policy, Dir, Noexcept>
{
	template <class Integer, class Func>
	CC_ALWAYS_INLINE
	static auto apply(
		const Integer& off,
		const Integer& idx,
		Integer, Integer,
		const Func& f
	)
	noexcept(Noexcept) { return f(off + idx); }
};

template <size_t End, size_t Factor, class Policy, class Dir, bool Noexcept>
struct unroll_helper<End, End, Factor, Policy, Dir, Noexcept>
{
	template <class Integer, class Func>
	CC_ALWAYS_INLINE constexpr
	static auto apply(Integer, Integer, Integer, Integer, Func)
	noexcept { return true; }
};

template <
	size_t Cur,
	size_t End,
	size_t Factor,
	bool HasRem,
	class Dir,
	bool Noexcept
>
struct unroll_helper<
	Cur, End, Factor,
	contiguous<Factor, HasRem>,
	Dir, Noexcept
>
{
	static constexpr auto next_index =
	std::is_same<Dir, forward>::value ?
	Cur + 1 : Cur - 1;

	using policy = contiguous<Factor, HasRem>;
	using next = unroll_helper<
		next_index, End, Factor,
		policy, Dir, Noexcept
	>;

	template <class Integer, class Func>
	CC_ALWAYS_INLINE
	static auto apply(
		const Integer& off,
		const Integer& idx,
		const Integer& len,
		const Integer& stride,
		const Func& f
	) noexcept(Noexcept)
	{
		if (!f(off + Integer(Factor) * idx + Integer(Cur) * stride))
			return false;
		return next::apply(off, idx, len, stride, f);
	}
};

template <
	size_t Cur,
	size_t End,
	size_t Factor,
	bool HasRem,
	class Dir,
	bool Noexcept
>
struct unroll_helper<
	Cur, End, Factor,
	split<Factor, HasRem>,
	Dir, Noexcept
>
{
	static constexpr auto next_index =
	std::is_same<Dir, forward>::value ?
	Cur + 1 : Cur - 1;

	using policy = contiguous<Factor, HasRem>;
	using next = unroll_helper<
		next_index, End, Factor,
		policy, Dir, Noexcept
	>;

	template <class Integer, class Func>
	CC_ALWAYS_INLINE
	static auto apply(
		const Integer& off,
		const Integer& idx,
		const Integer& len,
		const Integer& stride,
		const Func& f
	) noexcept(Noexcept)
	{
		if (!f(off + idx + Integer(Cur) * len))
			return false;
		return next::apply(off, idx, len, stride, f);
	}
};

/*
** Used to unroll the remainder loops when the extents are accessible during
** compile-time.
*/
template <class Dir, bool Noexcept>
struct remainder_loop_helper
{
	using dir_h = direction_helper<Dir>;

	template <class L1, class L2, class L3, class Func,
	nd_enable_if(
		L1::allows_static_access &&
		L2::allows_static_access &&
		L3::allows_static_access
	)>
	CC_ALWAYS_INLINE
	static auto try_unroll(L1, L2, L3, const Func& f)
	noexcept(Noexcept)
	{
		static constexpr auto start =
		std::is_same<Dir, forward>::value ?
		L1::value() : L2::value();

		static constexpr auto end =
		std::is_same<Dir, forward>::value ?
		L2::value() : L1::value();

		static constexpr auto stride = L3::value();
		static constexpr auto len = end - start + stride;
		static constexpr auto unroll_fac = len / stride;

		using unroll_policy = contiguous<unroll_fac, false>;
		using unroll_helper = unroll_helper<
			dir_h::start(size_t{0}, unroll_fac - 1),
			dir_h::finish(size_t{0}, unroll_fac - 1, unroll_fac),
			unroll_fac, unroll_policy, Dir, Noexcept
		>;
		return unroll_helper::apply(0, start, stride, stride, f);
	}

	template <class L1, class L2, class L3, class Func,
	nd_enable_if(!(
		L1::allows_static_access &&
		L2::allows_static_access &&
		L3::allows_static_access
	))>
	CC_ALWAYS_INLINE
	static auto try_unroll(const L1& l1, const L2& l2, const L3& l3, const Func& f)
	noexcept(Noexcept)
	{
		for (auto i = l1(); i != l2(); dir_h::step(i, l3())) {
			if (!f(i)) return false;
		}
		return true;
	}
};

/*
** This class performs the bulk of the loop optimization work: it addresses all
** of the possibilities for the kinds of loops that can arise as a result of the
** interactions among the different loop optimizations.
*/
template <size_t Coord, class Dir, class UnrollPolicy, class TilePolicy, bool Noexcept>
struct tile_helper
{
	static constexpr auto unroll_fac = UnrollPolicy::factor;
	static constexpr auto tile_fac   = TilePolicy::factor;
	static constexpr auto unroll_rem = UnrollPolicy::has_rem;
	static constexpr auto tile_rem   = TilePolicy::has_rem;
	static constexpr auto n          = sc_coord<Coord>;

	using dir_h = direction_helper<Dir>;
	using rem_loop_h = remainder_loop_helper<Dir, Noexcept>;
	using tile_count_unroller = unroll_helper<
		dir_h::start(size_t{0}, unroll_fac - 1),
		dir_h::finish(size_t{0}, unroll_fac - 1, unroll_fac),
		unroll_fac, UnrollPolicy, Dir, Noexcept
	>;
	using tile_unroller = unroll_helper<
		dir_h::start(size_t{0}, tile_fac - 1),
		dir_h::finish(size_t{0}, tile_fac - 1, tile_fac),
		tile_fac, contiguous<tile_fac, true>, Dir, Noexcept
	>;

	template <class Range, class Func, nd_enable_if(
		tile_fac == 1 && unroll_fac == 1
	)>
	CC_ALWAYS_INLINE
	static auto apply(const Range& r, const Func& f)
	noexcept(Noexcept)
	{
		for (
			auto i = dir_h::start(r.start(n), r.finish(n));
			i != dir_h::finish(r.start(n), r.finish(n), r.length(n));
			dir_h::step(i, r.stride(n))
		) { if (!f(i)) return false; }
		return true;
	}

	template <class Range, class Func, nd_enable_if((
		std::is_same<Dir, forward>::value &&
		tile_fac != 1
	))>
	CC_ALWAYS_INLINE
	static auto apply(const Range& r, const Func& f)
	noexcept(Noexcept)
	{
		using integer = typename Range::integer;
		static constexpr auto tf_coord = basic_sc_coord<integer, tile_fac>;
		static constexpr auto uf_coord = basic_sc_coord<integer, unroll_fac>;

		validate(r);

		/*
		** Unrolled loop over tile numbers.
		*/
		for (
			auto i = integer{0};
			i != r.length(n) / (r.stride(n) * tf_coord() * uf_coord());
			++i
		)
		{
			/*
			** Unroll the loop over the chunk of `unroll_fac` tiles.
			*/
			if (!tile_count_unroller::apply(
				0, i,
				r.length(n) / (r.stride(n) * tf_coord() * uf_coord()), 1,
				/*
				** Unroll the loop over each tile.
				*/
				[&] (const auto& j) CC_ALWAYS_INLINE noexcept(Noexcept) {
					tile_unroller::apply(
						r.start(n), j, r.stride(n),
						r.stride(n), f
					);
				}
			)) { return false; }
		}

		if (unroll_rem) {
			/*
			** Loop over the remaining tiles that we could not
			** unroll.
			*/
			if (!rem_loop_h::try_unroll(
				uf_coord * (r.length_c(n) / (r.stride_c(n) * tf_coord * uf_coord)),
				r.length_c(n) / (r.stride_c(n) * tf_coord),
				basic_sc_coord<integer, 1>,
				[&] (const auto& i) CC_ALWAYS_INLINE noexcept(Noexcept) {
					tile_unroller::apply(
						r.start(n), i, r.stride(n),
						r.stride(n), f
					);
				}
			)) { return false; }
		}

		if (tile_rem) {
			/*
			** Loop over the remaining partial tile. The length of
			** the partial tile must always be divisible by the
			** stride, because the tile factor is in terms of stride
			** units.
			*/
			if (!rem_loop_h::try_unroll(
				r.start_c(n) + r.stride_c(n) * tf_coord *
				(r.length_c(n) / (r.stride_c(n) * tf_coord)),
				r.finish_c(n), r.stride_c(n), f
			)) { return false; }
		}

		return true;
	}

	template <class Range, class Func, nd_enable_if((
		std::is_same<Dir, backward>::value &&
		tile_fac != 1
	))>
	CC_ALWAYS_INLINE
	static auto apply(const Range& r, const Func& f)
	noexcept(Noexcept)
	{
		using integer = typename Range::integer;
		static constexpr auto tf_coord = basic_sc_coord<integer, tile_fac>;
		static constexpr auto uf_coord = basic_sc_coord<integer, unroll_fac>;

		validate(r);

		/*
		** Unrolled loop over tile numbers.
		*/
		for (
			auto i = r.length(n) / (r.stride(n) * tile_fac) - 1;
			i != r.length(n) / (r.stride(n) * tf_coord() * uf_coord());
			--i
		)
		{
			/*
			** Unroll the loop over the chunk of `unroll_fac` tiles.
			*/
			if (!tile_count_unroller::apply(
				0, i, r.length(n) / (tf_coord() * uf_coord()), 1,
				/*
				** Unroll the loop over each tile.
				*/
				[&] (const auto& j) CC_ALWAYS_INLINE noexcept(Noexcept) {
					tile_unroller::apply(
						r.start(n) + r.length(n) %
						(r.stride(n) * tf_coord()), j,
						r.stride(n), r.stride(n), f
					);
				}
			)) { return false; }
		}

		if (unroll_rem) {
			/*
			** Loop over the remaining tiles that we could not
			** unroll.
			*/
			if (!rem_loop_h::try_unroll(
				r.length_c(n) / (r.stride_c(n) * tf_coord * uf_coord),
				basic_sc_coord<integer, integer(-1)>,
				basic_sc_coord<integer, 1>,
				[&] (const auto& i) CC_ALWAYS_INLINE noexcept(Noexcept) {
					tile_unroller::apply(
						r.start(n) + r.length(n) %
						(r.stride(n) * tf_coord()),
						i, r.stride(n), r.stride(n), f
					);
				}
			)) { return false; }
		}

		if (tile_rem) {
			/*
			** Loop over the remaining partial tile. The length of
			** the partial tile must always be divisible by the
			** stride, because the tile factor is in terms of stride
			** units.
			*/
			if (!rem_loop_h::try_unroll(
				r.start_c(n) + r.length_c(n) % (r.stride_c(n) * tf_coord),
				r.start_c(n) - r.stride_c(n), r.stride_c(n), f
			)) { return false; }
		}

		return true;
	}
private:
	template <class Range>
	CC_ALWAYS_INLINE
	static void validate(const Range& r) noexcept
	{
		nd_assert(
			(!unroll_rem &&
			(r.length(n) / (r.stride(n) * tile_fac)) % unroll_fac != 0),
			"unrolling requires remainder loop, but remainder option "
			"is set to false.\n▶ Number of tiles is $; unroll factor "
			"is $; but $1 % $2 != 0",
			r.length(n) / (r.stride(n) * tile_fac), unroll_fac
		);

		nd_assert(
			(!tile_rem && r.length(n) % (r.stride(n) * tile_fac) != 0),
			"tiling requires remainder loop, but remainder option "
			"is set to false.\n▶ Length of range is $ - $ + $ = $; "
			"length of tile is $ * $ = $; but $4 % $7 != 0",
			r.finish(n), r.start(n), r.stride(n), r.length(n),
			r.stride(n), tile_fac, r.stride(n) * r.tile_fac
		);
	}
};

/*
** This helper struct is responsible for the following:
** - Converting the unroll factor `full_unroll_policy` to a positive integer
** when the range is statically accessible, or triggering a static assertion
** otherwise.
** - Setting the remainders for the unroll and tile policy to the correct values
** when the range is statically accessible along the given coordinate. This
** allows us to avoid generating remainder loops when we can determine that they
** are not necessary.
*/
template <
	size_t Coord,
	class UnrollPolicy,
	class TilePolicy,
	class Range,
	bool AllowsStaticAccess
>
struct policy_traits;

template <size_t Coord, class UnrollPolicy, class TilePolicy, class Range>
struct policy_traits<Coord, UnrollPolicy, TilePolicy, Range, true>
{
	static constexpr auto unroll_fac = UnrollPolicy::factor;
	static constexpr auto tile_fac   = TilePolicy::factor;
	static constexpr auto tile_size  = stride<Coord, Range> * tile_fac;
	static constexpr auto tile_count = length<Coord, Range> / tile_size;

	static constexpr auto checked_unroll_fac =
	UnrollPolicy::factor == full_unroll ? tile_count : unroll_fac;

	/*
	** Compute the remainders in case we can eliminate a remainder loop.
	*/
	static constexpr auto unroll_rem     = tile_count % checked_unroll_fac;
	static constexpr auto tile_rem       = length<Coord, Range> % tile_size;
	static constexpr auto has_unroll_rem = !unroll_rem;
	static constexpr auto has_tile_rem   = !tile_rem;

	using checked_unroll_policy =
	modify_policy<unroll_fac, has_unroll_rem, UnrollPolicy>;

	using checked_tile_policy =
	modify_policy<tile_fac, has_tile_rem, TilePolicy>;
};

template <size_t Coord, class UnrollPolicy, class TilePolicy, class Range>
struct policy_traits<Coord, UnrollPolicy, TilePolicy, Range, false>
{
	static_assert(
		UnrollPolicy::factor != full_unroll,
		"Range not statically accessible: full unroll impossible."
	);

	using checked_unroll_policy = UnrollPolicy;
	using checked_tile_policy = TilePolicy;
};

template <size_t Dim, size_t Dims, class Attribs, bool Noexcept>
struct evaluate_loop_helper
{
	using attrib        = mpl::at_c<Dim, Attribs>;
	using dir           = typename attrib::dir;
	using unroll_policy = typename attrib::unroll_policy;
	using tile_policy   = typename attrib::tile_policy;
	static constexpr auto coord = attrib::coord;

	template <class Range, class Func>
	CC_ALWAYS_INLINE
	static auto apply(const Range& r, const Func& f)
	noexcept(Noexcept)
	{
		using traits = policy_traits<
			coord, unroll_policy, tile_policy,
			Range, Range::template dim_allows_static_access<coord>()
		>;

		using checked_unroll_policy =
		typename traits::checked_unroll_policy;

		using checked_tile_policy =
		typename traits::checked_tile_policy;

		using helper = tile_helper<
			coord, dir,
			checked_unroll_policy,
			checked_tile_policy,
			Noexcept
		>;
		return helper::apply(r, f);
	}
};

template <size_t Dim, size_t Dims, class Attribs, bool Noexcept>
struct evaluator
{
	using next = evaluator<Dim + 1, Dims, Attribs, Noexcept>;

	template <class Range, class Func, class... Args>
	CC_ALWAYS_INLINE
	static auto apply(const Range& r, const Func& f, const Args&... args)
	noexcept(Noexcept)
	{
		using evaluator = evaluate_loop_helper<
			Dim, Dims, Attribs, Noexcept
		>;
		return evaluator::apply(r, [&] (const auto& arg)
			CC_ALWAYS_INLINE noexcept(Noexcept) {
				return next::apply(r, f, args..., arg);
			});
	}
};

template <size_t Dims, class Attribs, bool Noexcept>
struct evaluator<Dims, Dims, Attribs, Noexcept>
{
	template <class Range, class Func, class... Args>
	CC_ALWAYS_INLINE
	static auto apply(const Range&, const Func& f, const Args&... args)
	noexcept(Noexcept) { return f(c_index(args...)); }
};

}}}

#endif
