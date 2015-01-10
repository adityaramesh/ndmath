/*
** File Name: index.hpp
** Author:    Aditya Ramesh
** Date:      01/08/2015
** Contact:   _@adityaramesh.com
*/

#ifndef ZC3DF0134_42DB_48EA_B8DC_8B2AFB58D2CD
#define ZC3DF0134_42DB_48EA_B8DC_8B2AFB58D2CD

#include <array>
#include <utility>
#include <ndmath/index/constant_index.hpp>

namespace nd {

template <size_t Dims>
class index final :
public index_base<Dims, false, size_t&, const size_t&, index<Dims>>
{
	using self = index<Dims>;
	using base = index_base<Dims, false, size_t&, const size_t&, self>;

	using index_list = std::initializer_list<size_t>;
	std::array<size_t, Dims> m_indices;
public:
	using base::operator=;
	using base::operator();

	template <size_t... Indices>
	CC_ALWAYS_INLINE constexpr
	explicit index(const std::index_sequence<Indices...>&)
	noexcept : m_indices{{Indices...}} {}

	template <size_t... Indices>
	CC_ALWAYS_INLINE constexpr
	explicit index(const constant_index<Indices...>&)
	noexcept : index{std::index_sequence<Indices...>{}} {}

	CC_ALWAYS_INLINE
	explicit index(const index_list& indices)
	noexcept { boost::copy(indices, std::begin(m_indices)); }

	CC_ALWAYS_INLINE index(const index& rhs) noexcept
	{ boost::copy(rhs, m_indices.begin()); }

	template <
		size_t Dims_,
		bool IsConstexpr_,
		class Value_,
		class ConstValue_,
		class Derived_
	>
	CC_ALWAYS_INLINE
	index(const index_base<Dims_, IsConstexpr_, Value_, ConstValue_, Derived_>& rhs)
	noexcept { boost::copy(rhs, m_indices.begin()); }

	CC_ALWAYS_INLINE auto&
	operator()(const size_t& n) noexcept
	{ return m_indices[n]; }

	CC_ALWAYS_INLINE const auto&
	operator()(const size_t& n) const noexcept
	{ return m_indices[n]; }
};

template <class... Ts>
CC_ALWAYS_INLINE auto
make_index(const Ts&... ts) noexcept
{ return index<sizeof...(Ts)>{((size_t)ts)...}; }

}

#endif
