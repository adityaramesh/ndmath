/*
** File Name: location_wrapper.hpp
** Author:    Aditya Ramesh
** Date:      01/08/2015
** Contact:   _@adityaramesh.com
*/

#ifndef ZFF60AFF1_88C6_4555_A018_43F5FCE35128
#define ZFF60AFF1_88C6_4555_A018_43F5FCE35128

#include <utility>
#include <ndmath/common.hpp>

namespace nd {

template <class T, bool IsConstant, bool Enable>
struct location_traits;

template <class T, bool IsConstant>
struct location_traits<T, IsConstant, false>
{
	template <class Integer>
	using result = void;

	template <class Integer>
	using const_result = void;
};

template <class T>
struct location_traits<T, true, true>
{
	template <class Integer>
	using result =
	decltype(std::declval<T>().value());

	template <class Integer>
	using const_result =
	decltype(std::declval<const T>().value());
};

template <class T>
struct location_traits<T, false, true>
{
	template <class Integer>
	using result =
	decltype(std::declval<T>().value(Integer{}));

	template <class Integer>
	using const_result =
	decltype(std::declval<const T>().value(Integer{}));
};

template <class T>
class location_wrapper final
{
public:
	static constexpr auto is_constant = T::is_constant;
	static constexpr auto allows_static_access = T::allows_static_access;
private:
	T m_wrapped;
public:
	CC_ALWAYS_INLINE CC_CONST constexpr
	explicit location_wrapper()
	noexcept : m_wrapped{} {}

	template <class... Args>
	CC_ALWAYS_INLINE constexpr 
	explicit location_wrapper(in_place_t, Args&&... args)
	noexcept : m_wrapped(std::forward<Args>(args)...) {}

	CC_ALWAYS_INLINE CC_CONST constexpr
	auto wrapped() const noexcept
	{ return m_wrapped; }

	template <class Integer = uint_fast32_t, nd_enable_if(
		allows_static_access && is_constant
	)>
	CC_ALWAYS_INLINE CC_CONST constexpr
	static auto value(Integer = 0) noexcept ->
	typename location_traits<T, is_constant, allows_static_access && is_constant>::
		template result<Integer>
	{ return T::value(); }

	template <class Integer, nd_enable_if(
		allows_static_access && !is_constant
	)>
	CC_ALWAYS_INLINE CC_CONST constexpr
	static auto value(const Integer n) noexcept ->
	typename location_traits<T, is_constant, allows_static_access && !is_constant>::
		template result<Integer>
	{ return T::value(n); }

	template <class Integer = uint_fast32_t, nd_enable_if(
		!allows_static_access && is_constant
	)>
	CC_ALWAYS_INLINE
	auto value(Integer = 0) noexcept ->
	typename location_traits<T, is_constant, !allows_static_access && is_constant>::
		template result<Integer>
	{ return m_wrapped.value(); }

	template <class Integer = uint_fast32_t, nd_enable_if(
		!allows_static_access && is_constant
	)>
	CC_ALWAYS_INLINE constexpr
	auto value(Integer = 0) const noexcept ->
	typename location_traits<T, is_constant, !allows_static_access && is_constant>::
		template const_result<Integer>
	{ return m_wrapped.value(); }

	template <class Integer, nd_enable_if(
		!allows_static_access && !is_constant
	)>
	CC_ALWAYS_INLINE
	auto value(const Integer n) noexcept ->
	typename location_traits<T, is_constant, !allows_static_access && !is_constant>::
		template result<Integer>
	{ return m_wrapped.value(n); }

	template <class Integer, nd_enable_if(
		!allows_static_access && !is_constant
	)>
	CC_ALWAYS_INLINE constexpr
	auto value(const Integer n) const noexcept ->
	typename location_traits<T, is_constant, !allows_static_access && !is_constant>::
		template const_result<Integer>
	{ return m_wrapped.value(n); }
};

}

#endif
