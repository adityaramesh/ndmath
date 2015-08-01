/*
** File Name: parse_array.hpp
** Author:    Aditya Ramesh
** Date:      07/30/2015
** Contact:   _@adityaramesh.com
*/

#ifndef ZF6C45856_8F7C_4F96_BD51_61E26FABD98A
#define ZF6C45856_8F7C_4F96_BD51_61E26FABD98A

namespace nd {
namespace detail {

template <class Depth, class Lists, class Extents>
struct array_state
{
	// Incremented using `inc_depth` each time we pass a '['; decremented
	// using `dec_depth` each time we pass a ']'.
	using depth   = Depth;
	using lists   = Lists;
	using extents = Extents;

	static constexpr auto dims = lists::size();
};

/*
** Utilities to manipulate the array state.
*/

template <class State>
struct inc_depth_helper
{
	using cur_depth   = typename State::depth;
	using cur_lists   = typename State::lists;
	using cur_extents = typename State::extents;

	static_assert(
		cur_lists::size() == cur_extents::size(),
		"Internal error: either State::lists or State::extents was not "
		"correctly updated."
	);
	
	static_assert(
		cur_lists::size() >= cur_depth::value,
		"Internal error: unexpected size of State::lists."
	);

	using new_depth = mpl::plus<cur_depth, mpl::size_t<1>>;

	/*
	** Note: if the current size of `lists` and `extents` is equal to
	** `depth`, then we need to enlarge both of the former data structures.
	*/
	
	using new_lists = std::conditional_t<
		cur_lists::size() == cur_depth::value,
		mpl::append<mpl::list<>, cur_lists>,
		cur_lists
	>;

	using new_extents = std::conditional_t<
		cur_extents::size() == cur_depth::value,
		mpl::append<mpl::size_t<0>, cur_extents>,
		cur_extents
	>;

	using type = array_state<new_depth, new_lists, new_extents>;
};

/*
** We need a helper struct to update the `lists` data structure. If `dec_depth`
** is called when `depth == 1` (which happens at the last `]`), then we will
** trigger a compilation error unless we use template specialization. This is
** updating `lists` involves accessing the sublist at index `depth - 2`.
*/

template <class CurDepth, class CurLists>
struct get_new_lists_helper
{
	using new_depth      = mpl::minus<CurDepth, mpl::size_t<1>>;
	using completed_list = mpl::at_c<CurDepth::value - 1, CurLists>;
	using prev_list      = mpl::at_c<new_depth::value - 1, CurLists>;

	using new_lists = mpl::replace_at_c<
		new_depth::value - 1,
		mpl::append<completed_list, prev_list>,
		mpl::replace_at_c<CurDepth::value - 1, mpl::list<>, CurLists>
	>;
};

template <class CurLists>
struct get_new_lists_helper<mpl::size_t<1>, CurLists>
{
	using new_depth      = mpl::size_t<0>;
	using completed_list = mpl::at_c<0, CurLists>;
	using new_lists      = CurLists;
};

template <class State>
struct dec_depth_helper
{
	using cur_depth   = typename State::depth;
	using cur_lists   = typename State::lists;
	using cur_extents = typename State::extents;

	static_assert(
		cur_depth::value != 0,
		"Internal error: depth must be positive."
	);

	using helper         = get_new_lists_helper<cur_depth, cur_lists>;
	using new_depth      = typename helper::new_depth;
	using new_lists      = typename helper::new_lists;
	using completed_list = typename helper::completed_list;
	using cur_extent     = mpl::at_c<cur_depth::value - 1, cur_extents>;

	static_assert(
		cur_extent::value == 0 ||
		cur_extent::value == completed_list::size(),
		"Mismatching extents."
	);

	using new_extents = mpl::replace_at_c<
		cur_depth::value - 1,
		mpl::size<completed_list>,
		cur_extents
	>;

	using type = array_state<new_depth, new_lists, new_extents>;
};

template <class T, class State>
struct append_helper
{
	using depth     = typename State::depth;
	using extents   = typename State::extents;
	using cur_lists = typename State::lists;

	static_assert(
		depth::value == State::dims,
		"Encountered scalar at incorrect level of nesting."
	);
	
	using new_lists = mpl::replace_at_c<
		cur_lists::size() - 1,
		mpl::append<T, mpl::back<cur_lists>>,
		cur_lists
	>;

	using type = array_state<depth, new_lists, extents>;
};

template <class State>
using inc_depth = typename inc_depth_helper<State>::type;

template <class State>
using dec_depth = typename dec_depth_helper<State>::type;

template <class T, class State>
using append = typename append_helper<T, State>::type;

/*
** Helper structs to parse the array.
*/

template <class Parser, class State, class Tail>
struct parse_array_helper;

template <class Parser, class State>
struct parse_array_helper<Parser, State, mpl::list<>>
{ using type = State; };

template <class Parser, class State, class... Ts>
struct parse_array_helper<Parser, State, mpl::list<mpl::char_<'['>, Ts...>>
{
	using type = typename parse_array_helper<
		Parser, inc_depth<State>, mpl::list<Ts...>
	>::type;
};

template <class Parser, class State, class... Ts>
struct parse_array_helper<Parser, State, mpl::list<mpl::char_<']'>, Ts...>>
{
	using type = typename parse_array_helper<
		Parser, dec_depth<State>, mpl::list<Ts...>
	>::type;
};

template <bool IsWhitespace, class Parser, class State, class List>
struct parse_array_helper_2;

template <class Parser, class State, class T, class... Ts>
struct parse_array_helper_2<true, Parser, State, mpl::list<T, Ts...>>
{
	using type = typename parse_array_helper<
		Parser, State, mpl::list<Ts...>
	>::type;
};

template <class Parser, class State, class T, class... Ts>
struct parse_array_helper_2<false, Parser, State, mpl::list<T, Ts...>>
{
	using result = mpl::apply<Parser, mpl::list<T, Ts...>>;
	using scalar = typename result::type;
	using tail   = typename result::tail;

	using type = typename parse_array_helper<
		Parser, append<scalar, State>, tail
	>::type;
};

template <class Parser, class State, class T, class... Ts>
struct parse_array_helper<Parser, State, mpl::list<T, Ts...>>
{
	using helper = parse_array_helper_2<
		mpl::apply<is_whitespace, T>::value,
		Parser, State, mpl::list<T, Ts...>
	>;
	
	using type = typename helper::type;
};

}

template <class Parser, class Seq>
struct parse_array
{
	using state       = detail::array_state<mpl::size_t<0>, mpl::list<>, mpl::list<>>;
	using helper      = detail::parse_array_helper<Parser, state, Seq>;
	using final_state = typename helper::type;
	using final_lists = typename final_state::lists;
	using type        = mpl::front<final_lists>;
};

}

#endif
