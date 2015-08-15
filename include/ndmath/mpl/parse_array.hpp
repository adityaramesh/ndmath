/*
** File Name: parse_array.hpp
** Author:    Aditya Ramesh
** Date:      07/30/2015
** Contact:   _@adityaramesh.com
*/

#ifndef ZF6C45856_8F7C_4F96_BD51_61E26FABD98A
#define ZF6C45856_8F7C_4F96_BD51_61E26FABD98A

#include <ndmath/mpl/flatten_list.hpp>

namespace nd {
namespace detail {
namespace parse_array {

template <
	class Depth, class BracketDepth, class Lists,
	class SemicolonList, class Extents
>
struct state
{
	/*
	** Incremented using `inc_depth` each time we pass a '[' (or a `;` for
	** the first time); decremented using `dec_depth` each time we pass a
	** ']'.
	*/
	using depth = Depth;

	/*
	** Like `depth`, but is not affected by semicolons. This value is used
	** to keep track of whether we have seen a semicolon for the first time
	** after passing a `[`.
	*/
	using bracket_depth = BracketDepth;

	// A list of lists that is used to accumulate the entries of the array.
	using lists = Lists;

	/*
	** The boolean at index `i` of `semicolon_list` indicates whether we
	** have passed by a semicolon at bracket depth `i`. For example, the
	** value of the semicolon list before processing the first semicolon in
	** the array `[0; 1; 2]` would be `[false]`. After processing this
	** semicolon, it would be `[true]`.
	*/
	using semicolon_list = SemicolonList;

	// A list of integers used to compute the extents of the array during
	// parsing.
	using extents = Extents;

	static constexpr auto dims = lists::size;
};

/*
** Utilities to manipulate the array state.
*/

template <class State, bool IsBracket>
struct inc_depth_helper
{
	using cur_depth          = typename State::depth;
	using cur_bracket_depth  = typename State::bracket_depth;
	using cur_lists          = typename State::lists;
	using cur_semicolon_list = typename State::semicolon_list;
	using cur_extents        = typename State::extents;

	static_assert(
		cur_lists::size == cur_extents::size,
		"Internal error: either State::lists or State::extents was not "
		"correctly updated."
	);

	static_assert(
		cur_lists::size >= cur_depth::value,
		"Internal error: unexpected size of State::lists."
	);

	using new_depth = mpl::inc<cur_depth>;

	using new_bracket_depth = mpl::if_c<
		IsBracket,
		mpl::inc<cur_bracket_depth>,
		cur_bracket_depth
	>;

	/*
	** Note: if the current size of `lists` and `extents` is equal to
	** `depth`, then we need to enlarge both of the former data structures.
	*/

	using new_lists = std::conditional_t<
		cur_lists::size == cur_depth::value,
		mpl::append<mpl::list<>, cur_lists>,
		cur_lists
	>;

	using new_semicolon_list = std::conditional_t<
		cur_semicolon_list::size == cur_bracket_depth::value,
		mpl::append<mpl::false_, cur_semicolon_list>,
		cur_semicolon_list
	>;

	using new_extents = std::conditional_t<
		cur_extents::size == cur_depth::value,
		mpl::append<mpl::list_index_c<0>, cur_extents>,
		cur_extents
	>;

	using type = state<
		new_depth, new_bracket_depth, new_lists,
		new_semicolon_list, new_extents
	>;
};

/*
** We need a helper struct to update the `lists` data structure. If `dec_depth`
** is called when `depth == 1` (which happens at the last `]`), then we will
** trigger a compilation error unless we use template specialization. This is
** because updating `lists` involves accessing the sublist at index `depth - 2`.
*/

template <class CurLists, class CurDepth, mpl::list_index DepthReduction>
struct update_lists_helper;

template <class CurLists, class CurDepth>
struct update_lists_helper<CurLists, CurDepth, 1>
{
	using inner_list     = mpl::at_c<CurDepth::value - 1, CurLists>;
	using outer_list     = mpl::at_c<CurDepth::value - 2, CurLists>;
	using new_outer_list = mpl::append<inner_list, outer_list>;

