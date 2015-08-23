/*
** File Name: elemwise_view.hpp
** Author:    Aditya Ramesh
** Date:      08/15/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z485491EA_9715_4B7F_973C_58E0EA5942C8
#define Z485491EA_9715_4B7F_973C_58E0EA5942C8

#include <ndmath/array/boolean_storage.hpp>
#include <ndmath/array/zip_with_iterator.hpp>
#include <ndmath/utility/fusion.hpp>
#include <ndmath/utility/named_operator.hpp>

namespace nd {

template <class Func, class... Ts>
struct elemwise_view;

namespace detail {

struct check_extents_helper
{
	template <class Tuple, nd_enable_if((Tuple::size != 1))>
	CC_ALWAYS_INLINE
	static auto apply(const Tuple& t) noexcept
	{
		const auto& h = nd::get<0>(t);

		nd::for_each(erase_front(t), [&] (const auto& x)
			CC_ALWAYS_INLINE noexcept {
				nd_assert(
					h.extents() == x.extents(),
					"mismatching extents.\n▶ $ ≠ $",
					h.extents(), x.extents()
				);
			});
	}

	template <class Tuple, nd_enable_if((Tuple::size == 1))>
	CC_ALWAYS_INLINE
	static auto apply(Tuple) noexcept {}
};

template <class T, class U>
static constexpr auto storage_orders_same_2 =
decltype(std::declval<T>().storage_order()){} ==
decltype(std::declval<U>().storage_order()){};

template <bool V, class T, class... Ts>
struct storage_orders_same_helper;

template <bool V, class T, class U, class... Us>
struct storage_orders_same_helper<V, T, U, Us...> :
storage_orders_same_helper<V && storage_orders_same_2<T, U>, T, Us...> {};

template <bool V, class T>
struct storage_orders_same_helper<V, T>
{ static constexpr auto value = V; };

template <class... Ts>
static constexpr auto storage_orders_same =
storage_orders_same_helper<true, Ts...>::value;

template <class T>
using allows_static_access = mpl::bool_<T::allows_static_access>;

template <class T>
struct remove_rvalue_reference
{ using type = T; };

template <class T>
struct remove_rvalue_reference<T&&>
{ using type = T; };

template <class T>
using remove_rvalue_reference_t = typename remove_rvalue_reference<T>::type;

struct elemwise_comp {};

template <class T>
struct is_elemwise_comp_expr_helper
: std::false_type {};

template <class T>
struct is_elemwise_comp_expr_helper<array_wrapper<
	elemwise_view<elemwise_comp, T>
>> : std::true_type {};

template <class T>
static constexpr auto is_elemwise_comp_expr =
is_elemwise_comp_expr_helper<std::decay_t<T>>::value;

template <class T, nd_enable_if((is_elemwise_comp_expr<T>))>
CC_ALWAYS_INLINE
constexpr decltype(auto) unwrap_elemwise_comp_expr(T& t) noexcept
{ return t.wrapped().argument(); }

template <class T, nd_enable_if((!is_elemwise_comp_expr<T>))>
CC_ALWAYS_INLINE
constexpr decltype(auto)
unwrap_elemwise_comp_expr(T& t) noexcept
{ return t; }

}

template <class Func, class... Ts>
struct elemwise_view final
{
private:
	static_assert(
		mpl::and_c<std::is_lvalue_reference<Ts>::value...>::value,
		"Expression can only involve reference types."
	);

	template <class... Us>
	static constexpr auto supported_by_underlying_type(void*) ->
	decltype(
		std::declval<Func>()(
			*std::declval<Us>().underlying_view().begin()...
		),
		bool{}
	) { return true; }

	template <class... Us>
	static constexpr auto supported_by_underlying_type(...)
	{ return false; }

	using extents_list = mpl::list<std::decay_t<decltype(std::declval<Ts>().extents())>...>;
	using access_list  = mpl::transform<extents_list, mpl::quote<detail::allows_static_access>>;
	using match        = mpl::find<mpl::true_, access_list>;

	/*
	** Prevent a compiler error in case `match == mpl::no_match` when we use
	** `nd::get<N>()` later. If none of the arrays in `Ts` has statically
	** accessible extents, then we might as well return the extents of the
	** first array.
	*/
	using index = mpl::if_<
		mpl::is_same<match, mpl::no_match>,
		mpl::list_index_c<0>, match
	>;
