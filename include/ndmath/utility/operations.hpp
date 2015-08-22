/*
** File Name: operations.hpp
** Author:    Aditya Ramesh
** Date:      01/08/2015
** Contact:   _@adityaramesh.com
**
** TODO: unary operations.
*/

#ifndef Z0801DF89_4E7C_4FA7_83F3_A43D34D22E18
#define Z0801DF89_4E7C_4FA7_83F3_A43D34D22E18

namespace nd {
namespace detail {

struct unary_plus final
{
	template <class T>
	CC_ALWAYS_INLINE constexpr
	auto operator()(const T& t)
	const noexcept { return +t; }
};

struct unary_minus final
{
	template <class T>
	CC_ALWAYS_INLINE constexpr
	auto operator()(const T& t)
	const noexcept { return -t; }
};

struct bit_not final
{
	template <class T>
	CC_ALWAYS_INLINE constexpr
	auto operator()(const T& t)
	const noexcept { return ~t; }
};

#define nd_define_binary_op(symbol, name)                                             \
	struct name final                                                             \
	{                                                                             \
		template <class T, class U, nd_enable_if((                            \
			std::is_same<                                                 \
				decltype(std::declval<T>() symbol std::declval<U>()), \
				decltype(std::declval<T>() symbol std::declval<U>())  \
			>::value                                                      \
		))>                                                                   \
		CC_ALWAYS_INLINE constexpr                                            \
		auto operator()(const T& u, const U& v)                               \
		const noexcept { return u symbol v; }                                 \
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

template <bool IsIntegral>
struct logical_op_helper;

template <>
struct logical_op_helper<true>
{
	template <class T>
	CC_ALWAYS_INLINE constexpr
	static auto logical_not(const T& t)
	noexcept { return ~t; }

	template <class T, class U>
	CC_ALWAYS_INLINE constexpr
	static auto logical_and(const T& t, const U& u)
	noexcept { return t & u; }

	template <class T, class U>
	CC_ALWAYS_INLINE constexpr
	static auto logical_or(const T& t, const U& u)
	noexcept { return t | u; }
};

template <>
struct logical_op_helper<false>
{
	template <class T>
	CC_ALWAYS_INLINE constexpr
	static auto logical_not(const T& t)
	noexcept { return !t; }

	template <class T, class U>
	CC_ALWAYS_INLINE constexpr
	static auto logical_and(const T& t, const U& u)
	noexcept { return t && u; }

	template <class T, class U>
	CC_ALWAYS_INLINE constexpr
	static auto logical_or(const T& t, const U& u)
	noexcept { return t || u; }
};

struct logical_not final
{
	template <class T>
	CC_ALWAYS_INLINE constexpr
	auto operator()(const T& t) const noexcept
	{
		using helper = logical_op_helper<
			!std::is_same<T, bool>::value &&
			std::is_integral<T>::value
		>;
		return helper::logical_not(t);
	}
};

struct logical_and final
{
	template <class T, class U>
	CC_ALWAYS_INLINE constexpr
	auto operator()(const T& t, const U& u) const noexcept
	{
		using helper = logical_op_helper<
			!std::is_same<T, bool>::value &&
			!std::is_same<U, bool>::value &&
			std::is_integral<T>::value    &&
			std::is_integral<U>::value
		>;
		return helper::logical_and(t, u);
	}
};

struct logical_or final
{
	template <class T, class U>
	CC_ALWAYS_INLINE constexpr
	auto operator()(const T& t, const U& u) const noexcept
	{
		using helper = logical_op_helper<
			!std::is_same<T, bool>::value &&
			!std::is_same<U, bool>::value &&
			std::is_integral<T>::value    &&
			std::is_integral<U>::value
		>;
		return helper::logical_or(t, u);
	}
};

}}

#endif
