/*
** File Name: range.hpp
** Author:    Aditya Ramesh
** Date:      01/11/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z2FFF753A_E01F_4298_B56F_AFB95A4D87C5
#define Z2FFF753A_E01F_4298_B56F_AFB95A4D87C5

#include <ccbase/platform.hpp>
#include <ndmath/index.hpp>
#include <ndmath/utility.hpp>
#include <ndmath/range/for_each.hpp>
#include <ndmath/range/range_iterator.hpp>

namespace nd {

template <class Bases, class Extents, class Strides>
class range final
{
public:
	static CC_ALWAYS_INLINE CC_CONST
	constexpr auto dims() noexcept { return Bases::dims(); }

	using self           = range<Bases, Extents, Strides>;
	using integer        = typename Bases::integer;
	using iterator       = range_iterator<self>;
	using const_iterator = iterator;
private:
	const Bases& m_bases;
	const Extents& m_extents;
	const Strides& m_strides;
public:
	CC_ALWAYS_INLINE CC_CONST constexpr
	explicit range(
		const Bases& bases,
		const Extents& extents,
		const Strides& strides
	) noexcept :
		m_bases{bases},
		m_extents{extents},
		m_strides{strides} {}

	template <class T>
	CC_ALWAYS_INLINE auto
	base(const T& n) const noexcept
	{ return m_bases(n); }

	template <class T>
	CC_ALWAYS_INLINE auto
	extent(const T& n) const noexcept
	{ return m_extents(n); }

	template <class T>
	CC_ALWAYS_INLINE auto
	stride(const T& n) const noexcept
	{ return m_strides(n); }

	CC_ALWAYS_INLINE CC_CONST constexpr
	const auto& bases() const noexcept
	{ return m_bases; }

	CC_ALWAYS_INLINE CC_CONST constexpr
	const auto& extents() const noexcept
	{ return m_extents; }

	CC_ALWAYS_INLINE CC_CONST constexpr
	const auto& strides() const noexcept
	{ return m_strides; }

	template <class Func>
	CC_ALWAYS_INLINE void
	operator()(const Func& f)
	const noexcept(noexcept(f(cindex_n<Bases::dims(), 0>)))
	{ for_each(*this, f); }

	CC_ALWAYS_INLINE CC_CONST constexpr
	auto begin() const noexcept
	{ return iterator{*this}; }

	CC_ALWAYS_INLINE CC_CONST constexpr
	auto end() const noexcept
	{ return iterator{*this}; }
};

template <class Bases, class Extents, class Strides>
CC_ALWAYS_INLINE constexpr
auto operator==(
	const range<Bases, Extents, Strides>& lhs,
	const range<Bases, Extents, Strides>& rhs
) noexcept
{
	return lhs.bases()   == rhs.bases()   &&
	       lhs.extents() == rhs.extents() &&
	       lhs.strides() == rhs.strides();
}

template <class Bases, class Extents, class Strides>
CC_ALWAYS_INLINE constexpr
auto operator!=(
	const range<Bases, Extents, Strides>& lhs,
	const range<Bases, Extents, Strides>& rhs
) noexcept { return !(lhs == rhs); }

template <class Bases, class Extents, class Strides>
CC_ALWAYS_INLINE CC_CONST constexpr
auto begin(const range<Bases, Extents, Strides>& r)
noexcept { return r.begin(); }

template <class Bases, class Extents, class Strides>
CC_ALWAYS_INLINE CC_CONST constexpr
auto end(const range<Bases, Extents, Strides>& r)
noexcept { return r.end(); }

template <
	class Bases,
	class Extents,
	class Strides,
	nd_enable_if((
		Bases::dims() == Extents::dims() && 
		Extents::dims() == Strides::dims() &&
		std::is_same<
			typename Bases::integer,
			typename Extents::integer
		>::value &&
		std::is_same<
			typename Extents::integer,
			typename Strides::integer
		>::value
	))
>
CC_ALWAYS_INLINE CC_CONST constexpr
auto make_range(const Bases& b, const Extents& e, const Strides& s)
noexcept { return range<Bases, Extents, Strides>{b, e, s}; }

template <
	class Bases,
	class Extents,
	nd_enable_if((
		Bases::dims() == Extents::dims() &&
		std::is_same<
			typename Bases::integer,
			typename Extents::integer
		>::value
	))
>
CC_ALWAYS_INLINE CC_CONST constexpr
auto make_range(const Bases& b, const Extents& e)
noexcept
{
	constexpr auto dims = Bases::dims();
	using integer = typename Bases::integer;
	return make_range(b, e, basic_cindex_n<integer, dims, 1>);
}

template <class Extents>
CC_ALWAYS_INLINE CC_CONST constexpr
auto make_range(const Extents& e) noexcept
{
	constexpr auto dims = Extents::dims();
	using integer = typename Extents::integer;
	return make_range(basic_cindex_n<integer, dims, 0>, e);
}

template <class Integer, Integer... Ts>
static constexpr auto basic_crange =
make_range(nd::basic_cindex<Integer, Ts...>);

template <uint_fast32_t... Ts>
static constexpr auto crange =
basic_crange<uint_fast32_t, Ts...>;

template <class Integer, size_t N, size_t Value>
static constexpr auto basic_crange_n =
make_range(nd::basic_cindex_n<Integer, N, Value>);

template <size_t N, size_t Value>
static constexpr auto crange_n =
basic_crange_n<uint_fast32_t, N, Value>;

}

#endif
