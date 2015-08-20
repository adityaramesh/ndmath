/*
** File Name: elemwise_expr.hpp
** Author:    Aditya Ramesh
** Date:      08/15/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z485491EA_9715_4B7F_973C_58E0EA5942C8
#define Z485491EA_9715_4B7F_973C_58E0EA5942C8

#include <ndmath/array/zip_with_iterator.hpp>
#include <ndmath/utility/fusion.hpp>

namespace nd {
namespace detail {

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
struct elemwise_expr final
{
private:
	static_assert(
		mpl::and_c<std::is_lvalue_reference<Ts>::value...>::value,
		"Expression can only involve reference types."
	);

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
		std::result_of_t<Func(typename std::decay_t<Ts>::reference...)>
	>;

	using size_type  = std::common_type_t<typename std::decay_t<Ts>::size_type...>;
	using value_type = external_type;
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
	explicit elemwise_expr(const Func& f, Ts... ts)
	noexcept : m_refs{ts...}, m_func{f}
	{
		#ifndef nd_no_debug
			const auto& h = front(ts...);

			for_each(erase_front(m_refs), [&] (const auto& x)
				CC_ALWAYS_INLINE noexcept {
					nd_assert(
						h.extents() == x.extents(),
						"mismatching extents.\n▶ $ ≠ $",
						h.extents(), x.extents()
					);
				});
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
		// underlying types.
		mpl::is_same<
			std::result_of_t<Func(typename std::decay_t<Ts>::underlying_type...)>,
			std::result_of_t<Func(typename std::decay_t<Ts>::underlying_type...)>
		>::value
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
		// underlying types.
		mpl::is_same<
			std::result_of_t<Func(typename std::decay_t<Ts>::underlying_type...)>,
			std::result_of_t<Func(typename std::decay_t<Ts>::underlying_type...)>
		>::value
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
		std::result_of_t<Func(typename std::decay_t<Ts>::reference...)>,
		std::result_of_t<Func(typename std::decay_t<Ts>::reference...)>
	>::value
))>
CC_ALWAYS_INLINE constexpr
auto zip_with(const Func& f, Ts&... ts) noexcept
{
	using expr = elemwise_expr<Func, Ts&...>;
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
auto make_elemwise_expr(Ts&... ts) noexcept
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

#define nd_define_elemwise_expr(symbol, name)                                       \
	template <class T, class U>                                                 \
	CC_ALWAYS_INLINE constexpr                                                  \
	auto operator symbol (const array_wrapper<T>& t, const array_wrapper<U>& u) \
	noexcept { return zip_with(name{}, t, u); }

// Arithmetic expressions.
nd_define_elemwise_expr(+, detail::plus)
nd_define_elemwise_expr(-, detail::minus)
nd_define_elemwise_expr(*, detail::multiplies)
nd_define_elemwise_expr(/, detail::divides)
nd_define_elemwise_expr(%, detail::modulus)

// Relational expressions. TODO only use these when the arrays are both
// elemwise_comparison_expr types, or have external type bool.
//
//nd_define_elemwise_expr(==, detail::equal_to)
//nd_define_elemwise_expr(!=, detail::not_equal_to)
//nd_define_elemwise_expr(>, detail::greater)
//nd_define_elemwise_expr(<, detail::less)
//nd_define_elemwise_expr(>=, detail::greater_equal)
//nd_define_elemwise_expr(<=, detail::less_equal)

// Bitwise expressions.
nd_define_elemwise_expr(&, detail::bit_and)
nd_define_elemwise_expr(|, detail::bit_or)
nd_define_elemwise_expr(^, detail::bit_xor)
nd_define_elemwise_expr(<<, detail::left_shift)
nd_define_elemwise_expr(>>, detail::right_shift)

#undef nd_define_elemwise_expr

}

#endif
