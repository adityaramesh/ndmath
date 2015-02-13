/*
** File Name: constant_array.hpp
** Author:    Aditya Ramesh
** Date:      01/12/2015
** Contact:   _@adityaramesh.com
*/

#ifndef ZFE630DE2_3D69_492F_BA3A_B1C4BBC0DE15
#define ZFE630DE2_3D69_492F_BA3A_B1C4BBC0DE15

#include <ndmath/index/offset.hpp>

namespace nd {

/*
** TODO: should we incorporate the notion of storage order here?
*/

template <size_t Dims, class Scalar, class Extents, class... Ts>
class constant_array final :
public array_base<
	Dims, true, Scalar, Scalar, size_t,
	constant_array<Dims, Scalar, Ts...>
>
{
	using self = constant_index<Dims, Scalar, Ts...>;
	using base = array_base<Dims, true, Scalar, Scalar, size_t, self>;

	static constexpr std::array<Scalar, sizeof...(Ts)>
	m_data{{cc::cast<Scalar>(Ts)...}};

	const Extents& m_extents;
public:
	using base::operator();

	CC_ALWAYS_INLINE CC_CONST constexpr
	explicit constant_array(const Extents& extents)
	noexcept : m_extents{extents} {}
	
	CC_ALWAYS_INLINE CC_CONST constexpr
	const auto& extents() const noexcept
	{ return m_extents; }
	
	template <
		bool IsConstexpr_,
		class Result_,
		class ConstResult_,
		class Derived_
	>
	CC_ALWAYS_INLINE CC_CONST constexpr
	auto operator()(const index_base<
		IsConstexpr_, Result_, ConstResult_, Derived_
	>& b) noexcept { return m_data[offset(b, e)]; }
};

}

#endif
