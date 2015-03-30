/*
** File Name: array_assignment.hpp
** Author:    Aditya Ramesh
** Date:      03/16/2015
** Contact:   _@adityaramesh.com
**
** TODO: Noexcept specifications. These are irritating to write, and it's not
** clear to me how they would be beneficial for assignment operators.
*/

#ifndef Z9A5D0442_8832_4E17_ADF8_1BA2DC724D52
#define Z9A5D0442_8832_4E17_ADF8_1BA2DC724D52

#include <ndmath/array/array_assignment_traits.hpp>
#include <boost/range/algorithm/copy.hpp>

namespace nd {

template <class T>
class array_wrapper;

namespace detail {

template <bool IsResizable>
struct resize_helper;

template <>
struct resize_helper<true>
{
	template <class T, class U>
	CC_ALWAYS_INLINE
	static void
	apply(array_wrapper<T>& dst, const array_wrapper<U>& src)
	{
		if (dst.extents() != src.extents())
			dst.destructive_resize(src.extents());
	}
};

template <>
struct resize_helper<false>
{
	template <class T, class U>
	CC_ALWAYS_INLINE
	static void
	apply(const array_wrapper<T>& dst, const array_wrapper<U>& src)
	{
		nd_assert(
			src.extents() == dst.extents(),
			"destination of assignment has different "
			"extents from source, but is not resizable.\n"
			"▶ Destination extents: $; source extents: $",
			dst.extents(), src.extents()
		);
	}
};

template <
	bool DirectAssignmentFeasible, 
	bool DirectViewFeasible,
	bool FlatViewFeasible
>
struct copy_assign_helper;

template <
	bool DirectAssignmentFeasible, 
	bool DirectViewFeasible,
	bool FlatViewFeasible
>
struct move_assign_helper;

template <bool DirectViewFeasible, bool FlatViewFeasible>
struct copy_assign_helper<true, DirectViewFeasible, FlatViewFeasible>
{
	template <class T, class U>
	CC_ALWAYS_INLINE
	static void
	apply(array_wrapper<T>& dst, const array_wrapper<U>& src)
	{ dst.wrapped() = src.wrapped(); }
};

template <bool FlatViewFeasible>
struct copy_assign_helper<false, true, FlatViewFeasible>
{
	template <class T, class U>
	CC_ALWAYS_INLINE
	static void
	apply(array_wrapper<T>& dst, const array_wrapper<U>& src)
	{
		using src_type = array_wrapper<T>;
		using helper = resize_helper<src_type::is_destructively_resizable>;

		helper::apply(dst, src);
		boost::copy(src.direct_view(), dst.direct_view().begin());
	}
};

template <>
struct copy_assign_helper<false, false, true>
{
	template <class T, class U>
	CC_ALWAYS_INLINE
	static void
	apply(array_wrapper<T>& dst, const array_wrapper<U>& src)
	{
		using src_type = array_wrapper<T>;
		using helper = resize_helper<src_type::is_destructively_resizable>;

		helper::apply(dst, src);
		boost::copy(src.flat_view(), dst.flat_view().begin());
	}
};

template <>
struct copy_assign_helper<false, false, false>
{
	template <class T, class U>
	CC_ALWAYS_INLINE
	static void
	apply(array_wrapper<T>& dst, const array_wrapper<U>& src)
	{
		using src_type = array_wrapper<T>;
		using helper = resize_helper<src_type::is_destructively_resizable>;

		helper::apply(dst, src);
		for_each(src.extents(),
			[&] (const auto& i) CC_ALWAYS_INLINE {
				dst(i) = src(i);
			});
	}
};

template <bool DirectViewFeasible, bool FlatViewFeasible>
struct move_assign_helper<true, DirectViewFeasible, FlatViewFeasible>
{
	template <class T, class U>
	CC_ALWAYS_INLINE
	static void
	apply(array_wrapper<T>& dst, array_wrapper<U>&& src)
	{ dst.wrapped() = std::move(src.wrapped()); }
};

template <bool FlatViewFeasible>
struct move_assign_helper<false, true, FlatViewFeasible>
{
	template <class T, class U>
	CC_ALWAYS_INLINE
	static void
	apply(array_wrapper<T>& dst, array_wrapper<U>&& src)
	{
		using src_type = array_wrapper<T>;
		using helper = resize_helper<src_type::is_destructively_resizable>;

		helper::apply(dst, src);
		std::move(
			src.direct_view().begin(),
			src.direct_view().end(),
			dst.direct_view().begin()
		);
	}
};

template <>
struct move_assign_helper<false, false, true>
{
	template <class T, class U>
	CC_ALWAYS_INLINE
	static void
	apply(array_wrapper<T>& dst, array_wrapper<U>&& src)
	{
		using src_type = array_wrapper<T>;
		using helper = resize_helper<src_type::is_destructively_resizable>;

		helper::apply(dst, src);
		std::move(
			src.flat_view().begin(),
			src.flat_view().end(),
			dst.flat_view().begin()
		);
	}
};

template <>
struct move_assign_helper<false, false, false>
{
	template <class T, class U>
	CC_ALWAYS_INLINE
	static void
	apply(array_wrapper<T>& dst, array_wrapper<U>&& src)
	{
		using src_type = array_wrapper<T>;
		using helper = resize_helper<src_type::is_destructively_resizable>;

		helper::apply(dst, src);
		for_each(src.extents(),
			[&] (const auto& i) CC_ALWAYS_INLINE {
				dst(i) = std::move(src(i));
			});
	}
};

struct assignment_helper
{
	template <class T, class U>
	CC_ALWAYS_INLINE
	static void
	copy_assign(array_wrapper<T>& dst, const array_wrapper<U>& src)
	{
		using traits = copy_assignment_traits<
			array_wrapper<U>, array_wrapper<T>>;
		using helper = copy_assign_helper<
			traits::direct_assignment_feasible,
			traits::assignment_to_direct_view_feasible,
			traits::assignment_to_flat_view_feasible>;
		helper::apply(dst, src);
	}

	template <class T, class U>
	CC_ALWAYS_INLINE
	static void
	move_assign(array_wrapper<T>& dst, array_wrapper<U>&& src)
	{
		using traits = move_assignment_traits<
			array_wrapper<U>, array_wrapper<T>>;
		using helper = move_assign_helper<
			traits::direct_assignment_feasible,
			traits::assignment_to_direct_view_feasible,
			traits::assignment_to_flat_view_feasible>;
		helper::apply(dst, std::move(src));
	}
};

}}

#endif
