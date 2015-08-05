/*
** File Name: range.hpp
** Author:    Aditya Ramesh
** Date:      01/11/2015
** Contact:   _@adityaramesh.com
**
** Note: the reason that range iterators are not implemented right now is
** because simulating a for loop using an iterator results in a performance
** degradation. I could not get the generated code to be equivalent to what
** would be generated for an actual for loop. For this reason, I would
** discourage using range iterators even if they were implemented.
*/

#ifndef Z2FFF753A_E01F_4298_B56F_AFB95A4D87C5
#define Z2FFF753A_E01F_4298_B56F_AFB95A4D87C5

#include <ccbase/format.hpp>
#include <ccbase/platform.hpp>
#include <ndmath/index.hpp>
#include <ndmath/range/loop_attribute.hpp>
#include <ndmath/range/loop_transformation.hpp>
#include <ndmath/range/traversal.hpp>
//#include <ndmath/range/range_iterator.hpp>

namespace nd {

template <class Start, class Finish, class Stride, class Attribs>
class range;

namespace detail {

template <size_t Coord, class Range>
static constexpr auto start =
std::decay_t<decltype(
	std::declval<Range>().start().at_c(sc_coord<Coord>)
)>::value();

template <size_t Coord, class Range>
static constexpr auto finish =
std::decay_t<decltype(
	std::declval<Range>().finish().at_c(sc_coord<Coord>)
)>::value();

template <size_t Coord, class Range>
static constexpr auto stride =
std::decay_t<decltype(
	std::declval<Range>().strides().at_c(sc_coord<Coord>)
)>::value();

template <size_t Coord, class Range>
static constexpr auto length =
finish<Coord, Range> - start<Coord, Range> + stride<Coord, Range>;

}

template <
	class Start, class Finish, class Stride,
	class Attribs = default_attribs<Start::dims()>
>
class range final
{
public:
	CC_ALWAYS_INLINE constexpr
	static auto dims() noexcept { return Start::dims(); }

	template <size_t N>
	CC_ALWAYS_INLINE constexpr
	static auto dim_allows_static_access() noexcept
	{
		using a = std::decay_t<decltype(std::declval<Start>().at_c(sc_coord<N>))>;
		using b = std::decay_t<decltype(std::declval<Finish>().at_c(sc_coord<N>))>;
		using s = std::decay_t<decltype(std::declval<Stride>().at_c(sc_coord<N>))>;
		return a::allows_static_access &&
		       b::allows_static_access &&
		       s::allows_static_access;
	}

	using self           = range<Start, Finish, Stride, Attribs>;
	using integer        = typename Start::integer;
	/*
	using iterator       = range_iterator<self>;
	using const_iterator = iterator;
	*/

	static constexpr auto allows_static_access =
	Start::allows_static_access  &&
	Finish::allows_static_access &&
	Stride::allows_static_access;
private:
	Start m_start{};
	Finish m_finish{};
	Stride m_strides{};
public:
	CC_ALWAYS_INLINE constexpr
	explicit range() noexcept {}

	CC_ALWAYS_INLINE constexpr
	explicit range(
		const Start& start,
		const Finish& finish,
		const Stride& strides
	) noexcept :
		m_start{start},
		m_finish{finish},
		m_strides{strides}
	{
		nd_assert(
			finish >= start,
			"endpoints not greater than starting points: $ ≱ $",
			finish, start
		);
		nd_assert(
			((finish - start) % strides == sc_index_n<dims(), 0>),
			"lengths not multiples of strides: ($ - $) % $ ≠ 0",
			finish, start, strides
		);
	}

	template <class Start_, class Finish_, class Stride_, class Attribs_>
	CC_ALWAYS_INLINE
	auto& operator=(const range<Start_, Finish_, Stride_, Attribs_>& rhs)
	noexcept
	{
		m_start = rhs.start();
		m_finish = rhs.finish();
		m_strides = rhs.strides();
		return *this;
	}

	CC_ALWAYS_INLINE constexpr
	const auto& start() const noexcept
	{ return m_start; }

	CC_ALWAYS_INLINE constexpr
	const auto& finish() const noexcept
	{ return m_finish; }

	CC_ALWAYS_INLINE constexpr
	const auto& strides() const noexcept
	{ return m_strides; }

