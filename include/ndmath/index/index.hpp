/*
** File Name: index.hpp
** Author:    Aditya Ramesh
** Date:      01/08/2015
** Contact:   _@adityaramesh.com
*/

#ifndef ZC3DF0134_42DB_48EA_B8DC_8B2AFB58D2CD
#define ZC3DF0134_42DB_48EA_B8DC_8B2AFB58D2CD

#include <array>
#include <ndmath/index/index_base.hpp>

namespace nd {

template <size_t Dims>
class index final :
public index_base<Dims, false, index<Dims>>
{
	using index_list = std::initializer_list<size_t>;
	std::array<size_t, Dims> m_indices;
public:
	template <size_t... Indices>
	CC_ALWAYS_INLINE constexpr
	explicit index(const std::integer_sequence<size_t, Indices...>&)
	noexcept : m_indices{Indices...} {}

	CC_ALWAYS_INLINE constexpr
	explicit index(const index_list& indices)
	noexcept { boost::copy(indices, std::begin(m_indices)); }

	CC_ALWAYS_INLINE index(const index& rhs) noexcept
	{ boost::copy(rhs.m_indices, std::begin(m_indices)); }

	CC_ALWAYS_INLINE index&
	operator=(const index& rhs) noexcept
	{
		boost::copy(rhs.m_indices, std::begin(m_indices));
		return *this;
	}

	template <class T>
	CC_ALWAYS_INLINE index&
	operator=(const index_list& rhs) noexcept
	{
		boost::copy(rhs, std::begin(m_indices));
		return *this;
	}

	CC_ALWAYS_INLINE auto&
	operator()(const size_t& n) noexcept
	{ return m_indices[n]; }

	CC_ALWAYS_INLINE const auto&
	operator()(const size_t& n) const noexcept
	{ return m_indices[n]; }
};

}

#endif
