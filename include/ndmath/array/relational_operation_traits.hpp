/*
** File Name: elementwise_operation_traits.hpp
** Author:    Aditya Ramesh
** Date:      08/12/2015
** Contact:   _@adityaramesh.com
** 
** Traits used to determine how relational operations between arrays are
** implemented.
*/

#ifndef Z76A05F64_9C8A_4357_B911_031A6F51AAE8
#define Z76A05F64_9C8A_4357_B911_031A6F51AAE8

#include <ndmath/array/array_memory_traits.hpp>

namespace nd {
namespace detail {

/*
** General procedure for deciding how a reduction operation should be
** implemented. Our goal is to reduce the reduction operation to a single
** for-loop, whenever this would provide a gain in efficiency.
**
** Call the two arrays involved in the operation `A` and `B`.
** - If the exterior types of A and B are both `bool`, and A and B both have the
**   same underlying type, then use a while loop over the direct views.
** - Else if A or B provides a fast flat view implementation, then use a while
**   loop over the flat views.
** - Else, use a while-each loop over the arrays' range.
*/
template <class A, class B>
struct relational_operation_traits
{
	using src_etype = typename A::exterior_type;
	using dst_etype = typename B::exterior_type;
	using src_utype = typename A::underlying_type;
	using dst_utype = typename B::underlying_type;

	using src_order = decltype(std::declval<A>().storage_order());
	using dst_order = decltype(std::declval<B>().storage_order());

	static constexpr auto storage_orders_same =
	src_order{} == dst_order{};

	static constexpr auto can_use_direct_view =
	std::is_same<src_etype, bool>::value      &&
	std::is_same<dst_etype, bool>::value      &&
	std::is_same<src_utype, dst_utype>::value &&
	storage_orders_same                       &&
	A::provides_direct_view                   &&
	B::provides_direct_view;

	static constexpr auto can_use_flat_view =
	storage_orders_same &&
	(A::provides_fast_flat_view || B::provides_fast_flat_view);
};

}}

#endif
