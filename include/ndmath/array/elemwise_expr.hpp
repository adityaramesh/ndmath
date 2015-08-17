/*
** File Name: elemwise_expr.hpp
** Author:    Aditya Ramesh
** Date:      08/15/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z485491EA_9715_4B7F_973C_58E0EA5942C8
#define Z485491EA_9715_4B7F_973C_58E0EA5942C8

#include <ndmath/array/zip_with_iterator.hpp>

namespace nd {

template <class T, class U, class Func>
struct elemwise_expr final
{
private:
	using et_1        = std::decay_t<typename T::external_type>;
	using et_2        = std::decay_t<typename U::external_type>;
	using size_type_1 = typename T::size_type;
	using size_type_2 = typename U::size_type;
	using extents_1   = std::decay_t<decltype(std::declval<T>().extents())>;
	using extents_2   = std::decay_t<decltype(std::declval<U>().extents())>;

	static constexpr auto sa_1 = extents_1::allows_static_access;
	static constexpr auto sa_2 = extents_2::allows_static_access;
public:
	using external_type = std::result_of_t<Func(et_1, et_2)>;
	using size_type     = std::common_type_t<size_type_1, size_type_2>;
	using value_type    = std::decay_t<external_type>;
	static constexpr auto is_lazy = true;
private:
	const T& m_left;
	const U& m_right;
	const Func& m_func;
public:
	CC_ALWAYS_INLINE constexpr
	explicit elemwise_expr(const T& t, const U& u, const Func& f)
	noexcept : m_left{t}, m_right{u}, m_func{f} {}

	CC_ALWAYS_INLINE constexpr
	auto memory_size() const noexcept
	{ return size_type{}; }

	template <class... Ts>
	CC_ALWAYS_INLINE constexpr
	decltype(auto) at(const Ts... ts) const noexcept
	{ return m_func(m_left(ts...), m_right(ts...)); }

	template <nd_enable_if((
		// XXX: For now, we assume that if both arrays have the same
		// external type and underlying type, then it's safe to evaluate
		// the binary operation over the underlying type, if possible.
		std::is_same<
			std::decay_t<typename T::external_type>,
			std::decay_t<typename U::external_type>
		>::value &&
		std::is_same<
			typename T::underlying_type,
			typename U::underlying_type
		>::value &&
		decltype(std::declval<T>().storage_order()){} ==
		decltype(std::declval<U>().storage_order()){} &&
		T::provides_underlying_view &&
		U::provides_underlying_view &&
		// Ensure that `m_func` can actually be applied to the
		// underlying types.
		std::is_same<
			std::result_of_t<Func(
				typename T::underlying_type,
				typename U::underlying_type
			)>,
			std::result_of_t<Func(
				typename T::underlying_type,
				typename U::underlying_type
			)>
		>::value
	))>
	CC_ALWAYS_INLINE constexpr
	decltype(auto) underlying_view() const noexcept
	{ return zip_with(m_left.underlying_view(), m_right.underlying_view(), m_func); }

	template <nd_enable_if((
		decltype(std::declval<T>().storage_order()){} ==
		decltype(std::declval<U>().storage_order()){} &&
		T::provides_fast_flat_view &&
		U::provides_fast_flat_view
	))>
	CC_ALWAYS_INLINE constexpr
	decltype(auto) flat_view() const noexcept
	{ return zip_with(m_left.flat_view(), m_right.flat_view(), m_func); }

	CC_ALWAYS_INLINE constexpr
	decltype(auto) storage_order() const noexcept
	{ return m_left.storage_order(); }

	template <nd_enable_if(sa_1)>
	CC_ALWAYS_INLINE constexpr
	decltype(auto) extents() const noexcept
	{ return m_left.extents(); }

	template <nd_enable_if((!sa_1 && sa_2))>
	CC_ALWAYS_INLINE constexpr
	decltype(auto) extents() const noexcept
	{ return m_right.extents(); }

	template <nd_enable_if((!sa_1 && !sa_2))>
	CC_ALWAYS_INLINE
	decltype(auto) extents() const noexcept
	{ return m_left.extents(); }
};

template <class T, class U, class Func, nd_enable_if((
	// Check that the binary operation over the external types of the arrays
	// actually compiles, so that we don't get a cryptic compiler error
	// later down the line.
	std::is_same<
		std::result_of_t<Func(
			typename T::external_type,
			typename U::external_type
		)>,
		std::result_of_t<Func(
			typename T::external_type,
			typename U::external_type
		)>
	>::value
))>
CC_ALWAYS_INLINE constexpr
auto make_elemwise_expr(
	const array_wrapper<T>& t,
	const array_wrapper<U>& u,
	const Func& f
) noexcept
{
	using expr = elemwise_expr<array_wrapper<T>, array_wrapper<U>, Func>;
	using array_type = array_wrapper<expr>;
	return array_type{expr{t, u, f}};
}

namespace detail {

#define nd_define_binary_op(symbol, name)                     \
	struct name final                                     \
	{                                                     \
		template <class T, class U>                   \
		CC_ALWAYS_INLINE constexpr                    \
		auto operator()(const T& t, const U& u) const \
		noexcept { return t symbol u; }               \
	};

// Arithmetic operations.
nd_define_binary_op(+, plus)
nd_define_binary_op(-, minus)
nd_define_binary_op(*, multiplies)
nd_define_binary_op(/, divides)
nd_define_binary_op(%, modulus)

// Relational operations.
nd_define_binary_op(==, equal_to)
nd_define_binary_op(!=, not_equal_to)
nd_define_binary_op(>, greater)
nd_define_binary_op(<, less)
nd_define_binary_op(>=, greater_equal)
nd_define_binary_op(<=, less_equal)

// Bitwise operations.
nd_define_binary_op(&, bit_and)
nd_define_binary_op(|, bit_or)
nd_define_binary_op(^, bit_xor)
nd_define_binary_op(<<, left_shift)
nd_define_binary_op(>>, right_shift)

#undef nd_define_binary_op

struct logical_and final
{
	CC_ALWAYS_INLINE constexpr
	auto operator()(const bool& t, const bool& u) const
	noexcept { return t && u; }

	template <class T, class U>
	CC_ALWAYS_INLINE constexpr
	auto operator()(const T& t, const U& u) const
	noexcept { return t & u; }
};

struct logical_or final
{
	CC_ALWAYS_INLINE constexpr
	auto operator()(const bool& t, const bool& u) const
	noexcept { return t || u; }

	template <class T, class U>
	CC_ALWAYS_INLINE constexpr
	auto operator()(const T& t, const U& u) const
	noexcept { return t | u; }
};

}

#define nd_define_elemwise_expr(symbol, name)                                       \
	template <class T, class U>                                                 \
	CC_ALWAYS_INLINE constexpr                                                  \
	auto operator symbol (const array_wrapper<T>& t, const array_wrapper<U>& u) \
	noexcept { return make_elemwise_expr(t, u, name{}); }

nd_define_elemwise_expr(+, detail::plus)
nd_define_elemwise_expr(-, detail::minus)
nd_define_elemwise_expr(*, detail::multiplies)
nd_define_elemwise_expr(/, detail::divides)
nd_define_elemwise_expr(%, detail::modulus)

// Relational operations. TODO only use these when the arrays are both
// elemwise_comparison_expr types.
//
//nd_define_elemwise_expr(==, detail::equal_to)
//nd_define_elemwise_expr(!=, detail::not_equal_to)
//nd_define_elemwise_expr(>, detail::greater)
//nd_define_elemwise_expr(<, detail::less)
//nd_define_elemwise_expr(>=, detail::greater_equal)
//nd_define_elemwise_expr(<=, detail::less_equal)

// Bitwise operations.
nd_define_elemwise_expr(&, detail::bit_and)
nd_define_elemwise_expr(|, detail::bit_or)
nd_define_elemwise_expr(^, detail::bit_xor)
nd_define_elemwise_expr(<<, detail::left_shift)
nd_define_elemwise_expr(>>, detail::right_shift)

#undef nd_define_elemwise_expr

}

#endif
