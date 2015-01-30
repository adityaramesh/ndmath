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

	CC_ALWAYS_INLINE CC_CONST constexpr
	auto wrapped() const noexcept
	{ return m_wrapped; }

	template <class Integer = uint_fast32_t, nd_enable_if(
		allows_static_access)>
	CC_ALWAYS_INLINE CC_CONST constexpr
	static auto value(const Integer n = 0) noexcept ->
	decltype(std::declval<T>().value(n))
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
	decltype(std::declval<T>().value(n))
	{ return m_wrapped.value(n); }
};

}

#endif
