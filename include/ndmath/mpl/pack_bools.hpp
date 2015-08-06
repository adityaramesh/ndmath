/*
** File Name: pack_bools.hpp
** Author:    Aditya Ramesh
** Date:      08/01/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z88870006_5EAF_4B5F_84BC_D1CD639FA7A9
#define Z88870006_5EAF_4B5F_84BC_D1CD639FA7A9

#include <ndmath/mpl/common.hpp>

namespace nd {
namespace detail {

template <class Integer, class List>
struct pack_bools_helper
{
	static_assert(
		std::is_integral<Integer>::value,
		"Provided integer type is not integral."
	);

	static_assert(
		List::size <= 8 * sizeof(Integer),
		"Too many bools to pack into given integral type."
	);

	/*
	** Note that we use `reverse_fold` instead of `fold`, because the value
	** of the LSB should be the first boolean in the list.
	*/
	using result = mpl::reverse_fold<
		List,
		std::integral_constant<Integer, 0>,
		mpl::compose<
			mpl::quote<mpl::list>,
			mpl::uncurry<mpl::make_list<
				mpl::bind_back<
					mpl::quote<mpl::left_shift>,
					std::integral_constant<Integer, 1>
				>,
				mpl::bind_back<
					mpl::quote<mpl::if_>,
					std::integral_constant<Integer, 1>,
					std::integral_constant<Integer, 0>
				>
			>>,
			mpl::uncurry<mpl::quote<mpl::bit_or>>
		>
	>;

	using type = std::ratio<result::value, 1>;
};

}

template <class Integer, class List>
using pack_bools = mpl::_t<detail::pack_bools_helper<Integer, List>>;

namespace detail {
namespace pack_bools {

template <class Integer, class IntList, class BoolList>
struct state
{
	using integer    = Integer;
	using ratio_list = IntList;
	using bool_list  = BoolList;
};

template <class T, class State>
struct append
{
	using integer        = typename State::integer;
	using cur_ratio_list = typename State::ratio_list;
	using cur_bool_list  = typename State::bool_list;

	using temp = mpl::append<T, cur_bool_list>;

	using new_bool_list = std::conditional_t<
		temp::size == 8 * sizeof(integer),
		mpl::list<>, temp
	>;

	using new_ratio_list = std::conditional_t<
		temp::size == 8 * sizeof(integer),
		mpl::append<nd::pack_bools<integer, temp>, cur_ratio_list>,
		cur_ratio_list
	>;

	using type = state<integer, new_ratio_list, new_bool_list>;
};

template <class State>
struct get_ratio_list;

template <class Integer, class IntList>
struct get_ratio_list<state<Integer, IntList, mpl::list<>>>
{ using type = IntList; };

template <class Integer, class IntList, class BoolList>
struct get_ratio_list<state<Integer, IntList, BoolList>>
{ using type = mpl::append<nd::pack_bools<Integer, BoolList>, IntList>; };

template <class Integer, class List>
struct helper
{
	static_assert(
		List::size != 0,
		"Boolean list is empty."
	);

	using final_state = mpl::fold<
		List,
		state<Integer, mpl::list<>, mpl::list<>>,
		mpl::reverse_args<mpl::quote_trait<append>>
	>;

	using type = mpl::_t<get_ratio_list<final_state>>;
};

}}

template <class Integer, class List>
using pack_bool_list = mpl::_t<detail::pack_bools::helper<Integer, List>>;

}

#endif
