/*
** File Name: location_wrapper.hpp
** Author:    Aditya Ramesh
** Date:      01/08/2015
** Contact:   _@adityaramesh.com
*/

#ifndef ZFF60AFF1_88C6_4555_A018_43F5FCE35128
#define ZFF60AFF1_88C6_4555_A018_43F5FCE35128

#include <utility>

namespace nd {

template <class T>
class location_wrapper
{
	T m_val;
public:
	template <class... Args>
	CC_ALWAYS_INLINE CC_CONST constexpr 
	explicit location_wrapper(Args&&... args)
	noexcept : m_val(std::forward<Args>(args)...) {}

	CC_ALWAYS_INLINE CC_CONST constexpr
	auto value() const noexcept
	{ return m_val; }

	template <class Integer>
	CC_ALWAYS_INLINE CC_CONST constexpr
	auto operator()(const Integer n) const noexcept
	{ return m_val(n); }
};

}

#endif
