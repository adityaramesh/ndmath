/*
** File Name: range.hpp
** Author:    Aditya Ramesh
** Date:      01/11/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z2FFF753A_E01F_4298_B56F_AFB95A4D87C5
#define Z2FFF753A_E01F_4298_B56F_AFB95A4D87C5

#include <ccbase/platform.hpp>
#include <ndmath/index.hpp>
#include <ndmath/range/attribute.hpp>
#include <ndmath/range/attribute_traits.hpp>
#include <ndmath/range/for_each.hpp>
//#include <ndmath/range/range_iterator.hpp>

namespace nd {

template <class Start, class Finish, class Stride, class Attribs>
class range;

namespace detail {

template <size_t Coord, class Range>
static constexpr auto start =
std::decay_t<decltype(
	std::declval<Range>().start().at_l(tokens::c<Coord>)
)>::value();

template <size_t Coord, class Range>
static constexpr auto finish =
std::decay_t<decltype(
	std::declval<Range>().finish().at_l(tokens::c<Coord>)
)>::value();

template <size_t Coord, class Range>
static constexpr auto stride =
std::decay_t<decltype(
	std::declval<Range>().strides().at_l(tokens::c<Coord>)
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
	CC_ALWAYS_INLINE CC_CONST constexpr
	static auto dims() noexcept { return Start::dims(); }

	template <size_t N>
	CC_ALWAYS_INLINE CC_CONST constexpr
	static auto allows_static_access() noexcept
	{
		using tokens::c;
		using a = std::decay_t<decltype(std::declval<Start>().at_l(c<N>))>;
		using b = std::decay_t<decltype(std::declval<Finish>().at_l(c<N>))>;
		using s = std::decay_t<decltype(std::declval<Stride>().at_l(c<N>))>;
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
private:
	const Start& m_start;
	const Finish& m_finish;
	const Stride& m_strides;
public:
	CC_ALWAYS_INLINE CC_CONST constexpr
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
			"end points not larger than starting points: $ ≱ $",
			finish, start
		);
		/*
		nd_assert(
			((finish - start) % strides == sc_index_n<dims(), 0>),
			"lengths not multiples of strides: ($ - $) % $ ≠ 0",
			finish, start, strides
		);
		*/
	}

	CC_ALWAYS_INLINE CC_CONST constexpr
	const auto& start() const noexcept
	{ return m_start; }

	CC_ALWAYS_INLINE CC_CONST constexpr
	const auto& finish() const noexcept
	{ return m_finish; }

	CC_ALWAYS_INLINE CC_CONST constexpr
	const auto& strides() const noexcept
	{ return m_strides; }

	/*
	** XXX: the accessors below strip the reference from the inferred return
	** type, due to the use of auto return types. Right now, I'm leaning
	** toward ranges being immutable, so this doesn't really matter. If we
	** want to add non-const overloads in the future, we should probably
	** make a traits class.
	*/

	template <class Coord>
	CC_ALWAYS_INLINE constexpr auto
	start_l(const coord_wrapper<Coord>& c) const noexcept
	{ return m_start.at_l(c); }

	template <class Coord>
	CC_ALWAYS_INLINE constexpr auto
	finish_l(const coord_wrapper<Coord>& c) const noexcept
	{ return m_finish.at_l(c); }

	template <class Coord>
	CC_ALWAYS_INLINE constexpr auto
	stride_l(const coord_wrapper<Coord>& c) const noexcept
	{ return m_strides.at_l(c); }

	template <class Coord>
	CC_ALWAYS_INLINE constexpr
	auto length_l(const coord_wrapper<Coord>& c) const noexcept
	{ return finish_l(c) - start_l(c) + stride_l(c); }

	template <class Coord>
	CC_ALWAYS_INLINE constexpr auto
	start(const coord_wrapper<Coord>& c) const noexcept
	{ return start_l(c).value(); }

	template <class Coord>
	CC_ALWAYS_INLINE constexpr auto
	finish(const coord_wrapper<Coord>& c) const noexcept
	{ return finish_l(c).value(); }

	template <class Coord>
	CC_ALWAYS_INLINE constexpr auto
	stride(const coord_wrapper<Coord>& c) const noexcept
	{ return stride_l(c).value(); }

	template <class Coord>
	CC_ALWAYS_INLINE constexpr
	auto length(const coord_wrapper<Coord>& c) const noexcept
	{ return length_l(c).value(); }

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
	operator()(const Func& f)
	const noexcept(noexcept(f(sc_index_n<dims(), 0>)))
	{ for_each(*this, f); }

	/*
	CC_ALWAYS_INLINE CC_CONST constexpr
	auto begin() const noexcept
	{ return iterator{*this}; }

	CC_ALWAYS_INLINE CC_CONST constexpr
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
template <class Start, class Finish, class Stride, class Attribs>
CC_ALWAYS_INLINE CC_CONST constexpr
auto begin(const range<Start, Finish, Stride, Attribs>& r)
noexcept { return r.begin(); }

template <class Start, class Finish, class Stride, class Attribs>
CC_ALWAYS_INLINE CC_CONST constexpr
auto end(const range<Start, Finish, Stride, Attribs>& r)
noexcept { return r.end(); }
*/

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
CC_ALWAYS_INLINE CC_CONST constexpr
auto make_range(const Start& b, const Finish& e, const Stride& s)
noexcept { return range<Start, Finish, Stride>{b, e, s}; }

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
CC_ALWAYS_INLINE CC_CONST constexpr
auto make_range(const Start& b, const Finish& e)
noexcept
{
	constexpr auto dims = Start::dims();
	using integer = typename Start::integer;
	return make_range(b, e, basic_sc_index_n<integer, dims, 1>);
}

template <class Finish>
CC_ALWAYS_INLINE CC_CONST constexpr
auto make_range(const Finish& e) noexcept
{
	constexpr auto dims = Finish::dims();
	using integer = typename Finish::integer;
	return make_range(basic_sc_index_n<integer, dims, 0>, e);
}

template <class Integer, Integer... Ts>
static constexpr auto basic_sc_range =
make_range(nd::basic_sc_index<Integer, Ts...>);

template <uint_fast32_t... Ts>
static constexpr auto sc_range =
basic_sc_range<uint_fast32_t, Ts...>;

template <class Integer, size_t Length, size_t Value>
static constexpr auto basic_sc_range_n =
make_range(nd::basic_sc_index_n<Integer, Length, Value>);

template <size_t Length, size_t Value>
static constexpr auto sc_range_n =
basic_sc_range_n<uint_fast32_t, Length, Value>;

}

#endif
