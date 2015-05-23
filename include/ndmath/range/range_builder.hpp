/*
** File Name: range_builder.hpp
** Author:    Aditya Ramesh
** Date:      01/30/2015
** Contact:   _@adityaramesh.com
**
** Convenient syntax for defining 1D ranges.
*/

#ifndef ZAD301DEC_3C71_4AE6_937F_3A77189CCD3C
#define ZAD301DEC_3C71_4AE6_937F_3A77189CCD3C

namespace nd {

class range_builder final
{
public:
	template <class A, class B>
	CC_ALWAYS_INLINE constexpr
	auto operator()(const A& a, const B& b)
	const noexcept
	{
		return make_range(index(a), index(b));
	}

	template <class A, class B, class S>
	CC_ALWAYS_INLINE constexpr
	auto operator()(const A& a, const B& b, const S& s)
	const noexcept
	{
		return make_range(index(a), index(b), index(s));
	}
};

class const_range_builder final
{
public:
	template <class A, class B>
	CC_ALWAYS_INLINE constexpr
	auto operator()(const A& a, const B& b)
	const noexcept
	{
		return make_range(c_index(a), c_index(b));
	}

	template <class A, class B, class S>
	CC_ALWAYS_INLINE constexpr
	auto operator()(const A& a, const B& b, const S& s)
	const noexcept
	{
		return make_range(c_index(a), c_index(b), c_index(s));
	}
};

namespace tokens {

static constexpr auto r = range_builder{};
static constexpr auto cr = const_range_builder{};

template <unsigned A, unsigned B, unsigned S = 1>
static constexpr auto scr = r(sc_coord<A>, sc_coord<B>, sc_coord<S>);

}

}

#endif
