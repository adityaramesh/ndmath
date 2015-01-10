/*
** File Name: index_base.hpp
** Author:    Aditya Ramesh
** Date:      01/08/2015
** Contact:   _@adityaramesh.com
**
** The top of the index expression hierarchy.
*/

#ifndef Z81B94778_5D32_488E_A55B_54F8D840A945
#define Z81B94778_5D32_488E_A55B_54F8D840A945

#include <boost/range/algorithm.hpp>
#include <boost/range/numeric.hpp>
#include <ndmath/location.hpp>
#include <ndmath/index/expression_forward.hpp>
#include <ndmath/index/index_iterator.hpp>

namespace nd {

/*
** Derived classes need only implement `operator()(const size_t&)`.
*/
template <size_t Dims, bool IsConstexpr, class Derived>
class index_base;

template <size_t Dims, class Derived>
class index_base<Dims, false, Derived>
{
	using self = index_base<Dims, false, Derived>;
	using index_list = std::initializer_list<size_t>;
public:
	static constexpr auto is_constexpr = false;
	using iterator = index_iterator<self>;
	using const_iterator = index_iterator<const self>;

	static CC_ALWAYS_INLINE CC_CONST
	constexpr auto dims() noexcept { return Dims; }

	CC_ALWAYS_INLINE CC_CONST constexpr
	auto& operator()() noexcept
	{ return static_cast<Derived&>(*this); }

	CC_ALWAYS_INLINE CC_CONST constexpr
	const auto& operator()() const noexcept
	{ return static_cast<const Derived&>(*this); }

	/*
	** Element accessors.
	*/

	CC_ALWAYS_INLINE index_base&
	operator=(const index_base& rhs) noexcept
	{ boost::copy(rhs, begin()); return *this; }

	template <size_t Dims_, bool IsConstexpr_, class Derived_>
	CC_ALWAYS_INLINE index_base&
	operator=(const index_base<Dims_, IsConstexpr_, Derived_>& rhs)
	noexcept { boost::copy(rhs, begin()); return *this; }

	CC_ALWAYS_INLINE index_base&
	operator=(const index_list& rhs) noexcept
	{ boost::copy(rhs, begin()); return *this; }

	CC_ALWAYS_INLINE
	auto& operator()(const size_t& i)
	noexcept { return (*this)()(i); }

	CC_ALWAYS_INLINE
	auto operator()(const size_t& i)
	const noexcept { return (*this)()(i); }

	template <class D>
	CC_ALWAYS_INLINE
	auto& operator()(const location_base<D>& l)
	noexcept { return (*this)(l(dims() - 1)); }

	template <class D>
	CC_ALWAYS_INLINE
	auto operator()(const location_base<D>& l)
	const noexcept { return (*this)(l(dims() - 1)); }

	CC_ALWAYS_INLINE
	auto& first() noexcept
	{ return (*this)(size_t{0}); }

	CC_ALWAYS_INLINE
	auto first() const noexcept
	{ return (*this)(size_t{0}); }

	CC_ALWAYS_INLINE
	auto& last() noexcept
	{ return (*this)(dims() - 1); }

	CC_ALWAYS_INLINE
	auto last() const noexcept
	{ return (*this)(dims() - 1); }

	/*
	** Subindex creation.
	*/

	/*
	** Note: we should be able to use a(dims() - 1) instead of the static
	** member function, but there's a bug in the version of clang that I'm
	** using right now that causes the code to be erroneously rejected.
	*/
	template <class D1, class D2>
	CC_ALWAYS_INLINE constexpr auto
	operator()(const location_base<D1>&, const location_base<D2>&)
	noexcept
	{
		return make_subindex<
			location_base<D1>::eval(dims() - 1),
			location_base<D2>::eval(dims() - 1)
		>(*this);
	}

	template <class D1, class D2>
	CC_ALWAYS_INLINE constexpr auto
	operator()(const location_base<D1>&, const location_base<D2>&)
	const noexcept
	{
		return make_subindex<
			location_base<D1>::eval(dims() - 1),
			location_base<D2>::eval(dims() - 1)
		>(*this);
	}

	CC_ALWAYS_INLINE
	auto head() noexcept
	{ return (*this)(tokens::c<0>, tokens::end - tokens::c<1>); }

	CC_ALWAYS_INLINE
	auto head() const noexcept
	{ return (*this)(tokens::c<0>, tokens::end - tokens::c<1>); }

	CC_ALWAYS_INLINE
	auto tail() noexcept
	{ return (*this)(tokens::c<1>, tokens::end); }

	CC_ALWAYS_INLINE
	auto tail() const noexcept
	{ return (*this)(tokens::c<1>, tokens::end); }

	/*
	** Iteration.
	*/

	CC_ALWAYS_INLINE
	auto begin() noexcept
	{ return iterator{*this}; }

	CC_ALWAYS_INLINE
	auto begin() const noexcept
	{ return const_iterator{*this}; }

	CC_ALWAYS_INLINE
	auto end() noexcept
	{ return iterator{*this, dims()}; }

	CC_ALWAYS_INLINE
	auto end() const noexcept
	{ return const_iterator{*this, dims()}; }
};

template <size_t Dims, class Derived>
class index_base<Dims, true, Derived>
{
	using self = index_base<Dims, true, Derived>;
public:
	static constexpr auto is_constexpr = true;
	using iterator = index_iterator<self>;
	using const_iterator = iterator;

	static CC_ALWAYS_INLINE CC_CONST
	constexpr auto dims() noexcept { return Dims; }

	CC_ALWAYS_INLINE CC_CONST constexpr
	const auto& operator()() const noexcept
	{ return static_cast<const Derived&>(*this); }