public:
	using external_type = std::decay_t<
		std::result_of_t<Func(typename std::decay_t<Ts>::external_type...)>
	>;

	using size_type = std::common_type_t<typename std::decay_t<Ts>::size_type...>;
	static constexpr auto is_lazy = true;
private:
	/*
	** There's a totally bizarre bug that manifests when using `std::tuple`
	** and compiling under clang-3.6. When initializing `m_refs` in the
	** constructor's initializer list, the compiler produces an error.
	** Analyzing the error shows that the arguments to the constructor of
	** `tuple` seem to be redundantly nested in a `tuple` themselves. So
	** when the constructor for `tuple(Us...)` is called, one of the
	** constructibility tests in the `enable_if` specification causes a
	** compiler error. Strangely, constructing a tuple of the exact same
	** type using `std::make_tuple(std::cref(ts)...)` in the body of the
	** constructor works without any problems.
	**
	** Note: I think the above bug is due to the "too perfect forwarding"
	** issue described by Eric Niebler, Scott Meyers, etc. It doesn't
	** manifest in my implementation of tuple because the perfect forwarding
	** constructor is only enabled in the case that both parameter packs
	** have the same size.
	**
	** Compiling under clang-3.7 causes an ICE. Again, I wasn't able to
	** produce an MWE.
	**
	** As a workaround, I'm using a custom tuple implementation.
	*/
	tuple<Ts...> m_refs;
	const Func& m_func;
public:
	CC_ALWAYS_INLINE
	explicit elemwise_view(const Func& f, Ts... ts)
	noexcept : m_refs{ts...}, m_func{f}
	{
		#ifndef nd_no_debug
			using helper = detail::check_extents_helper;
			helper::apply(m_refs);
		#endif
	}

	CC_ALWAYS_INLINE constexpr
	auto memory_size() const noexcept
	{ return size_type{}; }

	template <class... Us>
	CC_ALWAYS_INLINE
	decltype(auto) at(const Us&... us) noexcept
	{
		return expand(m_refs,
			[&] (auto&... ts) CC_ALWAYS_INLINE noexcept {
				return m_func(ts(us...)...);
			}
		);
	}

	template <class... Us>
	CC_ALWAYS_INLINE constexpr
	decltype(auto) at(const Us&... us) const noexcept
	{
		return expand(m_refs,
			[&] (const auto&... ts) CC_ALWAYS_INLINE noexcept {
				return m_func(ts(us...)...);
			}
		);
	}

	template <nd_enable_if((
		detail::storage_orders_same<Ts...> &&
		mpl::and_c<std::decay_t<Ts>::provides_underlying_view...>::value &&
		supported_by_underlying_type<Ts...>(0)
	))>
	CC_ALWAYS_INLINE
	decltype(auto) underlying_view() noexcept
	{
		return expand(m_refs,
			[&] (auto&... ts) CC_ALWAYS_INLINE noexcept {
				return zip_with(m_func, ts.underlying_view()...);
			});
	}

	template <nd_enable_if((
		detail::storage_orders_same<Ts...> &&
		mpl::and_c<std::decay_t<Ts>::provides_underlying_view...>::value &&
		supported_by_underlying_type<Ts...>(0)
	))>
	CC_ALWAYS_INLINE constexpr
	decltype(auto) underlying_view() const noexcept
	{
		return expand(m_refs,
			[&] (const auto&... ts) CC_ALWAYS_INLINE noexcept {
				return zip_with(m_func, ts.underlying_view()...);
			});
	}

	template <nd_enable_if((
		detail::storage_orders_same<Ts...> &&
		mpl::and_c<std::decay_t<Ts>::provides_fast_flat_view...>::value
	))>
	CC_ALWAYS_INLINE
	decltype(auto) flat_view() noexcept
	{
		return expand(m_refs,
			[&] (auto&... ts) CC_ALWAYS_INLINE noexcept {
				return zip_with(m_func, ts.flat_view()...);
			});
	}

	template <nd_enable_if((
		detail::storage_orders_same<Ts...> &&
		mpl::and_c<std::decay_t<Ts>::provides_fast_flat_view...>::value
	))>
	CC_ALWAYS_INLINE constexpr
	decltype(auto) flat_view() const noexcept
	{
		return expand(m_refs,
			[&] (const auto&... ts) CC_ALWAYS_INLINE noexcept {
				return zip_with(m_func, ts.flat_view()...);
			});
	}

	CC_ALWAYS_INLINE constexpr
	decltype(auto) storage_order() const noexcept
	{ return get<0>(m_refs).storage_order(); }

	CC_ALWAYS_INLINE
	decltype(auto) extents() const noexcept
	{ return get<index::value>(m_refs).extents(); }
};

