/*
** File Name: array_assignment_traits.hpp
** Author:    Aditya Ramesh
** Date:      03/27/2015
** Contact:   _@adityaramesh.com
*/

#ifndef ZF3BECB0C_8BCF_499F_AEB6_736DB2986112
#define ZF3BECB0C_8BCF_499F_AEB6_736DB2986112

namespace nd {
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

/*
** General procedure for copy assignment. The basic idea is to use the most
** efficient mechanism for copy assignment that is supported by both src and
** dst.
**
** - If dst's wrapped type provides a copy assignment operator for src's wrapped
** type, then use it and return.
** - If dst needs to be resized but is not resizable, then raise an error.
** - If src is not lazy and dst and src have compatible storage orders:
**   - If both dst and src support direct views over the elements, and it is
**   possible to copy from src's direct view to dst's direct view, then do so.
**   - Else if dst provides a fast flat view implementation, then copy from
**   src's flat view to dst's flat view.
** - Else, copy using a for-each loop over src's range.
*/
template <class Src, class Dst>
struct copy_assignment_traits
{
	using src_wrapped = typename Src::wrapped_type;
	using dst_wrapped = typename Dst::wrapped_type;

	using src_type = typename Src::const_reference;
	using dst_type = typename Dst::reference;

	static_assert(
		std::is_assignable<dst_type, src_type>::value,
		"Destination array cannot be assigned to source array."
	);

	static constexpr auto direct_assignment_feasible =
	std::is_assignable<dst_wrapped, const src_wrapped&>::value;

	using src_order = decltype(std::declval<Src>().storage_order());
	using dst_order = decltype(std::declval<Dst>().storage_order());

	static constexpr auto storage_orders_same =
	src_order{} == dst_order{};

	using dst_di = typename Dst::direct_iterator;
	using src_di = typename Src::direct_iterator;
	using traits = iterator_assignment_traits<dst_di, src_di>;

	static constexpr auto assignment_to_direct_view_feasible =
	storage_orders_same       &&
	Dst::provides_direct_view &&
	Src::provides_direct_view &&
	traits::is_copy_assignable;

	static constexpr auto assignment_to_flat_view_feasible =
	!Src::is_lazy       &&
	storage_orders_same &&
	Dst::provides_fast_flat_view;
};

template <class Src, class Dst>
struct move_assignment_traits
{
	using src_wrapped = typename Src::wrapped_type;
	using dst_wrapped = typename Dst::wrapped_type;

	using src_type = typename std::remove_reference_t<
		typename Src::reference>&&;
	using dst_type = typename Dst::reference;

	static_assert(
		std::is_assignable<dst_type, src_type>::value,
		"Destination array cannot be assigned to source array."
	);

	static constexpr auto direct_assignment_feasible =
	std::is_assignable<dst_wrapped, src_wrapped&&>::value;

	using src_order = decltype(std::declval<Src>().storage_order());
	using dst_order = decltype(std::declval<Dst>().storage_order());

	static constexpr auto storage_orders_same =
	src_order{} == dst_order{};

	using dst_di = typename Dst::direct_iterator;
	using src_di = typename Src::direct_iterator;
	using traits = iterator_assignment_traits<dst_di, src_di>;

	static constexpr auto assignment_to_direct_view_feasible =
	storage_orders_same       &&
	Dst::provides_direct_view &&
	Src::provides_direct_view &&
	traits::is_move_assignable;

	static constexpr auto assignment_to_flat_view_feasible =
	!Src::is_lazy       &&
	storage_orders_same &&
	Dst::provides_fast_flat_view;
};

/*
** General procedure for copy construction. The basic idea is to use the most
** efficient mechanism for copy construction that is supported by both src and
** dst.
**
** - If dst's wrapped type provides a copy constructor for src's type, then use
** it and return.
** - If dst supports late initialization and dst's underlying type is copy
** constructible from src's underlying type:
**   - Construct dst in an uninitialized state with the parameters supplied to
**   the copy constructor.
**   - If dst and src have compatible storage orders and src provides a direct
**   view over the elements:
**     - Copy from src's direct view to dst's construction view
**   - Else:
**     - Use a for-each loop together with uninitialized_at to copy-construct
**     dst's elements from those of src.
** - Else:
**   - Default construct dst with the parameters supplied to the copy
**   constructor.
**   - Copy assign src to dst.
*/
template <class Src, class Dst>
struct copy_construction_traits
{
	using src_wrapped = typename Src::wrapped_type;
	using dst_wrapped = typename Dst::wrapped_type;

	static constexpr auto direct_construction_feasible =
	std::is_constructible<dst_wrapped, const src_wrapped&>::value;

	using src_type = typename Src::underlying_type;
	using dst_type = typename Dst::underlying_type;

	static constexpr auto indirect_copy_construction_feasible =
	Src::supports_late_initialization &&
	std::is_constructible<dst_type, src_type>::value;

	using src_order = decltype(std::declval<Src>().storage_order());
	using dst_order = decltype(std::declval<Dst>().storage_order());

	static constexpr auto storage_orders_same =
	src_order{} == dst_order{};

	static constexpr auto construction_from_direct_view_feasible =
	indirect_copy_construction_feasible &&
	storage_orders_same                 &&
	Dst::provides_direct_view;

	static constexpr auto construction_from_loop_feasible =
	indirect_copy_construction_feasible;
};

// TODO: move construction traits

}}

#endif
