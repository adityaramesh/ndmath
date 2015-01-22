/*
** File Name: loop_optimization.hpp
** Author:    Aditya Ramesh
** Date:      01/21/2015
** Contact:   _@adityaramesh.com
*/

#ifndef ZCD72012D_4FBC_4CC8_AC50_77209FC0C952
#define ZCD72012D_4FBC_4CC8_AC50_77209FC0C952

namespace nd {
namespace detail {

/*
** Helps prevent combinatorial blowup in number of template specializations due
** to the chosen direction of traversal along each coordinate. This class can be
** used to write loops in a direction-independent way, as long as the length of
** the range traversed by the loop can be described using a relatively simple
** expression.
*/
template <class Direction>
struct direction_helper;

template <>
struct direction_helper<forward>
{
	template <class Integer>
	CC_ALWAYS_INLINE CC_CONST constexpr
	static auto start(const Integer& a, Integer) noexcept
	{ return a; }

	template <class Integer>
	CC_ALWAYS_INLINE CC_CONST constexpr
	static auto finish(const Integer& a, const Integer& b, const Integer& l) noexcept
	{ return a + l; }

	template <class Integer>
	CC_ALWAYS_INLINE
	static void step(Integer& c, const Integer& s) noexcept
	{ c += s; }
};

template <>
struct direction_helper<backward>
{
	template <class Integer>
	CC_ALWAYS_INLINE CC_CONST constexpr
	static auto start(Integer, const Integer& b) noexcept
	{ return b; }

	template <class Integer>
	CC_ALWAYS_INLINE CC_CONST constexpr
	static auto finish(Integer, const Integer& b, const Integer& l) noexcept
	{ return b - l; }

