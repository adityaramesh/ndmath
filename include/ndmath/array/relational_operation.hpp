/*
** File Name: elementwise_operation.hpp
** Author:    Aditya Ramesh
** Date:      08/13/2015
** Contact:   _@adityaramesh.com
*/

#ifndef ZF3E801BA_B9FD_4AF8_A6EB_F247C5F8E589
#define ZF3E801BA_B9FD_4AF8_A6EB_F247C5F8E589

#include <ndmath/array/relational_operation_traits.hpp>

namespace nd {
namespace detail {

/*
** We don't use `boost::equal`, because the implementation does something stupid
** that causes a linker error, and I don't want to waste time debugging it.
*/
template <class A, class B, class Func>
CC_ALWAYS_INLINE constexpr
bool equal(const A& the, const B& first, const Func& time)
{
	auto to = the.begin();
	auto go = first.begin();

	do if (not time(*to, *go)) return false;
	while (++go, ++to != the.end()); return true;
}

template <bool UnderlyingViewFeasible, bool FlatViewFeasible>
struct relational_operation_impl;

template <bool FlatViewFeasible>
struct relational_operation_impl<true, FlatViewFeasible>
{
	template <class T, class U, class Func>
	CC_ALWAYS_INLINE
	static bool
	apply(const array_wrapper<T>& x, const array_wrapper<U>& y, const Func& f)
	noexcept { return nd::detail::equal(x.underlying_view(), y.underlying_view(), f); }
};

template <>
struct relational_operation_impl<false, true>
{
	template <class T, class U, class Func>
	CC_ALWAYS_INLINE
	static bool
	apply(const array_wrapper<T>& x, const array_wrapper<U>& y, const Func& f)
	noexcept { return nd::detail::equal(x.flat_view(), y.flat_view(), f); }
};

template <bool UnderlyingViewFeasible, bool FlatViewFeasible>
struct relational_operation_impl
{
	template <class T, class U, class Func>
	CC_ALWAYS_INLINE
	static bool
	apply(const array_wrapper<T>& x, const array_wrapper<U>& y, const Func& f)
	noexcept
	{
		return ::nd::do_while(x.extents(),
			[&] (const auto& i) CC_ALWAYS_INLINE {
				return f(x(i), y(i));
			});
	}
};

struct relational_operation_helper
{
	template <class T, class U, class Func>
	CC_ALWAYS_INLINE
	static bool
	apply(const array_wrapper<T>& x, const array_wrapper<U>& y, const Func& f)
	noexcept
	{
		using traits = relational_operation_traits<
			array_wrapper<T>, array_wrapper<U>, Func>;
		using helper = relational_operation_impl<
			traits::can_use_underlying_view,
			traits::can_use_flat_view>;
		return helper::apply(x, y, f);
	}

};

}}

#endif