	/*
	** Element accessors.
	*/

	CC_ALWAYS_INLINE CC_CONST
	constexpr auto operator()(const size_t& i)
	const noexcept { return (*this)()(i); }

	template <class D>
	CC_ALWAYS_INLINE CC_CONST
	constexpr auto operator()(const location_base<D>& l)
	const noexcept { return (*this)(l(dims() - 1)); }

	CC_ALWAYS_INLINE CC_CONST
	constexpr auto first() const noexcept
	{ return (*this)(size_t{0}); }

	CC_ALWAYS_INLINE CC_CONST
	constexpr auto last() const noexcept
	{ return (*this)(dims() - 1); }

	/*
	** Subindex creation.
	*/

	template <class D1, class D2>
	CC_ALWAYS_INLINE CC_CONST constexpr
	auto operator()(const location_base<D1>&, const location_base<D2>&)
	const noexcept
	{
		return make_subindex<
			location_base<D1>::eval(dims() - 1),
			location_base<D2>::eval(dims() - 1)
		>(*this);
	}

	CC_ALWAYS_INLINE CC_CONST
	constexpr auto head() const noexcept
	{ return (*this)(tokens::c<0>, tokens::end - tokens::c<1>); }

	CC_ALWAYS_INLINE CC_CONST
	constexpr auto tail() const noexcept
	{ return (*this)(tokens::c<1>, tokens::end); }

	/*
	** Iteration.
	*/

	CC_ALWAYS_INLINE CC_CONST
	constexpr auto begin() const noexcept
	{ return iterator{*this}; }

	CC_ALWAYS_INLINE CC_CONST
	constexpr auto end() const noexcept
	{ return iterator{*this, dims()}; }
};

template <
	size_t Dims1,
	size_t Dims2,
	bool IsConstexpr1,
	bool IsConstexpr2,
	class Derived1,
	class Derived2
>
CC_ALWAYS_INLINE auto
operator==(
	const index_base<Dims1, IsConstexpr1, Derived1>& lhs,
	const index_base<Dims2, IsConstexpr2, Derived2>& rhs
) noexcept { return boost::equal(lhs, rhs); }

template <
	size_t Dims1,
	size_t Dims2,
	class Derived1,
	class Derived2
>
CC_ALWAYS_INLINE CC_CONST constexpr
auto operator==(
	const index_base<Dims1, true, Derived1>& lhs,
	const index_base<Dims2, true, Derived2>& rhs
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

template <
	size_t Dims1,
	size_t Dims2,
	bool IsConstexpr1,
	bool IsConstexpr2,
	class Derived1,
	class Derived2
>
CC_ALWAYS_INLINE auto
operator!=(
	const index_base<Dims1, IsConstexpr1, Derived1>& lhs,
	const index_base<Dims2, IsConstexpr2, Derived2>& rhs
) noexcept { return !(lhs == rhs); }

template <
	size_t Dims1,
	size_t Dims2,
	class Derived1,
	class Derived2
>
CC_ALWAYS_INLINE CC_CONST constexpr
auto operator!=(
	const index_base<Dims1, true, Derived1>& lhs,
	const index_base<Dims2, true, Derived2>& rhs
) noexcept { return !(lhs == rhs); }

template <size_t Dims, bool IsConstexpr, class Derived>
CC_ALWAYS_INLINE auto
begin(index_base<Dims, IsConstexpr, Derived>& b) noexcept
{ return b.begin(); }

template <size_t Dims, bool IsConstexpr, class Derived>
CC_ALWAYS_INLINE auto
begin(const index_base<Dims, IsConstexpr, Derived>& b) noexcept
{ return b.begin(); }

template <size_t Dims, class Derived>
CC_ALWAYS_INLINE CC_CONST constexpr
auto begin(const index_base<Dims, true, Derived>& b)
noexcept { return b.begin(); }

template <size_t Dims, bool IsConstexpr, class Derived>
CC_ALWAYS_INLINE auto
end(index_base<Dims, IsConstexpr, Derived>& b) noexcept
{ return b.end(); }

template <size_t Dims, bool IsConstexpr, class Derived>
CC_ALWAYS_INLINE auto
end(const index_base<Dims, IsConstexpr, Derived>& b) noexcept
{ return b.end(); }

template <size_t Dims, class Derived>
CC_ALWAYS_INLINE CC_CONST constexpr
auto end(const index_base<Dims, true, Derived>& b)
noexcept { return b.end(); }

template <size_t Dims, bool IsConstexpr, class Derived>
CC_ALWAYS_INLINE auto
off(const index_base<Dims, IsConstexpr, Derived>& b) noexcept
{
	return boost::accumulate(b, size_t{1},
		[] (const auto& x, const auto& y)
		CC_ALWAYS_INLINE CC_CONST noexcept
		{ return x * y; });
}

template <size_t Dims, class Derived>
CC_ALWAYS_INLINE CC_CONST constexpr
auto off(const index_base<Dims, true, Derived>& b)
noexcept
{
	auto n = size_t{1};
	for (auto i = size_t{0}; i != b.dims(); ++i) {
		n *= b(i);
	}
	return n;
}

template <
	class Char,
	class Traits,
	size_t Dims,
	bool IsConstexpr,
	class Derived
>
auto& operator<<(
	std::basic_ostream<Char, Traits>& os,
	const index_base<Dims, IsConstexpr, Derived>& b
) noexcept
{
	os << "[";
	for (const auto& i : b.head()) {
		os << i << ", ";
	}
	return os << b.last() << "]";
}

}

#endif