	CC_ALWAYS_INLINE constexpr
	auto size_c() const noexcept
	{
		return prod(
			(m_finish - m_start) / m_strides +
			sc_index_n<dims(), 1>
		);
	}

	CC_ALWAYS_INLINE constexpr
	auto size() const noexcept
	{ return size_c().value(); }

	/*
	** XXX: the accessors below strip the reference from the inferred return
	** type, due to the use of auto return types. Right now, I'm leaning
	** toward ranges being immutable, so this doesn't really matter. If we
	** want to add non-const overloads in the future, we should probably
	** make a traits class.
	*/

	template <class Coord>
	CC_ALWAYS_INLINE constexpr auto
	start_c(const coord_wrapper<Coord>& c) const noexcept
	{ return m_start.at_c(c); }

	template <class Coord>
	CC_ALWAYS_INLINE constexpr auto
	finish_c(const coord_wrapper<Coord>& c) const noexcept
	{ return m_finish.at_c(c); }

	template <class Coord>
	CC_ALWAYS_INLINE constexpr auto
	stride_c(const coord_wrapper<Coord>& c) const noexcept
	{ return m_strides.at_c(c); }

	template <class Coord>
	CC_ALWAYS_INLINE constexpr
	auto length_c(const coord_wrapper<Coord>& c) const noexcept
	{ return finish_c(c) - start_c(c) + stride_c(c); }

	template <class Coord>
	CC_ALWAYS_INLINE constexpr auto
	start(const coord_wrapper<Coord>& c) const noexcept
	{ return start_c(c).value(); }

	template <class Coord>
	CC_ALWAYS_INLINE constexpr auto
	finish(const coord_wrapper<Coord>& c) const noexcept
	{ return finish_c(c).value(); }

	template <class Coord>
	CC_ALWAYS_INLINE constexpr auto
	stride(const coord_wrapper<Coord>& c) const noexcept
	{ return stride_c(c).value(); }

	template <class Coord>
	CC_ALWAYS_INLINE constexpr
	auto length(const coord_wrapper<Coord>& c) const noexcept
	{ return length_c(c).value(); }

	template <size_t... Loops>
	CC_ALWAYS_INLINE constexpr
	auto reverse() const noexcept
	{
		using seq = std::index_sequence<Loops...>;
		using attribs = reverse_loops<mpl::to_types<seq>, Attribs>;
		using new_range = range<Start, Finish, Stride, attribs>;
		return new_range{m_start, m_finish, m_strides};
	}

	template <size_t... Loops>
	CC_ALWAYS_INLINE constexpr
	auto permute() const noexcept
	{
		using seq = std::index_sequence<Loops...>;
		using attribs = apply_perm<mpl::to_types<seq>, Attribs>;
		using new_range = range<Start, Finish, Stride, attribs>;
		return new_range{m_start, m_finish, m_strides};
	}

	template <size_t Loop, class Policy>
	CC_ALWAYS_INLINE constexpr
	auto unroll() const noexcept
	{
		using attribs = set_loop_unroll_policy<Loop, Policy, Attribs>;
		using new_range = range<Start, Finish, Stride, attribs>;
		return new_range{m_start, m_finish, m_strides};
	}

	template <size_t Loop, size_t N, bool HasRem = true>
	CC_ALWAYS_INLINE constexpr
	auto tile() const noexcept
	{
		using policy = tile_policy<N, HasRem>;
		using attribs = set_loop_tile_policy<Loop, policy, Attribs>;
		using new_range = range<Start, Finish, Stride, attribs>;
		return new_range{m_start, m_finish, m_strides};
	}

	template <class Func>
	CC_ALWAYS_INLINE void
	operator()(const Func& f) const
	nd_deduce_noexcept(for_each(*this, f))

