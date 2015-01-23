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
namespace detail {

/*
** We need the `location_traits` bullshit because:
** (1) The return type of the wrapped objects's member function can depend on
** the type of the arguments; and
** (2) auto return types strip the reference from the inferred type (otherwise I
** would have used auto return types).
*/
template <class T, bool IsConstant>
struct location_traits;

template <class T>
struct location_traits<T, true>
{
	template <class Integer>
	using result =
	decltype(std::declval<T>().value());

	template <class Integer>
	using const_result =
	decltype(std::declval<const T>().value());
};

template <class T>
struct location_traits<T, false>
{
	template <class Integer>
	using result =
	decltype(std::declval<T>().value(Integer{}));

	template <class Integer>
	using const_result =
	decltype(std::declval<const T>().value(Integer{}));
};

template <class Integer, class Traits, bool Enable>
struct result_helper;

template <class Integer, class Traits>
struct result_helper<Integer, Traits, false>
{
	using result = void;
	using const_result = void;
};

template <class Integer, class Traits>
struct result_helper<Integer, Traits, true>
{
	using result = typename Traits::template result<Integer>;
	using const_result = typename Traits::template const_result<Integer>;
};

template <class Integer, class Traits, bool Enable>
using result = typename result_helper<Integer, Traits, Enable>::result;

template <class Integer, class Traits, bool Enable>
using const_result = typename result_helper<Integer, Traits, Enable>::const_result;

}

template <class T>
class location_wrapper final
{
public:
	static constexpr auto is_constant = T::is_constant;
	static constexpr auto allows_static_access = T::allows_static_access;
private:
	using traits = detail::location_traits<T, is_constant>;

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
	detail::result<Integer, traits, allows_static_access && is_constant>
	{ return T::value(); }

	template <class Integer, nd_enable_if(
		allows_static_access && !is_constant
	)>
	CC_ALWAYS_INLINE CC_CONST constexpr
	static auto value(const Integer n) noexcept ->
	detail::result<Integer, traits, allows_static_access && !is_constant>
	{ return T::value(n); }

	template <class Integer = uint_fast32_t, nd_enable_if(
		!allows_static_access && is_constant
	)>
	CC_ALWAYS_INLINE
	auto value(Integer = 0) noexcept ->
	detail::result<Integer, traits, !allows_static_access && is_constant>
	{ return m_wrapped.value(); }

	template <class Integer = uint_fast32_t, nd_enable_if(
		!allows_static_access && is_constant
	)>
	CC_ALWAYS_INLINE constexpr
	auto value(Integer = 0) const noexcept ->
	detail::const_result<Integer, traits, !allows_static_access && is_constant>
	{ return m_wrapped.value(); }

	template <class Integer, nd_enable_if(
		!allows_static_access && !is_constant
	)>
	CC_ALWAYS_INLINE
	auto value(const Integer n) noexcept ->
	detail::const_result<Integer, traits, !allows_static_access && !is_constant>
	{ return m_wrapped.value(n); }

	template <class Integer, nd_enable_if(
		!allows_static_access && !is_constant
	)>
	CC_ALWAYS_INLINE constexpr
	auto value(const Integer n) const noexcept ->
	detail::const_result<Integer, traits, !allows_static_access && !is_constant>
	{ return m_wrapped.value(n); }
};

}

#endif