	using new_lists = mpl::replace_at_c<
		CurDepth::value - 2, new_outer_list,
		mpl::replace_at_c<CurDepth::value - 1, mpl::list<>, CurLists>
	>;
};

template <class CurLists>
struct update_lists_helper<CurLists, mpl::list_index_c<1>, 1>
{
	using inner_list = mpl::at_c<0, CurLists>;
	using new_lists  = CurLists;
};

template <class CurLists, class CurDepth>
struct update_lists_helper<CurLists, CurDepth, 2>
{
	using inner_list     = mpl::at_c<CurDepth::value - 1, CurLists>;
	using outer_list     = mpl::at_c<CurDepth::value - 2, CurLists>;
	using parent_list    = mpl::at_c<CurDepth::value - 3, CurLists>;
	using new_outer_list = mpl::append<inner_list, outer_list>;

	using new_lists = mpl::replace_at_c<
		CurDepth::value - 3,
		mpl::append<new_outer_list, parent_list>,
		mpl::replace_at_c<
			CurDepth::value - 2,
			mpl::list<>,
			mpl::replace_at_c<
				CurDepth::value - 1,
				mpl::list<>,
				CurLists
			>
		>
	>;
};

template <class CurLists>
struct update_lists_helper<CurLists, mpl::list_index_c<2>, 2>
{
	using helper         = update_lists_helper<CurLists, mpl::list_index_c<2>, 1>;
	using inner_list     = typename helper::inner_list;
	using new_outer_list = typename helper::new_outer_list;
	using new_lists      = typename helper::new_lists;
};

/*
** We need to make a special case for semicolons when processing `]` characters.
** Consider the following example: `[0; 1; 2]`. This array is equivalent to
** `[[0] [1] [2]]`. When we encounter the `]` character after the `2` in the
** first array, we must actually decrement the current depth by two. We also
** need to reset the boolean that indicates whether we have encountered a
** semicolon at bracket level one to false. In total, there are four cases:
**
** 1. `]` that is not followed by a semicolon at the same level, or a `;`.
** 2. `]` at depth 1.
** 3. `]` followed by a semicolon, at depth greater than 2 (this is the case
**    that is discussed above).
** 4. `]` at depth 2.
*/
template <class State, bool IsBracket, mpl::list_index DepthReduction>
struct dec_depth_impl;

template <class State, bool IsBracket>
struct dec_depth_impl<State, IsBracket, 1>
{
	using cur_depth         = typename State::depth;
	using cur_bracket_depth = typename State::bracket_depth;
	using cur_lists         = typename State::lists;
	using semicolon_list    = typename State::semicolon_list;
	using cur_extents       = typename State::extents;

	static_assert(
		cur_depth::value != 0,
		"Internal error: depth must be positive."
	);

	static_assert(
		cur_bracket_depth::value != 0,
		"Internal error: bracket depth must be positive."
	);

	using helper     = update_lists_helper<cur_lists, cur_depth, 1>;
	using new_lists  = typename helper::new_lists;
	using inner_list = typename helper::inner_list;
	using cur_extent = mpl::at<mpl::dec<cur_depth>, cur_extents>;

	/*
	** TODO: This assertion fires spuriously for examples such as
	** `[[0; 1]; [2; 3]; [4; 5]]`. Ideally we would modify this assertion so
	** that it is still useful.
	**
	static_assert(
		cur_extent::value == 0 ||
		cur_extent::value == inner_list::size,
		"Mismatching extents."
	);
	*/

	using new_depth = mpl::dec<cur_depth>;

	using new_bracket_depth = mpl::if_c<
		IsBracket,
		mpl::dec<cur_bracket_depth>,
		cur_bracket_depth
	>;

	using new_extents = mpl::replace_at<
		mpl::dec<cur_depth>,
		mpl::size<inner_list>,
		cur_extents
	>;

	using type = state<
		new_depth, new_bracket_depth, new_lists,
		semicolon_list, new_extents
	>;
};

template <class State, bool IsBracket>
struct dec_depth_impl<State, IsBracket, 2>
{
	using cur_depth          = typename State::depth;
	using cur_bracket_depth  = typename State::bracket_depth;
	using cur_lists          = typename State::lists;
	using cur_semicolon_list = typename State::semicolon_list;
	using cur_extents        = typename State::extents;

	static_assert(
		IsBracket,
		"Internal error: depth should only be reduced by 2 at a "
		"bracket following a semicolon."
	);

	static_assert(
		cur_depth::value >= 2,
		"Internal error: semicolon encountered at depth 0 or 1."
	);

