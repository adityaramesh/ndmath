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

template <
	class Start, class Finish, class Stride,
	class Attribs = default_attribs<Start::dims()>
>
class range final
{
public:
	static CC_ALWAYS_INLINE CC_CONST
	constexpr auto dims() noexcept { return Start::dims(); }

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
		nd_assert(
			((finish - start) % strides == cindex_n<dims(), 0>),
			"lengths not multiples of strides: ($ - $) % $ ≠ 0",
			finish, start, strides
		);
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

	template <class Integer, nd_enable_if(Start::allows_static_access)>
	CC_ALWAYS_INLINE CC_CONST constexpr
	static auto start(const Integer n) noexcept
	{ return Start::at(n); }

	template <class Integer, nd_enable_if(Finish::allows_static_access)>
	CC_ALWAYS_INLINE CC_CONST constexpr
	static auto finish(const Integer n) noexcept
	{ return Finish::at(n); }

	template <class Integer, nd_enable_if(Stride::allows_static_access)>
	CC_ALWAYS_INLINE CC_CONST constexpr
	static auto stride(const Integer n) noexcept
	{ return Stride::at(n); }

	template <class Integer, nd_enable_if(allows_static_access)>
	CC_ALWAYS_INLINE CC_CONST constexpr
	static auto length(const Integer n) noexcept
	{ return Finish::at(n) - Start::at(n) + Stride::at(n); }

	template <class Integer, nd_enable_if(!Start::allows_static_access)>
	CC_ALWAYS_INLINE constexpr auto
	start(const Integer n) const noexcept
	{ return m_start(n); }

	template <class Integer, nd_enable_if(!Finish::allows_static_access)>
	CC_ALWAYS_INLINE constexpr auto
	finish(const Integer n) const noexcept
	{ return m_finish(n); }

	template <class Integer, nd_enable_if(!Stride::allows_static_access)>
	CC_ALWAYS_INLINE constexpr auto
	stride(const Integer n) const noexcept
	{ return m_strides(n); }

	template <class Integer, nd_enable_if(!allows_static_access)>
	CC_ALWAYS_INLINE constexpr
	auto length(const Integer n) const noexcept
	{ return finish(n) - start(n) + stride(n); } 

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
	const noexcept(noexcept(f(cindex_n<Start::dims(), 0>)))
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
	size_t N, class Start, class Finish,
	class Stride, class Attribs,
	nd_enable_if(Start::allows_static_access)
>
CC_ALWAYS_INLINE CC_CONST constexpr
static auto start(const range<Start, Finish, Stride, Attribs>& r)
noexcept { return at<N>(r.start()); }

template <
	size_t N, class Start, class Finish,
	class Stride, class Attribs,
	nd_enable_if(Finish::allows_static_access)
>
CC_ALWAYS_INLINE CC_CONST constexpr
static auto finish(const range<Start, Finish, Stride, Attribs>& r)
noexcept { return at<N>(r.finish()); }

template <
	size_t N, class Start, class Finish,
	class Stride, class Attribs,
	nd_enable_if(Stride::allows_static_access)
>
CC_ALWAYS_INLINE CC_CONST constexpr
static auto strides(const range<Start, Finish, Stride, Attribs>& r)
noexcept { return at<N>(r.strides()); }

template <
	size_t N, class Start, class Finish,
	class Stride, class Attribs,
	nd_enable_if(!Start::allows_static_access)
>
CC_ALWAYS_INLINE constexpr
auto start(const range<Start, Finish, Stride, Attribs>& r)
noexcept { return at<N>(r.start()); }

template <
	size_t N, class Start, class Finish,
	class Stride, class Attribs,
	nd_enable_if(!Finish::allows_static_access)
>
CC_ALWAYS_INLINE constexpr
auto finish(const range<Start, Finish, Stride, Attribs>& r)
noexcept { return at<N>(r.finish()); }

template <
	size_t N, class Start, class Finish,
	class Stride, class Attribs,
	nd_enable_if(!Stride::allows_static_access)
>
CC_ALWAYS_INLINE constexpr
auto stride(const range<Start, Finish, Stride, Attribs>& r)
noexcept { return at<N>(r.strides()); }

template <
	size_t N, class Start, class Finish,
	class Stride, class Attribs,
	nd_enable_if((
		Start::allows_static_access  &&
		Finish::allows_static_access &&
		Stride::allows_static_access
	))
>
CC_ALWAYS_INLINE CC_CONST constexpr
static auto length(const range<Start, Finish, Stride, Attribs>& r)
noexcept { return finish<N>(r) - start<N>(r) + stride<N>(r); }

template <
	size_t N, class Start, class Finish,
	class Stride, class Attribs,
	nd_enable_if(!(
		Start::allows_static_access  &&
		Finish::allows_static_access &&
		Stride::allows_static_access
	))
>
CC_ALWAYS_INLINE constexpr
auto length(const range<Start, Finish, Stride, Attribs>& r)
noexcept { return finish<N>(r) - start<N>(r) + stride<N>(r); }

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
	return make_range(b, e, basic_cindex_n<integer, dims, 1>);
}

template <class Finish>
CC_ALWAYS_INLINE CC_CONST constexpr
auto make_range(const Finish& e) noexcept
{
	constexpr auto dims = Finish::dims();
	using integer = typename Finish::integer;
	return make_range(basic_cindex_n<integer, dims, 0>, e);
}

template <class Integer, Integer... Ts>
static constexpr auto basic_crange =
make_range(nd::basic_cindex<Integer, Ts...>);

template <uint_fast32_t... Ts>
static constexpr auto crange =
basic_crange<uint_fast32_t, Ts...>;

template <class Integer, size_t Length, size_t Value>
static constexpr auto basic_crange_n =
make_range(nd::basic_cindex_n<Integer, Length, Value>);

template <size_t Length, size_t Value>
static constexpr auto crange_n =
basic_crange_n<uint_fast32_t, Length, Value>;

}

#endif