	/*
	CC_ALWAYS_INLINE constexpr
	auto begin() const noexcept
	{ return iterator{*this}; }

	CC_ALWAYS_INLINE constexpr
	auto end() const noexcept
	{ return iterator{*this}; }
	*/
};

template <
	class Start1, class Start2,
	class Finish1, class Finish2,
	class Stride1, class Stride2,
	class Attribs1, class Attribs2
>
CC_ALWAYS_INLINE constexpr
auto operator==(
	const range<Start1, Finish1, Stride1, Attribs1>& lhs,
	const range<Start2, Finish2, Stride2, Attribs2>& rhs
) noexcept
{
	return lhs.start()   == rhs.start()  &&
	       lhs.finish()  == rhs.finish() &&
	       lhs.strides() == rhs.strides();
}

template <
	class Start1, class Start2,
	class Finish1, class Finish2,
	class Stride1, class Stride2,
	class Attribs1, class Attribs2
>
CC_ALWAYS_INLINE constexpr
auto operator!=(
	const range<Start1, Finish1, Stride1, Attribs1>& lhs,
	const range<Start2, Finish2, Stride2, Attribs2>& rhs
) noexcept { return !(lhs == rhs); }

/*
** TODO: uncomment once implemented.
**
** template <class Start, class Finish, class Stride, class Attribs>
** CC_ALWAYS_INLINE constexpr
** auto begin(const range<Start, Finish, Stride, Attribs>& r)
** noexcept { return r.begin(); }
** 
** template <class Start, class Finish, class Stride, class Attribs>
** CC_ALWAYS_INLINE constexpr
** auto end(const range<Start, Finish, Stride, Attribs>& r)
** noexcept { return r.end(); }
*/

template <
	class Char, class Traits, class Start,
	class Finish, class Stride, class Attribs
>
auto& operator<<(
	std::basic_ostream<Char, Traits>& os,
	const range<Start, Finish, Stride, Attribs>& r
) noexcept
{
	cc::write(os, "(start: $ • finish: $ • strides: $)",
		r.start(), r.finish(), r.strides());
	return os;
}

template <
	class Start,
	class Finish,
	class Stride,
	nd_enable_if((
		Start::dims() == Finish::dims() && 
		Finish::dims() == Stride::dims() &&
		std::is_same<
			typename Start::integer,
			typename Finish::integer
		>::value &&
		std::is_same<
			typename Finish::integer,
			typename Stride::integer
		>::value
	))
>
CC_ALWAYS_INLINE constexpr
auto make_range(const Start& b, const Finish& e, const Stride& s)
noexcept
{
	/*
	** Why is it important to invoke `eval` on the indices before creating
	** the range? If the indices are expressions (e.g. the result of
	** subtracting two indices), then instead of storing the result, we
	** would store the expression. Each expression stores copies of the two
	** indices involved in the corresponding arithmetic operation. (The
	** expression cannot store references, because it must be
	** default-constructible.) The end result is that storing the expression
	** may cause us to use many times more memory than is necessary to store
	** the result of evaluating the expression.
	*/
	using start  = decltype(eval(b));
	using finish = decltype(eval(e));
	using stride = decltype(eval(s));
	return range<start, finish, stride>{eval(b), eval(e), eval(s)};
}

template <
	class Start,
	class Finish,
	nd_enable_if((
		Start::dims() == Finish::dims() &&
		std::is_same<
			typename Start::integer,
			typename Finish::integer
		>::value
	))
>
CC_ALWAYS_INLINE constexpr
auto make_range(const Start& b, const Finish& e)
noexcept
{
	constexpr auto dims = Start::dims();
	using integer = typename Start::integer;
	return make_range(b, e, basic_sc_index_n<integer, dims, 1>);
}

template <class Finish>
CC_ALWAYS_INLINE constexpr
auto make_range(const Finish& e) noexcept
{
	constexpr auto dims = Finish::dims();
	using integer = typename Finish::integer;
	return make_range(basic_sc_index_n<integer, dims, 0>, e);
}

template <class Integer, Integer... Ts>
static constexpr auto basic_sc_range =
make_range(nd::basic_sc_index<Integer, Ts...>);

template <unsigned... Ts>
static constexpr auto sc_range =
basic_sc_range<unsigned, Ts...>;

template <class Integer, size_t Length, size_t Value>
static constexpr auto basic_sc_range_n =
make_range(nd::basic_sc_index_n<Integer, Length, Value>);

template <size_t Length, size_t Value>
static constexpr auto sc_range_n =
basic_sc_range_n<unsigned, Length, Value>;

template <unsigned... Ts>
static constexpr auto sc_extents =
basic_sc_range<unsigned, (Ts - 1)...>;

template <class... Ts>
CC_ALWAYS_INLINE constexpr
auto extents(const Ts... ts) noexcept
{
	return make_range(c_index(ts...) - sc_index_n<sizeof...(Ts), 1>);
}

}

#endif
