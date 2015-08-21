/*
** File Name: elemwise_view.hpp
** Author:    Aditya Ramesh
** Date:      08/15/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z485491EA_9715_4B7F_973C_58E0EA5942C8
#define Z485491EA_9715_4B7F_973C_58E0EA5942C8

#include <ndmath/array/zip_with_iterator.hpp>
#include <ndmath/utility/fusion.hpp>
#include <ndmath/utility/named_operator.hpp>

namespace nd {
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
	static constexpr auto supported_by_underlying_type(std::decay_t<Us>*...) ->
	decltype(
		std::declval<Func>()(
			*std::declval<std::decay_t<Us>>().underlying_view().begin()...
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

	using value_type = std::decay_t<
		std::result_of_t<Func(typename std::decay_t<Ts>::reference...)>
	>;

	using size_type  = std::common_type_t<typename std::decay_t<Ts>::size_type...>;
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
		// XXX: For now, we assume that if all arrays have the same
		// external type (e.g. `bool`) and underlying type (e.g.
		// `unsigned`), then it's safe to evaluate the binary operation
		// directly over the underlying type.
		mpl::is_same<std::decay_t<typename std::decay_t<Ts>::external_type>...>::value   &&
		mpl::is_same<std::decay_t<typename std::decay_t<Ts>::underlying_type>...>::value &&
		detail::storage_orders_same<Ts...>                                               &&
		mpl::and_c<std::decay_t<Ts>::provides_underlying_view...>::value                 &&
		// Ensure that `m_func` can actually be applied to the
		// underlying types. The purpose of the parameter pack expansion
		// is to generate a number of zeros equal to `sizeof...(Ts)`.
		supported_by_underlying_type<Ts...>((0, std::is_same<Ts, Ts>::value)...)
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
		// XXX: For now, we assume that if all arrays have the same
		// external type (e.g. `bool`) and underlying type (e.g.
		// `unsigned`), then it's safe to evaluate the binary operation
		// directly over the underlying type.
		mpl::is_same<std::decay_t<typename std::decay_t<Ts>::external_type>...>::value   &&
		mpl::is_same<std::decay_t<typename std::decay_t<Ts>::underlying_type>...>::value &&
		detail::storage_orders_same<Ts...>                                               &&
		mpl::and_c<std::decay_t<Ts>::provides_underlying_view...>::value                 &&
		// Ensure that `m_func` can actually be applied to the
		// underlying types. The purpose of the parameter pack expansion
		// is to generate a number of zeros equal to `sizeof...(Ts)`.
		supported_by_underlying_type<Ts...>((0, std::is_same<Ts, Ts>::value)...)
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
	using expr = elemwise_view<Func, Ts&...>;
	using array_type = array_wrapper<expr>;
	return array_type{expr{f, ts...}};
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

// Relational expressions. TODO only use these when the arrays are both
// binary_comparison_expr types, or have external type bool.
//
//nd_define_binary_expr(==, detail::equal_to)
//nd_define_binary_expr(!=, detail::not_equal_to)
//nd_define_binary_expr(>, detail::greater)
//nd_define_binary_expr(<, detail::less)
//nd_define_binary_expr(>=, detail::greater_equal)
//nd_define_binary_expr(<=, detail::less_equal)

// Logical expressions.
nd_define_binary_expr(&&, detail::logical_and)
nd_define_binary_expr(||, detail::logical_or)

// Bitwise expressions.
nd_define_binary_expr(&, detail::bit_and)
nd_define_binary_expr(|, detail::bit_or)
nd_define_binary_expr(^, detail::bit_xor)
nd_define_binary_expr(<<, detail::left_shift)
nd_define_binary_expr(>>, detail::right_shift)

#undef nd_define_elemwise_view

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

}

template <class T>
CC_ALWAYS_INLINE constexpr
auto fast_not(const array_wrapper<T>& t) noexcept
{ return zip_with(detail::fast_not{}, t); }

static constexpr auto fast_and = make_named_operator(detail::fast_and_helper{});
static constexpr auto fast_or = make_named_operator(detail::fast_or_helper{});

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
