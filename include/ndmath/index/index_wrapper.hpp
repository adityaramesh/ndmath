/*
** File Name: index_wrapper.hpp
** Author:    Aditya Ramesh
** Date:      01/13/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z26650853_4E79_4AA8_A435_BE8BBAB75C90
#define Z26650853_4E79_4AA8_A435_BE8BBAB75C90

#include <iostream>
#include <initializer_list>
#include <utility>
#include <boost/range/algorithm.hpp>
#include <ndmath/location.hpp>
#include <ndmath/index/index_iterator.hpp>

namespace nd {

template <class T>
class index_wrapper;

template <size_t A, size_t B, class Index>
CC_ALWAYS_INLINE constexpr
auto make_subindex(index_wrapper<Index>& w)
noexcept;

template <size_t A, size_t B, class Index>
CC_ALWAYS_INLINE constexpr
auto make_const_subindex(const index_wrapper<Index>& w)
noexcept;

template <class T>
class index_wrapper final
{
private:
	using self = index_wrapper<T>;
public:
	using result         = typename T::result;
	using const_result   = typename T::const_result;
	using iterator       = index_iterator<self>;
	using const_iterator = index_iterator<const self>;
	using integer        = std::decay_t<result>;
private:
	using index_list = std::initializer_list<integer>;

	T m_val;
public:
	template <class... Args>
	CC_ALWAYS_INLINE constexpr
	explicit index_wrapper(Args&&... args)
	noexcept : m_val(std::forward<Args>(args)...) {}

	CC_ALWAYS_INLINE
	auto& operator=(const index_list indices) noexcept
	{
		boost::copy(indices, begin());
		return *this;
	}

	template <class Index>
	CC_ALWAYS_INLINE
	auto& operator=(const index_wrapper<Index> w) noexcept
	{
		boost::copy(w, begin());
		return *this;
	}

	/*
	** General accessors.
	*/

	CC_ALWAYS_INLINE CC_CONST constexpr
	auto value() const noexcept
	{ return m_val; }

	CC_ALWAYS_INLINE CC_CONST constexpr
	static auto dims() noexcept
	{ return T::dims; }

	/*
	** Element accessors.
	*/

	template <class Integer>
	CC_ALWAYS_INLINE constexpr
	result operator()(const Integer n) noexcept
	{ return m_val(n); }

	template <class Integer>
	CC_ALWAYS_INLINE constexpr
	const_result operator()(const Integer n) const noexcept
	{ return m_val(n); }

	template <class Loc>
	CC_ALWAYS_INLINE constexpr
	result operator()(const location_wrapper<Loc> l) noexcept
	{ return m_val(l(dims() - 1)); }

	template <class Loc>
	CC_ALWAYS_INLINE constexpr
	const_result operator()(const location_wrapper<Loc> l) const noexcept
	{ return m_val(l(dims() - 1)); }

	CC_ALWAYS_INLINE constexpr
	result first() noexcept
	{ return m_val(uint_fast32_t{0}); }

	CC_ALWAYS_INLINE constexpr
	const_result first() const noexcept
	{ return m_val(uint_fast32_t{0}); }

	CC_ALWAYS_INLINE constexpr
	result last() noexcept
	{ return m_val(dims() - 1); }

	CC_ALWAYS_INLINE constexpr
	const_result last() const noexcept
	{ return m_val(dims() - 1); }

	/*
	** Subindex creation.
	*/

	template <class D1, class D2>
	CC_ALWAYS_INLINE constexpr
	auto operator()(
		const const_location_wrapper<D1> l1,
		const const_location_wrapper<D2> l2
	) noexcept
	{
		constexpr auto a = l1(dims() - 1);
		constexpr auto b = l2(dims() - 1);
		return make_subindex<a, b>(*this);
	}

	template <class D1, class D2>
	CC_ALWAYS_INLINE constexpr
	auto operator()(
		const const_location_wrapper<D1> l1,
		const const_location_wrapper<D2> l2
	) const noexcept
	{
		constexpr auto a = l1(dims() - 1);
		constexpr auto b = l2(dims() - 1);
		return make_const_subindex<a, b>(*this);
	}

	CC_ALWAYS_INLINE constexpr
	auto head() noexcept
	{ return (*this)(tokens::c<0>, tokens::end); }

	CC_ALWAYS_INLINE constexpr
	auto head() const noexcept
	{ return (*this)(tokens::c<0>, tokens::end); }

	CC_ALWAYS_INLINE constexpr
	auto tail() noexcept
	{ return (*this)(tokens::c<1>, tokens::end); }

	CC_ALWAYS_INLINE constexpr
	auto tail() const noexcept
	{ return (*this)(tokens::c<1>, tokens::end); }

	/*
	** Iteration.
	*/

	CC_ALWAYS_INLINE constexpr
	auto begin() noexcept
	{ return iterator{*this}; }

	CC_ALWAYS_INLINE constexpr
	auto begin() const noexcept
	{ return const_iterator{*this}; }

	CC_ALWAYS_INLINE constexpr
	auto end() noexcept
	{ return iterator{*this, dims() - 1}; }

	CC_ALWAYS_INLINE constexpr
	auto end() const noexcept
	{ return const_iterator{*this, dims() - 1}; }
};

template <class Index1, class Index2>
CC_ALWAYS_INLINE constexpr
auto operator==(
	const index_wrapper<Index1>& lhs,
	const index_wrapper<Index2>& rhs
) noexcept
{
	if (lhs.dims() != rhs.dims()) {
		return false;
	}
	for (auto i = size_t{0}; i != lhs.dims(); ++i) {
		if (lhs(i) != rhs(i)) {
			return false;
		}
	}
	return true;
}

template <class Index1, class Index2>
CC_ALWAYS_INLINE constexpr
auto operator!=(
	const index_wrapper<Index1>& lhs,
	const index_wrapper<Index2>& rhs
) noexcept { return !(lhs == rhs); }

template <class Char, class Traits, class Index>
auto& operator<<(
	std::basic_ostream<Char, Traits>& os,
	const index_wrapper<Index>& w
) noexcept
{
	os << "[";
	for (const auto& i : w.head()) {
		os << i << ", ";
	}
	return os << w.last() << "]";
}

}

#endif