	static_assert(
		cur_bracket_depth::value != 0,
		"Internal error: bracket depth must be positive."
	);

	using helper         = update_lists_helper<cur_lists, cur_depth, 2>;
	using new_lists      = typename helper::new_lists;
	using inner_list     = typename helper::inner_list;
	using new_outer_list = typename helper::new_outer_list;
	using inner_extent   = mpl::at_c<cur_depth::value - 1, cur_extents>;
	using outer_extent   = mpl::at_c<cur_depth::value - 2, cur_extents>;

	/*
	** TODO: This assertion fires spuriously for examples such as
	** `[[0; 1]; [2; 3]; [4; 5]]`. Ideally we would modify this assertion so
	** that it is still useful.
	**
	static_assert(
		inner_extent::value == 0 ||
		inner_extent::value == inner_list::size,
		"Mismatching extents."
	);
	*/

	/*
	** TODO: This assertion fires spuriously for examples such as
	** `[[0; 1]; [2; 3]; [4; 5]]`. Ideally we would modify this assertion so
	** that it is still useful.
	**
	static_assert(
		outer_extent::value == 0 ||
		outer_extent::value == new_outer_list::size,
		"Mismatching extents."
	);
	*/

	using new_depth = mpl::minus<cur_depth, mpl::list_index_c<2>>;

	using new_bracket_depth = mpl::dec<cur_bracket_depth>;

	using new_extents = mpl::replace_at_c<
		cur_depth::value - 2,
		mpl::size<new_outer_list>,
		mpl::replace_at_c<
			cur_depth::value - 1,
			mpl::size<inner_list>,
			cur_extents
		>
	>;

	using new_semicolon_list = mpl::replace_at<
		mpl::dec<cur_bracket_depth>,
		mpl::false_,
		cur_semicolon_list
	>;

	using type = state<
		new_depth, new_bracket_depth, new_lists,
		new_semicolon_list, new_extents
	>;
};

template <class State, bool IsBracket>
struct dec_depth_helper
{
	using depth          = typename State::depth;
	using bracket_depth  = typename State::bracket_depth;
	using semicolon_list = typename State::semicolon_list;
	using has_semicolon  = mpl::at<mpl::dec<bracket_depth>, semicolon_list>;

	static constexpr auto depth_reduction =
	IsBracket && has_semicolon::value ? 2 : 1;

	using helper = dec_depth_impl<State, IsBracket, depth_reduction>;
	using type   = mpl::_t<helper>;
};

template <class T, class State>
struct append_helper
{
	using depth          = typename State::depth;
	using bracket_depth  = typename State::bracket_depth;
	using extents        = typename State::extents;
	using semicolon_list = typename State::semicolon_list;
	using cur_lists      = typename State::lists;

	using new_lists = mpl::replace_at<
		mpl::dec<depth>,
		mpl::append<T, mpl::at<mpl::dec<depth>, cur_lists>>,
		cur_lists
	>;

	using type = state<
		depth, bracket_depth, new_lists,
		semicolon_list, extents
	>;
};

template <class State, bool IsBracket = true>
using inc_depth = typename inc_depth_helper<State, IsBracket>::type;

template <class State, bool IsBracket = true>
using dec_depth = typename dec_depth_helper<State, IsBracket>::type;

template <class T, class State>
using append = typename append_helper<T, State>::type;

/*
** The helper structs used to process semicolons are defined after the above
** template aliases, because we need `inc_depth` and `dec_depth`.
*/

/*
** When we encounter a seimcolon, there are two cases that determine the action
** we must take. Consider the following example: `[0 1; 2 3; 4 5]`. This array
** is equivalent to `[[0 1] [2 3] [4 5]]`. When we pass by the first semicolon,
** the list at depth 1 is `[0 1]`. The first semicolon tells us that the current
** contents of the list actually form the first row, so we change the list to
** `[[0 1]]`. After processing the first semicolon, we also increase the current
** depth from 1 to 2 (since the semicolon works as if we have processed `][`).
** When we pass by additional semicolons at the same level, we need only call
** `dec_depth` and `inc_depth`. These two cases are distinguished by the use of
** the `IsFirstSemicolon` template parameter.
*/

template <class State, bool IsFirstSemicolon>
struct process_semicolon_impl;

template <class State>
struct process_semicolon_impl<State, true>
{
	using new_state          = inc_depth<State, false>;
	using depth              = typename new_state::depth;
	using bracket_depth      = typename new_state::bracket_depth;
	using extents            = typename new_state::extents;
	using cur_lists          = typename new_state::lists;
	using cur_semicolon_list = typename new_state::semicolon_list;

