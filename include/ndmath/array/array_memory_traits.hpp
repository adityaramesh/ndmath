/*
** File Name: array_memory_traits.hpp
** Author:    Aditya Ramesh
** Date:      03/27/2015
** Contact:   _@adityaramesh.com
**
** Traits used to determine how array construction and assignment are
** implemented.
*/

#ifndef ZF3BECB0C_8BCF_499F_AEB6_736DB2986112
#define ZF3BECB0C_8BCF_499F_AEB6_736DB2986112

namespace nd {

struct partial_init_t;
struct uninitialized_t;

namespace detail {

template <class LHSIter, class RHSIter>
struct iterator_assignment_traits
{
	template <class T, class U>
	static constexpr auto check_copy_assignable(T*, U*) ->
	decltype(*std::declval<T>() = *std::declval<U>(), bool{})
	{ return true; }

	template <class T, class U>
	static constexpr auto check_copy_assignable(...)
	{ return false; }

	template <class T, class U>
	static constexpr auto check_move_assignable(T*, U*) ->
	decltype(*std::declval<T>() = std::move(*std::declval<U>()), bool{})
	{ return true; }

	template <class T, class U>
	static constexpr auto check_move_assignable(...)
	{ return false; }

	static constexpr auto is_copy_assignable =
	check_copy_assignable<LHSIter, RHSIter>(0, 0);

	static constexpr auto is_move_assignable =
	check_move_assignable<LHSIter, RHSIter>(0, 0);
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
** Otherwise, resize.
** - If dst and src have compatible storage orders:
**   - If both dst and src support underlying views over the elements, and it is
**   possible to copy from src's underlying view to dst's underlying view, then do so.
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

	static constexpr auto can_use_direct_assignment =
	std::is_assignable<dst_wrapped, const src_wrapped&>::value;

	using src_order = decltype(std::declval<Src>().storage_order());
	using dst_order = decltype(std::declval<Dst>().storage_order());

	static constexpr auto storage_orders_same =
	src_order{} == dst_order{};

	using dst_di = typename Dst::underlying_iterator;
	using src_di = typename Src::underlying_iterator;
	using traits = iterator_assignment_traits<dst_di, src_di>;

	static constexpr auto can_use_underlying_view =
	storage_orders_same           &&
	Dst::provides_underlying_view &&
	Src::provides_underlying_view &&
	traits::is_copy_assignable;

	static constexpr auto can_use_flat_view =
	storage_orders_same &&
	Dst::provides_fast_flat_view;
};

/*
** See comments for copy assignment.
*/
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

	static constexpr auto can_use_direct_assignment =
	std::is_assignable<dst_wrapped, src_wrapped&&>::value;

	using src_order = decltype(std::declval<Src>().storage_order());
	using dst_order = decltype(std::declval<Dst>().storage_order());

	static constexpr auto storage_orders_same =
	src_order{} == dst_order{};

	using dst_di = typename Dst::underlying_iterator;
	using src_di = typename Src::underlying_iterator;
	using traits = iterator_assignment_traits<dst_di, src_di>;

	static constexpr auto can_use_underlying_view =
	storage_orders_same           &&
	Dst::provides_underlying_view &&
	Src::provides_underlying_view &&
	traits::is_move_assignable;

	static constexpr auto can_use_flat_view =
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
** - If dst supports fast initialization and dst's underlying type is copy
** constructible from src's underlying type:
**   - Construct dst in a partially-initialized state with the parameters
**     supplied to the copy constructor.
**   - If dst and src have compatible storage orders and src provides a underlying
**   view over the elements:
**     - Copy from src's underlying view to dst's construction view
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

	static constexpr auto can_use_direct_construction =
	std::is_constructible<dst_wrapped, const src_wrapped&>::value;

	using src_type = typename Src::underlying_type;
	using dst_type = typename Dst::underlying_type;

	static constexpr auto can_use_indirect_construction =
	Src::supports_fast_initialization &&
	std::is_constructible<dst_type, const src_type&>::value;

	using src_order = decltype(std::declval<Src>().storage_order());
	using dst_order = decltype(std::declval<Dst>().storage_order());

	static constexpr auto storage_orders_same =
	src_order{} == dst_order{};

	static constexpr auto can_use_underlying_view =
	can_use_indirect_construction &&
	storage_orders_same           &&
	Dst::provides_underlying_view;

	static constexpr auto can_use_loop =
	can_use_indirect_construction;
};

/*
** General procedure for move construction. The basic idea is to use the most
** efficient mechanism for move construction that is supported by both src and
** dst.
**
** Note that this procedure is different from the one for copy construction!
** When performing copy construction, we only use copy assignment as a last
** resort when all methods to copy construct the elements of dst from those of
** src are ruled out as impossible. For move construction, the opposite is true.
** If the wrapped type implements move assignment, then it is likely to be much
** cheaper than move-constructing the elements of dst from those of src.
**
** - If dst's wrapped type provides a move constructor for src's type, then use
** it and return.
** - If dst supports fast initialization:
** 	- If dst's wrapped type provides a move assignment operator for src's
** 	wrapped type:
**   	        - Construct dst in an uninitialized state with the parameters
**   	          supplied to the move constructor.
** 		- Move assign src to dst.
** 	- Else if dst's underlying type is move constructible from src's
** 	underlying type:
**   	        - Construct dst in an partially-initialized state with the
**   	          parameters supplied to the move constructor.
**   		- If dst and src have compatible storage orders and src provides
**   		a underlying view over the elements:
**     			- Move from src's underlying view to dst's construction
**     			view.
**   		- Else:
**     			- Use a for-each loop together with uninitialized_at to
**     			move-construct dst's elements from those of src.
** - Else:
**   - Default construct dst with the parameters supplied to the move
**   constructor.
**   - Move assign src to dst.
*/
template <class Src, class Dst>
struct move_construction_traits
{
	using src_wrapped = typename Src::wrapped_type;
	using dst_wrapped = typename Dst::wrapped_type;

	static constexpr auto can_use_direct_construction =
	std::is_constructible<dst_wrapped, src_wrapped&&>::value;

	static constexpr auto can_use_fast_move_assignment =
	Src::supports_fast_initialization &&
	std::is_assignable<dst_wrapped, src_wrapped&&>::value;

	using src_type = typename Src::underlying_type;
	using dst_type = typename Dst::underlying_type;

	static constexpr auto can_use_indirect_construction =
	Src::supports_fast_initialization &&
	std::is_constructible<dst_type, src_type&&>::value;

	using initialization_tag = mpl::if_c<
		can_use_fast_move_assignment,
		uninitialized_t,
		mpl::if_c<
			can_use_indirect_construction,
			partial_init_t, uninitialized_t
		>
	>;

	using src_order = decltype(std::declval<Src>().storage_order());
	using dst_order = decltype(std::declval<Dst>().storage_order());

	static constexpr auto storage_orders_same =
	src_order{} == dst_order{};

	static constexpr auto can_use_underlying_view =
	can_use_indirect_construction &&
	storage_orders_same           &&
	Dst::provides_underlying_view;

	static constexpr auto can_use_loop =
	can_use_indirect_construction;
};

}}

#endif
