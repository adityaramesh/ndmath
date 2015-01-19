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

template <class T>
class location_wrapper final
{
public:
	static constexpr auto allows_static_access =
	T::allows_static_access;
private:
	const T m_val;
public:
	template <class... Args>
	CC_ALWAYS_INLINE constexpr 
	explicit location_wrapper(Args&&... args)
	noexcept : m_val(std::forward<Args>(args)...) {}

	CC_ALWAYS_INLINE CC_CONST constexpr
	auto value() const noexcept
	{ return m_val; }

	template <class Integer, nd_enable_if(allows_static_access)>
	CC_ALWAYS_INLINE CC_CONST constexpr
	static auto eval(const Integer n) noexcept
	{ return T::eval(n); }

	template <class Integer, nd_enable_if(!allows_static_access)>
	CC_ALWAYS_INLINE CC_CONST constexpr
	auto eval(const Integer n) const noexcept
	{ return m_val.eval(n); }

	template <class Integer>
	CC_ALWAYS_INLINE CC_CONST constexpr
	auto operator()(const Integer n) const noexcept
	{ return eval(n); }
};

}

#endif