template <class T>
struct elemwise_view<detail::elemwise_comp, T> final
{
	static_assert(
		std::is_lvalue_reference<T>::value,
		"Expression can only involve reference types."
	);

	using external_type = typename std::decay_t<T>::external_type;
	using size_type     = typename std::decay_t<T>::size_type;
	static constexpr auto is_lazy = true;
private:
	T m_ref;
public:
	CC_ALWAYS_INLINE
	explicit elemwise_view(T t)
	noexcept : m_ref{t} {}

	CC_ALWAYS_INLINE constexpr
	auto memory_size() const noexcept
	{ return size_type{}; }

	template <class... Us>
	CC_ALWAYS_INLINE
	decltype(auto) at(const Us&... us) noexcept
	{ return m_ref(us...); }

	template <class... Us>
	CC_ALWAYS_INLINE constexpr
	decltype(auto) at(const Us&... us) const noexcept
	{ return m_ref(us...); }

	template <nd_enable_if((std::decay_t<T>::provides_underlying_view))>
	CC_ALWAYS_INLINE
	decltype(auto) underlying_view() noexcept
	{ return m_ref.underlying_view(); }

	template <nd_enable_if((std::decay_t<T>::provides_underlying_view))>
	CC_ALWAYS_INLINE constexpr
	decltype(auto) underlying_view() const noexcept
	{ return m_ref.underlying_view(); }

	template <nd_enable_if((std::decay_t<T>::provides_fast_flat_view))>
	CC_ALWAYS_INLINE
	decltype(auto) flat_view() noexcept
	{ return m_ref.flat_view(); }

	template <nd_enable_if((std::decay_t<T>::provides_fast_flat_view))>
	CC_ALWAYS_INLINE constexpr
	decltype(auto) flat_view() const noexcept
	{ return m_ref.flat_view(); }

	CC_ALWAYS_INLINE constexpr
	decltype(auto) storage_order() const noexcept
	{ return m_ref.storage_order(); }

	CC_ALWAYS_INLINE
	decltype(auto) extents() const noexcept
	{ return m_ref.extents(); }

	CC_ALWAYS_INLINE
	decltype(auto) argument() noexcept
	{ return m_ref; }

	CC_ALWAYS_INLINE
	decltype(auto) argument() const noexcept
	{ return m_ref; }
};

template <class Func, class... Ts, nd_enable_if((
	mpl::and_c<
		mpl::is_specialization_of<array_wrapper, std::decay_t<Ts>>::value...
	>::value &&
	// Ensure that the function can actually be applied to the types of the
	// arrays, so that we don't get a cryptic error message later down the
	// line.
	std::is_same<
		std::result_of_t<Func(typename std::decay_t<Ts>::external_type...)>,
		std::result_of_t<Func(typename std::decay_t<Ts>::external_type...)>
	>::value
))>
CC_ALWAYS_INLINE constexpr
auto zip_with(const Func& f, Ts&... ts) noexcept
{
	using view = elemwise_view<Func, Ts&...>;
	using array_type = array_wrapper<view>;
	return array_type{view{f, ts...}};
}

template <class T>
CC_ALWAYS_INLINE constexpr
auto use_elemwise_comp(const array_wrapper<T>& t) noexcept
{
	using func       = detail::elemwise_comp;
	using view       = elemwise_view<func, const array_wrapper<T>&>;
	using array_type = array_wrapper<view>;
	return array_type{view{t}};
}

template <class... Ts, nd_enable_if((
	mpl::and_c<
		mpl::is_specialization_of<array_wrapper, std::decay_t<Ts>>::value...
	>::value
))>
CC_ALWAYS_INLINE constexpr
auto zip(Ts&... ts) noexcept
{
	return zip_with([] (auto&&... us) CC_ALWAYS_INLINE noexcept {
		using tuple = nd::tuple<detail::remove_rvalue_reference_t<decltype(us)>...>;
		return tuple{us...};
	}, ts...);
}

