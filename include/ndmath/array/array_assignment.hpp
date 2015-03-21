/*
** File Name: array_assignment.hpp
** Author:    Aditya Ramesh
** Date:      03/16/2015
** Contact:   _@adityaramesh.com
**
** TODO: Noexcept specifications. These are irritating to write, and it's not
** clear to me what their practical use would be.
*/

#ifndef Z9A5D0442_8832_4E17_ADF8_1BA2DC724D52
#define Z9A5D0442_8832_4E17_ADF8_1BA2DC724D52

#include <boost/range/algorithm/copy.hpp>

namespace nd {

template <class T>
class array_wrapper;

namespace detail {

template <class SrcIter, class DstIter>
struct iterator_assignment_traits
{
	template <class T, class U>
	static constexpr auto check_copy_assignable(int) ->
	decltype(*std::declval<T>() = *std::declval<U>(), bool{})
	{ return true; }

	template <class T, class U>
	static constexpr auto check_copy_assignable(...)
	{ return false; }

	template <class T, class U>
	static constexpr auto check_move_assignable(int) ->
	decltype(*std::declval<T>() = std::move(*std::declval<U>()), bool{})
	{ return true; }

	template <class T, class U>
	static constexpr auto check_move_assignable(...)
	{ return false; }

	static constexpr auto is_copy_assignable =
	check_copy_assignable<SrcIter, DstIter>(0);

