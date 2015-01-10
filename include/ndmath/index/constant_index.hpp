/*
** File Name: constant_index.hpp
** Author:    Aditya Ramesh
** Date:      01/08/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z3AA81C53_00C1_4028_9A17_85048FB3164F
#define Z3AA81C53_00C1_4028_9A17_85048FB3164F

#include <array>
#include <ndmath/index/index_base.hpp>

namespace nd {

template <size_t... Ts>
class constant_index final :
public index_base<sizeof...(Ts), true, constant_index<Ts...>>
{
	using self = constant_index<Ts...>;
	using base = index_base<sizeof...(Ts), true, self>;

	static constexpr auto m_indices =
	std::array<size_t, sizeof...(Ts)>{{Ts...}};
public:
	using base::operator();

	CC_ALWAYS_INLINE CC_CONST
	constexpr auto operator()(const size_t& i)
	const noexcept { return m_indices[i]; }
};

template <size_t... Ts>
static constexpr auto cindex = constant_index<Ts...>{};

}

#endif
