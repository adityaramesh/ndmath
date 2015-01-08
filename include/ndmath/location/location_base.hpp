/*
** File Name: location_base.hpp
** Author:    Aditya Ramesh
** Date:      01/08/2015
** Contact:   _@adityaramesh.com
*/

#ifndef ZFF60AFF1_88C6_4555_A018_43F5FCE35128
#define ZFF60AFF1_88C6_4555_A018_43F5FCE35128

#include <ndmath/location/expression_forward.hpp>

namespace nd {

template <class Derived>
class location_base
{
public:
	CC_ALWAYS_INLINE CC_CONST
	constexpr auto operator()(const size_t& dims)
	const noexcept { return (*this)()(dims); }

	CC_ALWAYS_INLINE CC_CONST
	constexpr auto& operator()() noexcept
	{ return static_cast<Derived&>(*this); }

	CC_ALWAYS_INLINE CC_CONST
	constexpr const auto& operator()() const noexcept
	{ return static_cast<const Derived&>(*this); }
};

}

#endif
