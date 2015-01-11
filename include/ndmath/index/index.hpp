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

template <class Integer, size_t Dims>
class index final :
public index_base<Dims, false, Integer&, const Integer&, index<Integer, Dims>>
{
	using self = index<Integer, Dims>;
	using base = index_base<Dims, false, Integer&, const Integer&, self>;

	using index_list = std::initializer_list<Integer>;
	std::array<Integer, Dims> m_indices;
public:
	using base::operator=;
	using base::operator();

	template <Integer... Indices>
	CC_ALWAYS_INLINE constexpr
	explicit index(const std::integer_sequence<Integer, Indices...>&)
	noexcept : m_indices{{Indices...}} {}

	template <Integer... Indices>
	CC_ALWAYS_INLINE constexpr
	explicit index(const constant_index<Integer, Indices...>&)
	noexcept : index{std::integer_sequence<Integer, Indices...>{}} {}

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

	template <class T>
	CC_ALWAYS_INLINE auto&
	operator()(const T& n) noexcept
	{ return m_indices[n]; }

	template <class T>
	CC_ALWAYS_INLINE const auto&
	operator()(const T& n) const noexcept
	{ return m_indices[n]; }
};

template <class... Ts, class Integer = uint_fast32_t>
CC_ALWAYS_INLINE auto
make_index(const Ts&... ts) noexcept
{ return index<Integer, sizeof...(Ts)>{((Integer)ts)...}; }

template <class Sequence>
static constexpr index<
	typename Sequence::value_type,
	Sequence::size()
> index_c{Sequence{}};

template <class Integer, size_t Length, size_t Value>
static constexpr index<Integer, Length>
basic_index_cn{cc::constant_sequence<Integer, Length, Value>{}};

template <size_t Length, size_t Value>
static constexpr index<uint_fast32_t, Length>
index_cn{cc::constant_sequence<uint_fast32_t, Length, Value>{}};

}

#endif