	template <class Integer>
	CC_ALWAYS_INLINE
	static void step(Integer& c, const Integer& s) noexcept
	{ c -= s; }
};

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
	static void apply(
		const Integer& o,
		const Integer& i,
		Integer, Integer,
		const Func& f
	)
	noexcept(Noexcept) { f(o + i); }
};

template <size_t End, size_t Factor, class Dir, bool Noexcept>
struct unroll_helper<End, End, Factor, Dir, Noexcept>
{
	template <class Integer, class Func>
	CC_ALWAYS_INLINE CC_CONST constexpr
	static void apply(Integer, Integer, Integer, Integer, Func)
	noexcept {}
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
	static void apply(
		const Integer& o,
		const Integer& i,
		const Integer& l,
		const Integer& s,
		const Func& f
	) noexcept(Noexcept)
	{
		f(o + Integer(Factor) * i + Integer(Cur) * s);
		next::apply(o, i, l, f);
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
	static void apply(
		const Integer& o,
		const Integer& i,
		const Integer& l,
		const Integer& s,
		const Func& f
	) noexcept(Noexcept)
	{
		f(o + i + Integer(Cur) * l);
		next::apply(o, i, l, f);
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
	static void try_unroll(L1, L2, L3, const Func& f)
	noexcept(Noexcept)
	{
		// The left endpoint.
		static constexpr auto a =
		std::is_same<Dir, forward>::value ?
		L1::value() : L2::value();

		// The right endpoint.
		static constexpr auto b =
		std::is_same<Dir, forward>::value ?
		L2::value() : L1::value();

		// The stride.
		static constexpr auto s = L3::value();
		static constexpr auto l = b - a + s;
		static constexpr auto unroll_fac = l / s;

		using unroll_policy = contiguous<unroll_fac, false>;
		using unroll_helper = unroll_helper<
			dir_h::start(size_t{0}, unroll_fac - 1, unroll_fac),
			dir_h::finish(size_t{0}, unroll_fac - 1, unroll_fac),
			unroll_fac, unroll_policy, Dir, Noexcept
		>;
		unroll_helper::apply(0, a, s, s, f);
	}

	template <class L1, class L2, class L3, class Func,
	nd_enable_if(!(
		L1::allows_static_access &&
		L2::allows_static_access &&
		L3::allows_static_access
	))>
	CC_ALWAYS_INLINE
	static void try_unroll(const L1& l1, const L2& l2, const L3& l3, const Func& f)
	noexcept(Noexcept)
	{
		for (auto i = l1.value(); i != l2.value(); dir_h::step(i, l3.value())) {
			f(i);
		}
	}
};

template <size_t C, class Dir, class UnrollPolicy, class TilePolicy, bool Noexcept>
struct tile_helper<C, Dir, UnrollPolicy, TilePolicy, Noexcept>
{
	static constexpr auto unroll_fac = UnrollPolicy::factor;
	static constexpr auto tile_fac   = TilePolicy::factor;
	static constexpr auto unroll_rem    = UnrollPolicy::unroll_rem;
	static constexpr auto tile_rem      = TilePolicy::tile_rem;

	using dir_h = direction_helper<Dir>;
	using rem_loop_h = remainder_loop_helper<Dir, Noexcept>;
	using tile_count_unroller = unroll_helper<
		dir_h::start(size_t{0}, unroll_fac - 1, unroll_fac),
		dir_h::finish(size_t{0}, unroll_fac - 1, unroll_fac),
		unroll_fac, UnrollPolicy, Dir, Noexcept
	>;
	using tile_unroller = unroll_helper<
		dir_h::start(size_t{0}, tile_fac - 1, tile_fac),
		dir_h::finish(size_t{0}, tile_fac - 1, tile_fac),
		tile_fac, contiguous<tile_fac, true>, Dir, Noexcept
	>;

	template <class Range, class Func, nd_enable_if(
		tile_fac == 1 && unroll_fac == 1
	)>
	CC_ALWAYS_INLINE
	static void apply(const Range& r, const Func& f)
	noexcept(Noexcept)
	{
		for (
			auto i = dir_h::start(r.start(C), r.finish(C), r.length(C));
			i != dir_h::finish(r.start(C), r.finish(C), r.length(C));
			dir_h::step(i, r.stride(C))
		) { f(i); }
	}

	template <class Range, class Func, nd_enable_if(
		std::is_same<Dir, forward>::value &&
		tile_fac != 1
	)>
	CC_ALWAYS_INLINE
	static void apply(const Range& r, const Func& f)
	noexcept(Noexcept)
	{
		using integer = typename Range::integer;
		using tf_loc = basic_cloc<integer, tile_fac>;
		using uf_loc = basic_cloc<integer, unroll_fac>;

		validate(r);

		/*
		** Unrolled loop over tile numbers.
		*/
		for (
			auto i = integer{0};
			i != r.length(C) / (r.stride(C) * tf_loc * uf_loc);
			++i
		)
		{
			/*
			** Unroll the loop over the chunk of `unroll_fac` tiles.
			*/
			tile_count_unroller::apply(
				0, i,
				r.length(C) / (r.stride(C) * tf_loc * uf_loc), 1,
				/*
				** Unroll the loop over each tile.
				*/
				[&] (const auto& j) CC_ALWAYS_INLINE noexcept(Noexcept) {
					tile_unroller::apply(
						r.start(C), j, r.stride(C), r.stride(C), f
					);
				}
			);
		}

		if (unroll_rem) {
			/*
			** Loop over the remaining tiles that we could not
			** unroll.
			*/
			rem_loop_h::try_unroll(
				uf_loc * (length<C>(r) / (stride<C>(r) * tf_loc * uf_loc)),
				length<C>(r) / (stride<C>(r) * tf_loc),
				basic_cloc<integer, 1>,
				[&] (const auto& i) CC_ALWAYS_INLINE noexcept(Noexcept) {
					tile_unroller::apply(
						r.start(C), i, r.stride(C), r.stride(C), f
					);
				}
			);
		}

		if (tile_rem) {
			/*
			** Loop over the remaining partial tile. The length of
			** the partial tile must always be divisible by the
			** stride, because the tile factor is in terms of stride
			** units.
			*/
			rem_loop_h::try_unroll(
				start<C>(r) + stride<C>(r) * tile_fac *
				(length<C>(r) / (stride<C>(r) * tf_loc)),
				finish<C>(r), stride<C>(r), f
			);
		}
	}

	template <class Range, class Func, nd_enable_if(
		std::is_same<Dir, backward>::value &&
		tile_fac != 1
	)>
	CC_ALWAYS_INLINE
	static void apply(const Range& r, const Func& f)
	noexcept(Noexcept)
	{
		using integer = typename Range::integer;
		using tf_loc = basic_cloc<integer, tile_fac>;
		using uf_loc = basic_cloc<integer, unroll_fac>;

		validate(r);

		/*
		** Unrolled loop over tile numbers.
		*/
		for (
			auto i = r.length(C) / (r.stride(C) * tile_fac) - 1;
			i != r.length(C) / (r.stride(C) * tile_fac * unroll_fac);
			--i
		)
		{
			/*
			** Unroll the loop over the chunk of `unroll_fac` tiles.
			*/
			tile_count_unroller::apply(
				0, i, r.length(C) / (tile_fac * unroll_fac), 1,
				/*
				** Unroll the loop over each tile.
				*/
				[&] (const auto& j) CC_ALWAYS_INLINE noexcept(Noexcept) {
					tile_unroller::apply(
						r.start(C) +
						r.length(C) % (r.stride(C) * tile_fac),
						j, r.stride(C), r.stride(C), f
					);
				}
			);
		}

		if (unroll_rem) {
			/*
			** Loop over the remaining tiles that we could not
			** unroll.
			*/
			rem_loop_h::try_unroll(
				length<C>(r) / (stride<C>(r) * tf_loc * uf_loc),
				basic_cloc<integer, integer(-1)>,
				basic_cloc<integer, 1>,
				[&] (const auto& i) CC_ALWAYS_INLINE noexcept(Noexcept) {
					tile_unroller::apply(
						r.start(C) + r.length(C) %
						(r.stride(C) * tile_fac),
						i, r.stride(C), r.stride(C), f
					);
				}
			);
		}

		if (tile_rem) {
			/*
			** Loop over the remaining partial tile. The length of
			** the partial tile must always be divisible by the
			** stride, because the tile factor is in terms of stride
			** units.
			*/
			rem_loop_h::try_unroll(
				start<C>(r) + length<C>(r) % (stride<C>(r) * tf_loc),
				start<C>(r) - stride<C>(r), stride<C>(r), f
			);
		}
	}
private:
	template <class Range>
	CC_ALWAYS_INLINE
	static void validate(const Range& r)
	noexcept(Noexcept)
	{
		nd_assert(
			(!unroll_rem &&
			(r.length(C) / (r.stride(C) * tile_fac)) % unroll_fac != 0),
			"Unrolling requires remainder loop, but remainder option "
			"is set to false. Number of tiles is $; unroll factor is $; "
			"but $1 % $2 != 0.",
			r.length(C) / (r.stride(C) * tile_fac), unroll_fac
		);

		nd_assert(
			(!tile_rem && r.length(C) % (r.stride(C) * tf_loc) != 0),
			"Tiling requires remainder loop, but remainder option "
			"is set to false. Length of range is $ - $ + $ = $; "
			"length of tile is $ * $ = $; but $4 % $7 != 0.",
			r.finish(C), r.start(C), r.stride(C), r.length(C),
			r.stride(C), tile_fac, r.stride(C) * r.tile_fac
		);
	}
};

/*
** Number of specializations:
** - 2 directions
** - 3 types of unrolling (none, without guard, with guard)
** - 3 types of tiling (none, without guard, with guard)
** This leads to 18 total specializations.
*/
template <
	size_t Coord,
	class Dir,
	size_t UnrollFac,
	size_t TileFac,
	bool UnrollGuard,
	bool TileGuard,
	bool Noexcept
>
struct evaluate_loop_helper;

template <size_t C, bool N>
struct evaluate_loop_helper<C, forward, 1, 1, false, false, N>
{
	template <class Range, class Func>
	CC_ALWAYS_INLINE
	static void apply(const Range& r, const Func& f)
	noexcept(N)
	{
		for (auto i = r.base(C); i != r.extent(C); i += r.stride(C)) {
			f(i);
		}
	}
};

template <size_t C, size_t U, size_t T, bool N>
struct evaluate_loop_helper<C, forward, U, T, true, true, N>
{
	template <class Range, class Func>
	CC_ALWAYS_INLINE
	static void apply(const Range& r, const Func& f)
	noexcept(N)
	{
		for (auto i = r.base(C); i != r.extent(C); i += r.stride(C)) {
			f(i);
		}
	}
};

template <size_t Dim, size_t Dims, bool Noexcept>
struct evaluate_helper
{
	using next = evaluate_helper<Dim + 1, Dims, Noexcept>;

	template <class Range, class Func, class... Args>
	CC_ALWAYS_INLINE
	static void apply(const Range& r, const Func& f, const Args&... args)
	noexcept(Noexcept)
	{
		using attrs = typename Range::attributes;
		using attr = mpl::at_c<Dim, attrs>;
		using evaluator = evaluate_loop_helper<
			attr::coord,
			typename attr::dir,
			attr::unroll_factor,
			attr::tile_factor,
			attr::use_unroll_guard,
			attr::use_tile_guard,
			Noexcept
		>;
		evaluator::apply(r, [&] (const auto& arg)
		CC_ALWAYS_INLINE noexcept(Noexcept) {
			next::apply(r, f, args..., arg);
		});
	}
};

template <size_t Dims, bool Noexcept>
struct evaluate_helper<Dims, Dims, Noexcept>
{
	template <class Range, class Func, class... Args>
	CC_ALWAYS_INLINE
	static void apply(const Range&, const Func& f, const Args&... args)
	noexcept(Noexcept) { f(make_index(args...)); }
};

}}

#endif