/*
** Exactly the same as `zip_with`, except that the function is the last argument
** instead of the first. This function may be more natural to use in certain
** contexts.
*/
template <class... Ts, nd_enable_if((sizeof...(Ts) >= 2))>
CC_ALWAYS_INLINE constexpr
auto make_elemwise_view(Ts&... ts) noexcept
{
	/*
	** The last parameter to this function should be the nary function used
	** invoked upon each element access; the parameters that appear before
	** it are the arrays involved in the expression. (This is to make the
	** syntax more natural for the user.) We need to invoke the helper
	** function with the last parameter appearing before the others.
	*/

	using list = mpl::to_values<mpl::prepend<
		mpl::size_t<sizeof...(Ts) - 1>,
		mpl::range_c<size_t, 0, sizeof...(Ts) - 2>
	>>;

	return invoke_with<list>([&] (auto&... us) CC_ALWAYS_INLINE noexcept
		{ return zip_with(us...); }, ts...);
}

#define nd_define_unary_expr(symbol, name)               \
	template <class T>                               \
	CC_ALWAYS_INLINE constexpr                       \
	auto operator symbol (const array_wrapper<T>& t) \
	noexcept { return zip_with(name{}, t); }

nd_define_unary_expr(+, detail::unary_plus)
nd_define_unary_expr(-, detail::unary_minus)
nd_define_unary_expr(!, detail::logical_not)
nd_define_unary_expr(~, detail::bit_not)

#undef nd_define_unary_expr

#define nd_define_binary_expr(symbol, name)                                         \
	template <class T, class U>                                                 \
	CC_ALWAYS_INLINE constexpr                                                  \
	auto operator symbol (const array_wrapper<T>& t, const array_wrapper<U>& u) \
	noexcept { return zip_with(name{}, t, u); }

// Arithmetic expressions.
nd_define_binary_expr(+, detail::plus)
nd_define_binary_expr(-, detail::minus)
nd_define_binary_expr(*, detail::multiplies)
nd_define_binary_expr(/, detail::divides)
nd_define_binary_expr(%, detail::modulus)

// Logical expressions.
nd_define_binary_expr(&&, detail::logical_and)
nd_define_binary_expr(||, detail::logical_or)

// Bitwise expressions.
nd_define_binary_expr(&, detail::bit_and)
nd_define_binary_expr(|, detail::bit_or)
nd_define_binary_expr(^, detail::bit_xor)
nd_define_binary_expr(<<, detail::left_shift)
nd_define_binary_expr(>>, detail::right_shift)

#undef nd_define_binary_expr

#define nd_define_relational_expr(symbol, name)                                   \
	template <class T, class U, nd_enable_if((                                \
		detail::is_elemwise_comp_expr<array_wrapper<T>> ||                \
		detail::is_elemwise_comp_expr<array_wrapper<U>>                   \
	))>                                                                       \
	CC_ALWAYS_INLINE                                                          \
	constexpr auto                                                            \
	operator symbol (const array_wrapper<T>& t, const array_wrapper<U>& u)    \
	noexcept                                                                  \
	{                                                                         \
		return zip_with(                                                  \
			name{},                                                   \
			detail::unwrap_elemwise_comp_expr(t),                     \
			detail::unwrap_elemwise_comp_expr(u)                      \
		);                                                                \
	}                                                                         \
	                                                                          \
	template <class T, class U, nd_enable_if(!(                               \
		detail::is_elemwise_comp_expr<array_wrapper<T>> ||                \
		detail::is_elemwise_comp_expr<array_wrapper<U>>                   \
	))>                                                                       \
	CC_ALWAYS_INLINE                                                          \
	constexpr auto                                                            \
	operator symbol (const array_wrapper<T>& t, const array_wrapper<U>& u)    \
	noexcept                                                                  \
	{                                                                         \
		using helper = detail::relational_operation_helper;               \
		return helper::apply(t, u, name{});                               \
	}

nd_define_relational_expr(==, detail::equal_to)
nd_define_relational_expr(>, detail::greater)
nd_define_relational_expr(<, detail::less)
nd_define_relational_expr(>=, detail::greater_equal)
nd_define_relational_expr(<=, detail::less_equal)

#undef nd_define_relational_expr

/*
** We need to define the functions for "not equal to" separately, because the
** version that returns a scalar cannot be implemented in quite the same way as
** the others.
*/

template <class T, class U, nd_enable_if((
	detail::is_elemwise_comp_expr<array_wrapper<T>> ||
	detail::is_elemwise_comp_expr<array_wrapper<U>>
))>
CC_ALWAYS_INLINE
constexpr auto
operator!=(const array_wrapper<T>& t, const array_wrapper<U>& u) noexcept
{
	return zip_with(
		detail::not_equal_to{},
		detail::unwrap_elemwise_comp_expr(t),
		detail::unwrap_elemwise_comp_expr(u)
	);
}

