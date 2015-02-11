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
** Helps us to avoid doubling the number of template specializations to address
** the direction of traversal along each coordinate. This class can be used to
** write loops in a direction-independent way, as long as the length of the
** range traversed by the loop can be described using a relatively simple
** expression.
*/
template <class Direction>
struct direction_helper;

template <>
struct direction_helper<forward>
{
	template <class Integer>
	CC_ALWAYS_INLINE constexpr
	static auto start(const Integer& a, Integer) noexcept
	{ return a; }

	template <class Integer>
	CC_ALWAYS_INLINE constexpr
	static auto finish(const Integer& a, Integer, const Integer& l) noexcept
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
	CC_ALWAYS_INLINE constexpr
	static auto start(Integer, const Integer& b) noexcept
	{ return b; }

	template <class Integer>
	CC_ALWAYS_INLINE constexpr
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

template <size_t End, size_t Factor, class Policy, class Dir, bool Noexcept>
struct unroll_helper<End, End, Factor, Policy, Dir, Noexcept>
{
	template <class Integer, class Func>
	CC_ALWAYS_INLINE constexpr
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
		next::apply(o, i, l, s, f);
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
		next::apply(o, i, l, s, f);
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
			dir_h::start(size_t{0}, unroll_fac - 1),
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
		for (auto i = l1(); i != l2(); dir_h::step(i, l3())) {
			f(i);
		}
	}
};

/*
** This class performs the bulk of the loop optimization work: it addresses all
** of the possibilities for the kinds of loops that can arise as a result of the
** interaction among the different loop optimizations.
*/
template <size_t Coord, class Dir, class UnrollPolicy, class TilePolicy, bool Noexcept>
struct tile_helper
{
	static constexpr auto unroll_fac = UnrollPolicy::factor;
	static constexpr auto tile_fac   = TilePolicy::factor;
	static constexpr auto unroll_rem = UnrollPolicy::has_rem;
	static constexpr auto tile_rem   = TilePolicy::has_rem;
	static constexpr auto n          = tokens::c<Coord>;

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
	static void apply(const Range& r, const Func& f)
	noexcept(Noexcept)
	{
		for (
			auto i = dir_h::start(r.start(n), r.finish(n));
			i != dir_h::finish(r.start(n), r.finish(n), r.length(n));
			dir_h::step(i, r.stride(n))
		) { f(i); }
	}

