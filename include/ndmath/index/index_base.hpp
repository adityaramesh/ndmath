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
#include <ndmath/index/expression_forward.hpp>

namespace nd {

/*
** Derived classes need only implement `operator()(const size_t&)`.
*/
template <size_t Dims, class Derived>
class index_base
{
public:
	static constexpr auto dims()
	noexcept { return Dims; }

	/*
	** Element accessors.
	*/

	CC_ALWAYS_INLINE
	auto& operator()(const size_t& i)
	noexcept { return (*this)()(i); }

	CC_ALWAYS_INLINE
	const auto& operator()(const size_t& i)
	const noexcept { return (*this)()(i); }

	template <class Loc>
	CC_ALWAYS_INLINE auto&
	operator()(const Loc& l)
	noexcept { return (*this)(l(dims)); }

	template <class Loc>
	CC_ALWAYS_INLINE const auto&
	operator()(const Loc& l)
	const noexcept { return (*this)(l(dims)); }

	CC_ALWAYS_INLINE
	auto& first() noexcept
	{ return (*this)(0); }

	CC_ALWAYS_INLINE
	const auto& first() const noexcept
	{ return (*this)(0); }

	CC_ALWAYS_INLINE
	auto& last() noexcept
	{ return (*this)(dims()); }

	CC_ALWAYS_INLINE
	const auto& last() const noexcept
	{ return (*this)(dims()); }

	/*
	** Subindex creation.
	*/

	CC_ALWAYS_INLINE
	auto& operator()(const size_t& a, const size_t& b)
	noexcept { return make_subindex(a, b, *this); }

	CC_ALWAYS_INLINE
	const auto& operator()(const size_t& a, const size_t& b)
	const noexcept { return make_subindex(a, b, *this); }

	template <class Loc>
	CC_ALWAYS_INLINE auto&
	operator()(const Loc& a, const size_t& b)
	noexcept { return make_cl_subindex<a(dims())>(b, *this); }

	template <class Loc>
	CC_ALWAYS_INLINE const auto&
	operator()(const Loc& a, const size_t& b)
	const noexcept { return make_cl_subindex<a(dims())>(b, *this); }

	template <class Loc>
	CC_ALWAYS_INLINE auto&
	operator()(const size_t& a, const Loc& b)
	noexcept { return make_cr_subindex<b(dims())>(a, *this); }

	template <class Loc>
	CC_ALWAYS_INLINE const auto&
	operator()(const size_t& a, const Loc& b)
	const noexcept { return make_cr_subindex<b(dims())>(a, *this); }

	template <class Loc1, class Loc2>
	CC_ALWAYS_INLINE auto&
	operator()(const Loc1& a, const Loc2& b)
	noexcept { return make_c_subindex<a(dims()), b(dims())>(*this); }

	template <class Loc1, class Loc2>
	CC_ALWAYS_INLINE const auto&
	operator()(const Loc1& a, const Loc2& b)
	const noexcept { return make_c_subindex<a(dims()), b(dims())>(*this); }

	CC_ALWAYS_INLINE
	auto& head() noexcept
	{ return (*this)(c<1>, end); }

	CC_ALWAYS_INLINE
	const auto& head() const noexcept
	{ return (*this)(c<1>, end); }

	CC_ALWAYS_INLINE
	auto& tail() noexcept
	{ return (*this)(c<0>, end - c<1>); }

	CC_ALWAYS_INLINE
	const auto& tail() const noexcept
	{ return (*this)(c<0>, end - c<1>); }

	/*
	** Iteration.
	*/

	CC_ALWAYS_INLINE
	auto begin() noexcept
	{ return (*this).begin(); }

	CC_ALWAYS_INLINE
	auto begin() const noexcept
	{ return (*this).begin(); }

	CC_ALWAYS_INLINE
	auto end() noexcept
	{ return (*this).end(); }

	CC_ALWAYS_INLINE
	auto end() const noexcept
	{ return (*this).end(); }
};

template <
	size_t Dims1,
	size_t Dims2,
	class Derived1,
	class Derived2
>
CC_ALWAYS_INLINE auto
operator==(
	const index_base<Dims1, Derived1>& lhs,
	const index_base<Dims2, Derived2>& rhs
) noexcept
{ return boost::equal(lhs, rhs); }

template <
	size_t Dims1,
	size_t Dims2,
	class Derived1,
	class Derived2
>
CC_ALWAYS_INLINE auto
operator!=(
	const index_base<Dims1, Derived1>& lhs,
	const index_base<Dims2, Derived2>& rhs
) noexcept
{ return !(lhs == rhs); }

template <
	size_t Dims1,
	size_t Dims2,
	class Derived1,
	class Derived2
>
CC_ALWAYS_INLINE auto
operator,(
	index_base<Dims1, Derived1>& lhs,
	index_base<Dims2, Derived2>& rhs
) noexcept
{ return make_composite_index(lhs, rhs); }

template <
	size_t Dims1,
	size_t Dims2,
	class Derived1,
	class Derived2
>
CC_ALWAYS_INLINE auto
operator,(
	const index_base<Dims1, Derived1>& lhs,
	index_base<Dims2, Derived2>& rhs
) noexcept
{ return make_composite_index(lhs, rhs); }

template <
	size_t Dims1,
	size_t Dims2,
	class Derived1,
	class Derived2
>
CC_ALWAYS_INLINE auto
operator,(
	index_base<Dims1, Derived1>& lhs,
	const index_base<Dims2, Derived2>& rhs
) noexcept
{ return make_composite_index(lhs, rhs); }

template <
	size_t Dims1,
	size_t Dims2,
	class Derived1,
	class Derived2
>
CC_ALWAYS_INLINE auto
operator,(
	const index_base<Dims1, Derived1>& lhs,
	const index_base<Dims2, Derived2>& rhs
) noexcept
{ return make_composite_index(lhs, rhs); }

template <size_t Dims, class Derived>
CC_ALWAYS_INLINE auto
begin(index_base<Dims, Derived>& b) noexcept
{ return b.begin(); }

template <size_t Dims, class Derived>
CC_ALWAYS_INLINE auto
begin(const index_base<Dims, Derived>& b) noexcept
{ return b.begin(); }

template <size_t Dims, class Derived>
CC_ALWAYS_INLINE auto
end(index_base<Dims, Derived>& b) noexcept
{ return b.end(); }

template <size_t Dims, class Derived>
CC_ALWAYS_INLINE auto
end(const index_base<Dims, Derived>& b) noexcept
{ return b.end(); }

template <size_t Dims, class Derived>
CC_ALWAYS_INLINE auto
off(const index_base<Dims, Derived>& b) noexcept
{
	return boost::accumulate(b, size_t{1},
		[] (const auto& x, const auto& y)
		CC_ALWAYS_INLINE CC_CONST noexcept
		{ return x * y; });
}

template <class Char, class Traits, size_t Dims, class Derived>
auto& operator<<(
	std::basic_ostream<Char, Traits>& os,
	const index_base<Dims, Derived>& b
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