template <class T, class U, nd_enable_if(!(
	detail::is_elemwise_comp_expr<array_wrapper<T>> ||
	detail::is_elemwise_comp_expr<array_wrapper<U>>
))>
CC_ALWAYS_INLINE
constexpr auto
operator!=(const array_wrapper<T>& t, const array_wrapper<U>& u) noexcept
{ return !(t == u); }

namespace detail {

struct fast_and_helper
{
	template <class T, class U>
	CC_ALWAYS_INLINE constexpr
	auto operator()(const array_wrapper<T>& t, const array_wrapper<U>& u)
	const noexcept { return zip_with(detail::fast_and{}, t, u); }
};

struct fast_or_helper
{
	template <class T, class U>
	CC_ALWAYS_INLINE constexpr
	auto operator()(const array_wrapper<T>& t, const array_wrapper<U>& u)
	const noexcept { return zip_with(detail::fast_or{}, t, u); }
};

struct fast_eq_helper
{
	/*
	** This overload is used for elementwise comparison operations.
	*/
	template <class T, class U, nd_enable_if((
		detail::is_elemwise_comp_expr<array_wrapper<T>> ||
		detail::is_elemwise_comp_expr<array_wrapper<U>>
	))>
	CC_ALWAYS_INLINE constexpr
	auto operator()(const array_wrapper<T>& t, const array_wrapper<U>& u)
	const noexcept
	{
		return zip_with(
			detail::fast_eq{},
			unwrap_elemwise_comp_expr(t),
			unwrap_elemwise_comp_expr(u)
		);
	}

	/*
	** This overload is used for comparison operations that output a single
	** boolean value.
	*/
	template <class T, class U, nd_enable_if((!(
		detail::is_elemwise_comp_expr<array_wrapper<T>> ||
		detail::is_elemwise_comp_expr<array_wrapper<U>>
	)))>
	CC_ALWAYS_INLINE
	bool operator()(const array_wrapper<T>& x, const array_wrapper<U>& y)
	const noexcept
	{
		using helper = detail::relational_operation_helper;
		return helper::apply(x, y, detail::fast_eq{});
	}
};

struct fast_neq_helper
{
	template <class T, class U, nd_enable_if((
		detail::is_elemwise_comp_expr<array_wrapper<T>> ||
		detail::is_elemwise_comp_expr<array_wrapper<U>>
	))>
	CC_ALWAYS_INLINE constexpr
	auto operator()(const array_wrapper<T>& t, const array_wrapper<U>& u)
	const noexcept
	{
		return zip_with(
			detail::fast_neq{},
			unwrap_elemwise_comp_expr(t),
			unwrap_elemwise_comp_expr(u)
		);
	}

	template <class T, class U, nd_enable_if((!(
		detail::is_elemwise_comp_expr<array_wrapper<T>> ||
		detail::is_elemwise_comp_expr<array_wrapper<U>>
	)))>
	CC_ALWAYS_INLINE
	bool operator()(const array_wrapper<T>& x, const array_wrapper<U>& y)
	const noexcept
	{
		using helper = detail::relational_operation_helper;
		return helper::apply(x, y, detail::fast_neq{});
	}
};

}

/*
** Named operators for boolean arrays.
*/

template <class T>
CC_ALWAYS_INLINE constexpr
auto fast_not(const array_wrapper<T>& t) noexcept
{ return zip_with(detail::fast_not{}, t); }

static constexpr auto fast_and = make_named_operator(detail::fast_and_helper{});
static constexpr auto fast_or  = make_named_operator(detail::fast_or_helper{});
static constexpr auto fast_eq  = make_named_operator(detail::fast_eq_helper{});
static constexpr auto fast_neq = make_named_operator(detail::fast_neq_helper{});

#define nd_define_reflexive_op(symbol)                                              \
	template <class T, class U>                                                 \
	CC_ALWAYS_INLINE                                                            \
	auto& operator symbol ## = (array_wrapper<T>& t, const array_wrapper<U>& u) \
	noexcept { return t = t symbol u; }

// Arithmetic operations.
nd_define_reflexive_op(+)
nd_define_reflexive_op(-)
nd_define_reflexive_op(*)
nd_define_reflexive_op(/)
nd_define_reflexive_op(%)

// Bitwise operations.
nd_define_reflexive_op(&)
nd_define_reflexive_op(|)
nd_define_reflexive_op(^)
nd_define_reflexive_op(<<)
nd_define_reflexive_op(>>)

#undef nd_define_reflexive_op

}

#endif