	static constexpr auto is_move_assignable =
	check_move_assignable<SrcIter, DstIter>(0);
};

template <class RHSIter>
struct iterator_assignment_traits<void, RHSIter>
{
	static constexpr auto is_copy_assignable = false;
	static constexpr auto is_move_assignable = false;
};

template <class LHSIter>
struct iterator_assignment_traits<LHSIter, void>
{
	static constexpr auto is_copy_assignable = false;
	static constexpr auto is_move_assignable = false;
};

template <
	bool UseDirectCopyAssign, 
	bool UseDirectViewCopy,
	bool UseFlatViewCopy
>
struct copy_assign_helper;

template <
	bool UseDirectMoveAssign, 
	bool UseDirectViewMove,
	bool UseFlatViewMove
>
struct move_assign_helper;

#define nd_copy_assign_assert                                           \
	if (dst.extents() != src.extents()) {                           \
		nd_assert(                                              \
			array_wrapper<T>::is_unsafe_resizable,          \
			"destination of copy assignment has different " \
			"extents from source, but is not resizable. "   \
			"Destination extents: $; source extents: $",    \
			dst.extents(), src.extents()                    \
		);                                                      \
		dst.unsafe_resize(src.extents());                       \
	}

#define nd_move_assign_assert                                           \
	if (dst.extents() != src.extents()) {                           \
		nd_assert(                                              \
			array_wrapper<T>::is_unsafe_resizable,          \
			"destination of move assignment has different " \
			"extents from source, but is not resizable. "   \
			"Destination extents: $; source extents: $",    \
			dst.extents(), src.extents()                    \
		);                                                      \
		dst.unsafe_resize(src.extents());                       \
	}

template <bool UseDirectViewCopy, bool UseFlatViewCopy>
struct copy_assign_helper<true, UseDirectViewCopy, UseFlatViewCopy>
{
	template <class T, class U>
	CC_ALWAYS_INLINE
	static void
	apply(array_wrapper<T>& dst, const array_wrapper<U>& src)
	{ dst.wrapped() = src.wrapped(); }
};

template <bool UseFlatViewCopy>
struct copy_assign_helper<false, true, UseFlatViewCopy>
{
	template <class T, class U>
	CC_ALWAYS_INLINE
	static void
	apply(array_wrapper<T>& dst, const array_wrapper<U>& src)
	{
		nd_copy_assign_assert
		boost::copy(src.direct_view(), dst.direct_view());
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
		nd_copy_assign_assert
		boost::copy(src.flat_view(), dst.flat_view());
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
		nd_copy_assign_assert
		for_each(src.extents(),
			[&] (const auto& i) CC_ALWAYS_INLINE {
				dst[i] = src[i];
			});
	}
};

template <bool UseDirectViewMove, bool UseFlatViewMove>
struct move_assign_helper<true, UseDirectViewMove, UseFlatViewMove>
{
	template <class T, class U>
	CC_ALWAYS_INLINE
	static void
	apply(array_wrapper<T>& dst, array_wrapper<U>&& src)
	{ dst.wrapped() = std::move(src.wrapped()); }
};

template <bool UseFlatViewMove>
struct move_assign_helper<false, true, UseFlatViewMove>
{
	template <class T, class U>
	CC_ALWAYS_INLINE
	static void
	apply(array_wrapper<T>& dst, array_wrapper<U>&& src)
	{
		nd_move_assign_assert
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
		nd_move_assign_assert
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
		nd_move_assign_assert
		for_each(src.extents(),
			[&] (const auto& i) CC_ALWAYS_INLINE {
				dst[i] = std::move(src[i]);
			});
	}
};

#undef nd_copy_assign_assert
#undef nd_move_assign_assert

struct assignment_helper
{
	/*
	** General procedure for copy assignment:
	** - If dst's wrapped type provides a copy assignment operator for src's
	** wrapped type, then use it and return.
	** - If dst needs to be resized but is not resizable, then raise an
	** error.
	** - If dst and src have compatible storage orders:
	**   - If both dst and src support direct views over the elements, and
	**   it is possible to copy from src's direct view to dst's direct view,
	**   then do so.
	**   - Else if src provides a fast flat view implementation, then copy
	**   from src's flat view to dst's flat view. I think it makes more
	**   sense to base this decision on whether src provides a fast view
	**   implementation rather than dst, because of the common case where we
	**   wish to store the result of a complex expression.
	** - Else, copy using a for_each loop over src's range.
	*/
	template <class T, class U>
	CC_ALWAYS_INLINE
	static void
	copy_assign(array_wrapper<T>& dst, const array_wrapper<U>& src)
	{
		using dst_type = array_wrapper<T>;
		using src_type = array_wrapper<U>;

		static constexpr auto use_direct_copy_assign =
		std::is_assignable<T, const U&>::value;

		using s1 = decltype(dst.storage_order());
		using s2 = decltype(src.storage_order());
		static constexpr auto storage_orders_same = s1{} == s2{};

		using dst_di = typename dst_type::direct_iterator;
		using src_di = typename src_type::direct_iterator;
		using traits = iterator_assignment_traits<dst_di, src_di>;

		static constexpr auto use_direct_view_copy =
		storage_orders_same &&
		dst_type::provides_direct_view &&
		src_type::provides_direct_view &&
		traits::is_copy_assignable;

		static constexpr auto use_flat_view_copy =
		storage_orders_same &&
		src_type::provides_fast_flat_view;

		using helper = copy_assign_helper<
			use_direct_copy_assign,
			use_direct_view_copy,
			use_flat_view_copy
		>;
		helper::apply(dst, src);
	}

	template <class T, class U>
	CC_ALWAYS_INLINE
	static void
	move_assign(array_wrapper<T>& dst, array_wrapper<U>&& src)
	{
		using dst_type = array_wrapper<T>;
		using src_type = array_wrapper<U>;

		static constexpr auto use_direct_move_assign =
		std::is_assignable<T, U&&>::value;

		using s1 = decltype(dst.storage_order());
		using s2 = decltype(src.storage_order());
		static constexpr auto storage_orders_same = s1{} == s2{};

		using dst_di = typename dst_type::direct_iterator;
		using src_di = typename src_type::direct_iterator;
		using traits = iterator_assignment_traits<dst_di, src_di>;

		static constexpr auto use_direct_view_move =
		storage_orders_same &&
		dst_type::provides_direct_view &&
		src_type::provides_direct_view &&
		traits::is_move_assignable;

		static constexpr auto use_flat_view_move =
		storage_orders_same &&
		src_type::provides_fast_flat_view;

		using helper = move_assign_helper<
			use_direct_move_assign,
			use_direct_view_move,
			use_flat_view_move
		>;
		helper::apply(dst, std::move(src));
	}
};

}}

#endif
