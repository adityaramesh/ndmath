/*
** File Name: array_construction.hpp
** Author:    Aditya Ramesh
** Date:      03/29/2015
** Contact:   _@adityaramesh.com
*/

#ifndef ZC5346F5B_67A0_4053_A3CA_AE5631CD511F
#define ZC5346F5B_67A0_4053_A3CA_AE5631CD511F

#include <ndmath/array/array_assignment.hpp>

namespace nd {
namespace detail {

template <
	bool DirectConstructionFeasible,
	bool DirectViewFeasible,
	bool LoopFeasible
>
struct copy_construct_helper;

template <
	bool DirectConstructionFeasible,
	bool FastMoveAssignmentFeasible,
	bool DirectViewFeasible,
	bool LoopFeasible
>
struct move_construct_helper;

template <bool DirectViewFeasible, bool LoopFeasible>
struct copy_construct_helper<true, DirectViewFeasible, LoopFeasible>
{
	template <class T, class U>
	CC_ALWAYS_INLINE constexpr
	static void apply(const array_wrapper<T>&, const array_wrapper<U>&)
	noexcept {}
};

template <bool LoopFeasible>
struct copy_construct_helper<false, true, LoopFeasible>
{
	template <class T, class U>
	CC_ALWAYS_INLINE
	static void apply(array_wrapper<T>& dst, const array_wrapper<U>& src)
	{ boost::copy(src.direct_view(), dst.construction_view().begin()); }
};

template <>
struct copy_construct_helper<false, false, true>
{
	template <class T, class U>
	CC_ALWAYS_INLINE
	static void apply(array_wrapper<T>& dst, const array_wrapper<U>& src)
	{
		nd::for_each(src.extents(),
			[&] (const auto& i) CC_ALWAYS_INLINE {
				dst.uninitialized_at(i) = src(i);
			});
	}
};

template <>
struct copy_construct_helper<false, false, false>
{
	template <class T, class U>
	CC_ALWAYS_INLINE
	static void apply(array_wrapper<T>& dst, const array_wrapper<U>& src)
	{ dst = src; }
};

template <bool FastMoveAssignmentFeasible, bool DirectViewFeasible, bool LoopFeasible>
struct move_construct_helper<true, FastMoveAssignmentFeasible, DirectViewFeasible, LoopFeasible>
{
	template <class T, class U>
	CC_ALWAYS_INLINE constexpr
	static void apply(array_wrapper<T>&, array_wrapper<U>&&)
	noexcept {}
};

template <bool DirectViewFeasible, bool LoopFeasible>
struct move_construct_helper<false, true, DirectViewFeasible, LoopFeasible>
{
	template <class T, class U>
	CC_ALWAYS_INLINE constexpr
	static void apply(array_wrapper<T>& dst, array_wrapper<U>&& src)
	{ dst = std::move(src); }
};

template <bool LoopFeasible>
struct move_construct_helper<false, false, true, LoopFeasible>
{
	template <class T, class U>
	CC_ALWAYS_INLINE
	static void apply(array_wrapper<T>& dst, array_wrapper<U>&& src)
	{
		std::move(
			src.direct_view().begin(),
			src.direct_view().end(),
			dst.construction_view().begin()
		);
	}
};

template <>
struct move_construct_helper<false, false, false, true>
{
	template <class T, class U>
	CC_ALWAYS_INLINE
	static void apply(array_wrapper<T>& dst, array_wrapper<U>&& src)
	{
		nd::for_each(src.extents(),
			[&] (const auto& i) CC_ALWAYS_INLINE {
				dst.uninitialized_at(i) = std::move(src(i));
			});
	}
};

template <>
struct move_construct_helper<false, false, false, false>
{
	template <class T, class U>
	CC_ALWAYS_INLINE
	static void apply(array_wrapper<T>& dst, array_wrapper<U>&& src)
	{ dst = std::move(src); }
};

struct construction_helper
{
	template <class T, class U>
	CC_ALWAYS_INLINE
	static void
	copy_construct(array_wrapper<T>& dst, const array_wrapper<U>& src)
	{
		using traits = copy_construction_traits<
			array_wrapper<U>, array_wrapper<T>>;
		using helper = copy_construct_helper<
			traits::direct_construction_feasible,
			traits::construction_from_direct_view_feasible,
			traits::construction_from_loop_feasible>;
		helper::apply(dst, src);
	}

	template <class T, class U>
	CC_ALWAYS_INLINE
	static void
	move_construct(array_wrapper<T>& dst, array_wrapper<U>&& src)
	{
		using traits = move_construction_traits<
			array_wrapper<U>, array_wrapper<T>>;
		using helper = move_construct_helper<
			traits::direct_construction_feasible,
			traits::fast_move_assignment_feasible,
			traits::construction_from_direct_view_feasible,
			traits::construction_from_loop_feasible>;
		helper::apply(dst, std::move(src));
	}
};

}}

#endif
