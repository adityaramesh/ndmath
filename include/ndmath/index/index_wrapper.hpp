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

namespace nd {

template <class T>
class index_wrapper;

template <class Index>
class index_iterator;

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
	static constexpr auto allows_static_access =
	T::allows_static_access;
	
	using result         = typename T::result;
	using const_result   = typename T::const_result;
	using iterator       = index_iterator<self>;
	using const_iterator = index_iterator<const self>;
	using integer        = std::decay_t<result>;
private:
	using index_list = std::initializer_list<integer>;

	T m_val;
public:
	CC_ALWAYS_INLINE CC_CONST constexpr
	explicit index_wrapper()
	noexcept : m_val{} {}

	template <class... Args>
	CC_ALWAYS_INLINE constexpr
	explicit index_wrapper(in_place_t, Args&&... args)
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

	template <class Integer, nd_enable_if(allows_static_access)>
	CC_ALWAYS_INLINE CC_CONST constexpr
	static const_result at(const Integer n) noexcept
	{ return T::at(n); }

	template <class Loc, nd_enable_if(allows_static_access)>
	CC_ALWAYS_INLINE CC_CONST constexpr
	static const_result at(const location_wrapper<Loc> l) noexcept
	{ return at(l.value(dims() - 1)); }

	template <class Integer, nd_enable_if(!allows_static_access)>
	CC_ALWAYS_INLINE constexpr
	result at(const Integer n) noexcept
	{ return m_val.at(n); }

	template <class Integer, nd_enable_if(!allows_static_access)>
	CC_ALWAYS_INLINE constexpr
	const_result at(const Integer n) const noexcept
	{ return m_val.at(n); }

	template <class Loc, nd_enable_if(!allows_static_access)>
	CC_ALWAYS_INLINE constexpr
	result at(const location_wrapper<Loc> l) noexcept
	{ return at(l.value(dims() - 1)); }

	template <class Loc, nd_enable_if(!allows_static_access)>
	CC_ALWAYS_INLINE constexpr
	const_result at(const location_wrapper<Loc> l) const noexcept
	{ return at(l.value(dims() - 1)); }

	template <class Integer>
	CC_ALWAYS_INLINE constexpr
	result operator()(const Integer n) noexcept
	{ return at(n); }

	template <class Integer>
	CC_ALWAYS_INLINE constexpr
	const_result operator()(const Integer n) const noexcept
	{ return at(n); }

	template <class Loc>
	CC_ALWAYS_INLINE constexpr
	result operator()(const location_wrapper<Loc> l) noexcept
	{ return at(l); }

	template <class Loc>
	CC_ALWAYS_INLINE constexpr
	const_result operator()(const location_wrapper<Loc> l) const noexcept
	{ return at(l); }

	template <nd_enable_if(allows_static_access)>
	CC_ALWAYS_INLINE CC_CONST constexpr
	static const_result first() noexcept
	{ return at(uint_fast32_t{0}); }

	template <nd_enable_if(allows_static_access)>
	CC_ALWAYS_INLINE CC_CONST constexpr
	static const_result last() noexcept
	{ return at(uint_fast32_t(dims() - 1)); }

	template <nd_enable_if(!allows_static_access)>
	CC_ALWAYS_INLINE constexpr
	result first() noexcept
	{ return at(uint_fast32_t{0}); }

	template <nd_enable_if(!allows_static_access)>
	CC_ALWAYS_INLINE constexpr
	const_result first() const noexcept
	{ return at(uint_fast32_t{0}); }

	template <nd_enable_if(!allows_static_access)>
	CC_ALWAYS_INLINE constexpr
	result last() noexcept
	{ return at(uint_fast32_t(dims() - 1)); }

	template <nd_enable_if(!allows_static_access)>
	CC_ALWAYS_INLINE constexpr
	const_result last() const noexcept
	{ return at(uint_fast32_t(dims() - 1)); }

	/*
	** Subindex creation.
	*/

	template <class D1, class D2>
	CC_ALWAYS_INLINE constexpr
	auto operator()(
		const location_wrapper<D1> l1,
		const location_wrapper<D2> l2
	) noexcept
	{
		constexpr auto a = l1.value(dims() - 1);
		constexpr auto b = l2.value(dims() - 1);
		return make_subindex<a, b>(*this);
	}

	template <class D1, class D2>
	CC_ALWAYS_INLINE constexpr
	auto operator()(
		const location_wrapper<D1> l1,
		const location_wrapper<D2> l2
	) const noexcept
	{
		constexpr auto a = l1.value(dims() - 1);
		constexpr auto b = l2.value(dims() - 1);
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

template <
	size_t N, class T,
	nd_enable_if(index_wrapper<T>::allows_static_access)
>
CC_ALWAYS_INLINE CC_CONST constexpr
static auto at(index_wrapper<T>) noexcept
{
	using integer = typename index_wrapper<T>::integer;
	return basic_cloc<integer, index_wrapper<T>::at(N)>;
}

template <
	size_t N, class T,
	nd_enable_if(!index_wrapper<T>::allows_static_access)
>
CC_ALWAYS_INLINE constexpr
auto at(const index_wrapper<T>& w) noexcept
{ return make_location(w.at(N)); }

#define nd_define_relational_op(symbol)                  \
                                                         \
template <class Index1, class Index2>                    \
CC_ALWAYS_INLINE constexpr                               \
auto operator symbol (                                   \
	const index_wrapper<Index1>& lhs,                \
	const index_wrapper<Index2>& rhs                 \
) noexcept                                               \
{                                                        \
	if (lhs.dims() != rhs.dims()) {                  \
		return false;                            \
	}                                                \
	for (auto i = size_t{0}; i != lhs.dims(); ++i) { \
		if (!(lhs(i) symbol rhs(i))) {           \
			return false;                    \
		}                                        \
	}                                                \
	return true;                                     \
}

nd_define_relational_op(==)
nd_define_relational_op(!=)
nd_define_relational_op(>)
nd_define_relational_op(<)
nd_define_relational_op(>=)
nd_define_relational_op(<=)

#undef nd_define_relational_op

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