	template <class Range, class Func, nd_enable_if((
		std::is_same<Dir, forward>::value &&
		tile_fac != 1
	))>
	CC_ALWAYS_INLINE
	static void apply(const Range& r, const Func& f)
	noexcept(Noexcept)
	{
		using integer = typename Range::integer;
		static constexpr auto tf_loc = basic_sc_coord<integer, tile_fac>;
		static constexpr auto uf_loc = basic_sc_coord<integer, unroll_fac>;

		validate(r);

		/*
		** Unrolled loop over tile numbers.
		*/
		for (
			auto i = integer{0};
			i != r.length(n) / (r.stride(n) * tf_loc() * uf_loc());
			++i
		)
		{
			/*
			** Unroll the loop over the chunk of `unroll_fac` tiles.
			*/
			tile_count_unroller::apply(
				0, i,
				r.length(n) / (r.stride(n) * tf_loc() * uf_loc()), 1,
				/*
				** Unroll the loop over each tile.
				*/
				[&] (const auto& j) CC_ALWAYS_INLINE noexcept(Noexcept) {
					tile_unroller::apply(
						r.start(n), j, r.stride(n),
						r.stride(n), f
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
				uf_loc * (r.length_l(n) / (r.stride_l(n) * tf_loc * uf_loc)),
				r.length_l(n) / (r.stride_l(n) * tf_loc),
				basic_sc_coord<integer, 1>,
				[&] (const auto& i) CC_ALWAYS_INLINE noexcept(Noexcept) {
					tile_unroller::apply(
						r.start(n), i, r.stride(n),
						r.stride(n), f
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
				r.start_l(n) + r.stride_l(n) * tf_loc *
				(r.length_l(n) / (r.stride_l(n) * tf_loc)),
				r.finish_l(n), r.stride_l(n), f
			);
		}
	}

	template <class Range, class Func, nd_enable_if((
		std::is_same<Dir, backward>::value &&
		tile_fac != 1
	))>
	CC_ALWAYS_INLINE
	static void apply(const Range& r, const Func& f)
	noexcept(Noexcept)
	{
		using integer = typename Range::integer;
		static constexpr auto tf_loc = basic_sc_coord<integer, tile_fac>;
		static constexpr auto uf_loc = basic_sc_coord<integer, unroll_fac>;

		validate(r);

		/*
		** Unrolled loop over tile numbers.
		*/
		for (
			auto i = r.length(n) / (r.stride(n) * tile_fac) - 1;
			i != r.length(n) / (r.stride(n) * tf_loc() * uf_loc());
			--i
		)
		{
			/*
			** Unroll the loop over the chunk of `unroll_fac` tiles.
			*/
			tile_count_unroller::apply(
				0, i, r.length(n) / (tf_loc() * uf_loc()), 1,
				/*
				** Unroll the loop over each tile.
				*/
				[&] (const auto& j) CC_ALWAYS_INLINE noexcept(Noexcept) {
					tile_unroller::apply(
						r.start(n) + r.length(n) %
						(r.stride(n) * tf_loc()), j,
						r.stride(n), r.stride(n), f
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
				r.length_l(n) / (r.stride_l(n) * tf_loc * uf_loc),
				basic_sc_coord<integer, integer(-1)>,
				basic_sc_coord<integer, 1>,
				[&] (const auto& i) CC_ALWAYS_INLINE noexcept(Noexcept) {
					tile_unroller::apply(
						r.start(n) + r.length(n) %
						(r.stride(n) * tf_loc()),
						i, r.stride(n), r.stride(n), f
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
				r.start_l(n) + r.length_l(n) % (r.stride_l(n) * tf_loc),
				r.start_l(n) - r.stride_l(n), r.stride_l(n), f
			);
		}
	}
private:
	template <class Range>
	CC_ALWAYS_INLINE
	static void validate(const Range& r) noexcept
	{
		nd_assert(
			(!unroll_rem &&
			(r.length(n) / (r.stride(n) * tile_fac)) % unroll_fac != 0),
			"Unrolling requires remainder loop, but remainder option "
			"is set to false. Number of tiles is $; unroll factor is $; "
			"but $1 % $2 != 0.",
			r.length(n) / (r.stride(n) * tile_fac), unroll_fac
		);

		nd_assert(
			(!tile_rem && r.length(n) % (r.stride(n) * tile_fac) != 0),
			"Tiling requires remainder loop, but remainder option "
			"is set to false. Length of range is $ - $ + $ = $; "
			"length of tile is $ * $ = $; but $4 % $7 != 0.",
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
	static void apply(const Range& r, const Func& f)
	noexcept(Noexcept)
	{
		using traits = policy_traits<
			coord, unroll_policy, tile_policy,
			Range, Range::template allows_static_access<coord>()
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
		helper::apply(r, f);
	}
};

template <size_t Dim, size_t Dims, class Attribs, bool Noexcept>
struct evaluate_helper
{
	using next = evaluate_helper<Dim + 1, Dims, Attribs, Noexcept>;

	template <class Range, class Func, class... Args>
	CC_ALWAYS_INLINE
	static void apply(const Range& r, const Func& f, const Args&... args)
	noexcept(Noexcept)
	{
		using evaluator = evaluate_loop_helper<
			Dim, Dims, Attribs, Noexcept
		>;
		evaluator::apply(r, [&] (const auto& arg)
			CC_ALWAYS_INLINE noexcept(Noexcept) {
				next::apply(r, f, args..., arg);
			});
	}
};

template <size_t Dims, class Attribs, bool Noexcept>
struct evaluate_helper<Dims, Dims, Attribs, Noexcept>
{
	template <class Range, class Func, class... Args>
	CC_ALWAYS_INLINE
	static void apply(const Range&, const Func& f, const Args&... args)
	noexcept(Noexcept) { f(make_index(args...)); }
};

}}

#endif
