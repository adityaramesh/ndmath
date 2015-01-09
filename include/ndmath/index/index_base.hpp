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

namespace nd {

/*
** Derived classes need only implement `operator()(const size_t&)`.
*/
template <size_t Dims, bool IsConstexpr, class Derived>
class index_base;

template <size_t Dims, class Derived>
class index_base<Dims, false, Derived>
{
public:
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

	CC_ALWAYS_INLINE
	auto operator()(const size_t& i)
	noexcept { return (*this)()(i); }

	CC_ALWAYS_INLINE
	auto operator()(const size_t& i)
	const noexcept { return (*this)()(i); }

	template <class Loc>
	CC_ALWAYS_INLINE auto
	operator()(const Loc& l)
	noexcept { return (*this)(l(dims())); }

	template <class Loc>
	CC_ALWAYS_INLINE auto
	operator()(const Loc& l)
	const noexcept { return (*this)(l(dims())); }

	CC_ALWAYS_INLINE
	auto first() noexcept
	{ return (*this)(size_t{0}); }

	CC_ALWAYS_INLINE
	auto first() const noexcept
	{ return (*this)(size_t{0}); }

	CC_ALWAYS_INLINE
	auto last() noexcept
	{ return (*this)(dims() - 1); }

	CC_ALWAYS_INLINE
	auto last() const noexcept
	{ return (*this)(dims() - 1); }

	/*
	** Subindex creation.
	*/

	CC_ALWAYS_INLINE
	auto operator()(const size_t& a, const size_t& b)
	noexcept { return make_subindex(a, b, *this); }

	CC_ALWAYS_INLINE
	auto operator()(const size_t& a, const size_t& b)
	const noexcept { return make_subindex(a, b, *this); }

	template <class Loc>
	CC_ALWAYS_INLINE
	auto operator()(const Loc& a, const size_t& b)
	noexcept { return make_cl_subindex<a(dims())>(b, *this); }

	template <class Loc>
	CC_ALWAYS_INLINE
	auto operator()(const Loc& a, const size_t& b)
	const noexcept { return make_cl_subindex<a(dims())>(b, *this); }

	template <class Loc>
	CC_ALWAYS_INLINE auto
	operator()(const size_t& a, const Loc& b)
	noexcept { return make_cr_subindex<b(dims())>(a, *this); }

	template <class Loc>
	CC_ALWAYS_INLINE auto
	operator()(const size_t& a, const Loc& b)
	const noexcept { return make_cr_subindex<b(dims())>(a, *this); }

	template <class Loc1, class Loc2>
	CC_ALWAYS_INLINE auto
	operator()(const Loc1& a, const Loc2& b)
	noexcept { return make_c_subindex<a(dims()), b(dims())>(*this); }

	template <class Loc1, class Loc2>
	CC_ALWAYS_INLINE auto
	operator()(const Loc1& a, const Loc2& b)
	const noexcept { return make_c_subindex<a(dims()), b(dims())>(*this); }

	CC_ALWAYS_INLINE
	auto head() noexcept
	{ return (*this)(tokens::c<1>, tokens::end); }

	CC_ALWAYS_INLINE
	auto head() const noexcept
	{ return (*this)(tokens::c<1>, tokens::end); }

	CC_ALWAYS_INLINE
	auto tail() noexcept
	{ return (*this)(tokens::c<0>, tokens::end - tokens::c<1>); }

	CC_ALWAYS_INLINE
	auto tail() const noexcept
	{ return (*this)(tokens::c<0>, tokens::end - tokens::c<1>); }

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

template <size_t Dims, class Derived>
class index_base<Dims, true, Derived>
{
public:
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

	template <class Loc>
	CC_ALWAYS_INLINE CC_CONST
	constexpr auto operator()(const Loc& l)
	const noexcept { return (*this)(l(dims())); }

	CC_ALWAYS_INLINE CC_CONST
	constexpr auto first() const noexcept
	{ return (*this)(size_t{0}); }

	CC_ALWAYS_INLINE CC_CONST
	constexpr auto last() const noexcept
	{ return (*this)(dims() - 1); }

	/*
	** Subindex creation.
	*/

	CC_ALWAYS_INLINE CC_CONST constexpr
	auto operator()(const size_t& a, const size_t& b)
	const noexcept { return make_subindex(a, b, *this); }

	template <class Loc>
	CC_ALWAYS_INLINE CC_CONST constexpr
	auto operator()(const Loc& a, const size_t& b)
	const noexcept { return make_cl_subindex<a(dims())>(b, *this); }

	template <class Loc>
	CC_ALWAYS_INLINE CC_CONST constexpr
	auto operator()(const size_t& a, const Loc& b)
	const noexcept { return make_cr_subindex<b(dims())>(a, *this); }

	template <class Loc1, class Loc2>
	CC_ALWAYS_INLINE CC_CONST constexpr
	auto operator()(const Loc1& a, const Loc2& b)
	const noexcept { return make_c_subindex<a(dims()), b(dims())>(*this); }

	CC_ALWAYS_INLINE CC_CONST
	constexpr auto head() const noexcept
	{ return (*this)(tokens::c<1>, tokens::end); }

	CC_ALWAYS_INLINE CC_CONST
	constexpr auto tail() const noexcept
	{ return (*this)(tokens::c<0>, tokens::end - tokens::c<1>); }

	/*
	** Iteration.
	*/

	CC_ALWAYS_INLINE CC_CONST
	constexpr auto begin() const noexcept
	{ return (*this).begin(); }

	CC_ALWAYS_INLINE CC_CONST
	constexpr auto end() const noexcept
	{ return (*this).end(); }
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
	for (auto i = 0; i != b.dims(); ++i) {
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
