/*
** File Name: fusion.hpp
** Author:    Aditya Ramesh
** Date:      08/18/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z3CEC0357_F6F2_442A_9571_33BFEA51A386
#define Z3CEC0357_F6F2_442A_9571_33BFEA51A386

#include <ndmath/utility/tuple.hpp>

namespace nd {

/*
** Fusion utilities for `tuple` objects.
*/

namespace detail {
namespace tuple {

using nd::tuple;

template <class List>
struct fusion_helper;

template <size_t... Ts>
struct fusion_helper<std::index_sequence<Ts...>>
{
	template <class Tuple, class Func>
	CC_ALWAYS_INLINE constexpr
	static decltype(auto) expand(Tuple& t, const Func& f)
	noexcept { return f(get<Ts>(t)...); }

	template <class Tuple, class Func1, class Func2>
	CC_ALWAYS_INLINE constexpr
	static decltype(auto)
	expand_with(Tuple& t, const Func1& f1, const Func2& f2)
	noexcept { return f2(f1(get<Ts>(t))...); }

	template <class Tuple, class Func>
	CC_ALWAYS_INLINE constexpr
	static auto for_each(Tuple& t, const Func& f)
	noexcept
	{
		using expander = int[];
		(void)expander{0, ((void)f(get<Ts>(t)), 0)...};
	}

	template <class U, class... Us>
	CC_ALWAYS_INLINE constexpr
	static auto erase_front(const tuple<U, Us...>& t)
	noexcept { return make_tuple(get<Ts>(t)...); }

	template <class... Us>
	CC_ALWAYS_INLINE constexpr
	static auto erase_back(const tuple<Us...>& t)
	noexcept { return make_tuple(get<Us>(t)...); }
};

}}

/*
** TODO: The noexcept signatures for all of these functions should be weaker.
*/

template <class... Ts, class Func>
CC_ALWAYS_INLINE constexpr
decltype(auto) expand(tuple<Ts...>& t, const Func& f)
noexcept
{
	using list   = std::make_index_sequence<sizeof...(Ts)>;
	using helper = detail::tuple::fusion_helper<list>;
	return helper::expand(t, f);
}

template <class... Ts, class Func>
CC_ALWAYS_INLINE constexpr
decltype(auto) expand(const tuple<Ts...>& t, const Func& f)
noexcept
{
	using list   = std::make_index_sequence<sizeof...(Ts)>;
	using helper = detail::tuple::fusion_helper<list>;
	return helper::expand(t, f);
}

template <class... Ts, class Func1, class Func2>
CC_ALWAYS_INLINE constexpr
decltype(auto)
expand_with(tuple<Ts...>& t, const Func1& f1, const Func2& f2)
noexcept
{
	using list   = std::make_index_sequence<sizeof...(Ts)>;
	using helper = detail::tuple::fusion_helper<list>;
	return helper::expand_with(t, f1, f2);
}

template <class... Ts, class Func1, class Func2>
CC_ALWAYS_INLINE constexpr
decltype(auto)
expand_with(const tuple<Ts...>& t, const Func1& f1, const Func2& f2)
noexcept
{
	using list   = std::make_index_sequence<sizeof...(Ts)>;
	using helper = detail::tuple::fusion_helper<list>;
	return helper::expand_with(t, f1, f2);
}

template <class... Ts, class Func>
CC_ALWAYS_INLINE constexpr
auto for_each(tuple<Ts...>& t, const Func& f)
noexcept
{
	using list   = std::make_index_sequence<sizeof...(Ts)>;
	using helper = detail::tuple::fusion_helper<list>;
	return helper::for_each(t, f);
}

template <class... Ts, class Func>
CC_ALWAYS_INLINE constexpr
auto for_each(const tuple<Ts...>& t, const Func& f)
noexcept
{
	using list   = std::make_index_sequence<sizeof...(Ts)>;
	using helper = detail::tuple::fusion_helper<list>;
	return helper::for_each(t, f);
}

template <class... Ts, nd_enable_if((sizeof...(Ts) != 1))>
CC_ALWAYS_INLINE constexpr
auto erase_front(const tuple<Ts...>& ts)
noexcept
{
	using list   = mpl::to_values<mpl::range_c<size_t, 0, sizeof...(Ts) - 2>>;
	using helper = detail::tuple::fusion_helper<list>;
	return helper::erase_front(ts);
}

template <class... Ts, nd_enable_if((sizeof...(Ts) != 1))>
CC_ALWAYS_INLINE constexpr
auto erase_back(const tuple<Ts...>& ts)
noexcept
{
	using list   = mpl::to_values<mpl::range_c<size_t, 1, sizeof...(Ts) - 1>>;
	using helper = detail::tuple::fusion_helper<list>;
	return helper::erase_back(ts);
}

/*
** Fusion utilities for variadic parameter packs.
*/

namespace detail {
namespace variadic {

template <size_t Cur, size_t Last>
struct fusion_helper
{
	template <class T, class... Ts>
	CC_ALWAYS_INLINE constexpr
	static decltype(auto) at(T, Ts&&... ts) noexcept
	{
		using helper = fusion_helper<Cur + 1, Last>;
		return helper::at(std::forward<Ts>(ts)...);
	}
};

template <size_t Last>
struct fusion_helper<Last, Last>
{
	template <class T, class... Ts>
	CC_ALWAYS_INLINE constexpr
	static decltype(auto) at(T&& t, Ts...) noexcept
	{ return std::forward<T>(t); }
};

}}

template <size_t N, class... Ts, nd_enable_if((sizeof...(Ts) != 0))>
CC_ALWAYS_INLINE constexpr
decltype(auto) at(Ts&&... ts) noexcept
{
	static_assert(N < sizeof...(Ts), "Index out of bounds.");
	using helper = detail::variadic::fusion_helper<0, N>;
	return helper::at(std::forward<Ts>(ts)...);
}

template <class... Ts, nd_enable_if((sizeof...(Ts) != 0))>
CC_ALWAYS_INLINE constexpr
decltype(auto) front(Ts&&... ts) noexcept
{ return at<0>(std::forward<Ts>(ts)...); }

template <class... Ts, nd_enable_if((sizeof...(Ts) != 0))>
CC_ALWAYS_INLINE constexpr
decltype(auto) back(Ts&&... ts) noexcept
{ return at<sizeof...(Ts) - 1>(std::forward<Ts>(ts)...); }

namespace detail {
namespace variadic {

template <class List>
struct invoke_with_helper;

template <size_t... Ts>
struct invoke_with_helper<std::index_sequence<Ts...>>
{
	template <class Func, class... Us>
	CC_ALWAYS_INLINE constexpr
	static decltype(auto) apply(const Func& f, const Us&... us)
	noexcept { return f(at<Ts>(us...)...); }
};

}}

/*
** Invoke a function with selected elements from a variadic parameter pack.
*/
template <class List, class Func, class... Ts>
CC_ALWAYS_INLINE constexpr
decltype(auto) invoke_with(const Func& f, const Ts&... ts)
noexcept
{
	using helper = detail::variadic::invoke_with_helper<List>;
	return helper::apply(f, ts...);
}

}

#endif
