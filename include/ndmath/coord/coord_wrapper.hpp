/*
** File Name: coord_wrapper.hpp
** Author:    Aditya Ramesh
** Date:      01/08/2015
** Contact:   _@adityaramesh.com
*/

#ifndef ZFF60AFF1_88C6_4555_A018_43F5FCE35128
#define ZFF60AFF1_88C6_4555_A018_43F5FCE35128

#include <utility>
#include <ndmath/common.hpp>

namespace nd {

template <class T>
class coord_wrapper final
{
public:
	static constexpr auto allows_static_access =
	T::allows_static_access;

	static constexpr auto is_constant =
	T::is_constant;

	using integer = std::conditional_t<
		std::is_same<typename T::integer, void>::value,
		int, typename T::integer
	>;
private:
	T m_wrapped;
public:
	CC_ALWAYS_INLINE CC_CONST constexpr
	explicit coord_wrapper()
	noexcept : m_wrapped{} {}

	template <class... Args>
	CC_ALWAYS_INLINE constexpr 
	explicit coord_wrapper(in_place_t, Args&&... args)
	noexcept : m_wrapped(std::forward<Args>(args)...) {}

	template <class Integer, nd_enable_if(
		!allows_static_access &&
		std::is_integral<Integer>::value
	)>
	CC_ALWAYS_INLINE
	auto& operator=(const Integer rhs) noexcept
	{
		m_wrapped.value() = rhs;
		return *this;
	}

	template <class U, nd_enable_if(!allows_static_access)>
	CC_ALWAYS_INLINE
	auto& operator=(const coord_wrapper<U>& rhs) noexcept
	{
		m_wrapped.value() = rhs.value();
		return *this;
	}

	CC_ALWAYS_INLINE CC_CONST constexpr
	auto wrapped() const noexcept
	{ return m_wrapped; }

	template <class Integer = uint_fast32_t, nd_enable_if(
		allows_static_access)>
	CC_ALWAYS_INLINE constexpr
	static auto value(const Integer n = 0) noexcept ->
	decltype(std::declval<const T>().value(n))
	{ return T::value(n); }

	template <class Integer = uint_fast32_t, nd_enable_if(
		!allows_static_access)>
	CC_ALWAYS_INLINE
	auto value(const Integer n = 0) noexcept ->
	decltype(std::declval<T>().value(n))
	{ return m_wrapped.value(n); }

	template <class Integer = uint_fast32_t, nd_enable_if(
		!allows_static_access)>
	CC_ALWAYS_INLINE constexpr
	auto value(const Integer n = 0) const noexcept ->
	decltype(std::declval<const T>().value(n))
	{ return m_wrapped.value(n); }

	template <nd_enable_if(is_constant)>
	CC_ALWAYS_INLINE constexpr
	operator integer() const noexcept
	{ return value(); }
};

#define nd_define_relational_op(symbol)    \
                                           \
template <class Coord1, class Coord2>      \
CC_ALWAYS_INLINE constexpr                 \
auto operator symbol (                     \
	const coord_wrapper<Coord1>& lhs,  \
	const coord_wrapper<Coord2>& rhs   \
) noexcept                                 \
{ return lhs.value() symbol rhs.value(); }

nd_define_relational_op(==)
nd_define_relational_op(!=)
nd_define_relational_op(>=)
nd_define_relational_op(>)
nd_define_relational_op(<=)
nd_define_relational_op(<)

#undef nd_define_relational_op

}

#endif