	using new_lists = mpl::replace_at_c<
		depth::value - 2,
		mpl::list<mpl::at_c<depth::value - 2, cur_lists>>,
		cur_lists
	>;

	using new_semicolon_list = mpl::replace_at<
		mpl::dec<bracket_depth>,
		mpl::true_,
		cur_semicolon_list
	>;

	using type = state<
		depth, bracket_depth, new_lists,
		new_semicolon_list, extents
	>;
};

template <class State>
struct process_semicolon_impl<State, false>
{
	using type = inc_depth<dec_depth<State, false>, false>;

	static_assert(
		State::depth::value == type::depth::value,
		"Internal error: depth changed after processing semicolon."
	);
};

template <class State>
struct process_semicolon_helper
{
	using bracket_depth  = typename State::bracket_depth;
	using semicolon_list = typename State::semicolon_list;

	static constexpr auto is_first_semicolon = !mpl::at<
		mpl::dec<bracket_depth>, semicolon_list
	>::value;

	using type = mpl::_t<process_semicolon_impl<State, is_first_semicolon>>;
};

template <class State>
using process_semicolon = mpl::_t<process_semicolon_helper<State>>;

/*
** Helper structs to parse the array.
*/

template <class Parser, class State, class Tail>
struct parse_helper;

template <class Parser, class State>
struct parse_helper<Parser, State, mpl::list<>>
{ using type = State; };

template <class Parser, class State, class... Ts>
struct parse_helper<Parser, State, mpl::list<mpl::char_<'['>, Ts...>>
{
	using type = typename parse_helper<
		Parser, inc_depth<State>, mpl::list<Ts...>
	>::type;
};

template <class Parser, class State, class... Ts>
struct parse_helper<Parser, State, mpl::list<mpl::char_<']'>, Ts...>>
{
	using type = typename parse_helper<
		Parser, dec_depth<State>, mpl::list<Ts...>
	>::type;
};

template <class Parser, class State, class... Ts>
struct parse_helper<Parser, State, mpl::list<mpl::char_<';'>, Ts...>>
{
	using type = typename parse_helper<
		Parser, process_semicolon<State>, mpl::list<Ts...>
	>::type;
};

template <bool IsWhitespace, class Parser, class State, class List>
struct parse_helper_2;

template <class Parser, class State, class T, class... Ts>
struct parse_helper_2<true, Parser, State, mpl::list<T, Ts...>>
{
	using type = typename parse_helper<
		Parser, State, mpl::list<Ts...>
	>::type;
};

template <class Parser, class State, class T, class... Ts>
struct parse_helper_2<false, Parser, State, mpl::list<T, Ts...>>
{
	using result = mpl::apply<Parser, mpl::list<T, Ts...>>;
	using scalar = typename result::type;
	using tail   = typename result::tail;

	using type = typename parse_helper<
		Parser, append<scalar, State>, tail
	>::type;
};

template <class Parser, class State, class T, class... Ts>
struct parse_helper<Parser, State, mpl::list<T, Ts...>>
{
	using helper = parse_helper_2<
		mpl::apply<is_whitespace, T>::value,
		Parser, State, mpl::list<T, Ts...>
	>;

	using type = typename helper::type;

	static_assert(
		type::depth::value == 0,
		"Unmatched '[' or ']'."
	);
};

}}

template <class Parser, class List>
struct parse_array
{
	using state = detail::parse_array::state<
		mpl::list_index_c<0>, mpl::list_index_c<0>,
		mpl::list<>, mpl::list<>, mpl::list<>
	>;

	using helper  = detail::parse_array::parse_helper<Parser, state, List>;
	using type    = typename helper::type;
	using lists   = typename type::lists;
	using extents = typename type::extents;

	using flattened_array = flatten_list<lists>;
	using count = mpl::apply_list<mpl::quote<mpl::multiplies>, extents>;
	
	static_assert(
		count::value == flattened_array::size,
		"Internal error: product of extents does not equal array size."
	);
};

}

#endif
