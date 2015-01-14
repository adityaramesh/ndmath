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
**
** It is not obvious why we need the `Result` template parameter. Consider the
** case where we have a subindex over an arithmetic expression involving
** non-constexpr indices. Then the subindex will not be constexpr, but its
** values cannot be references, since they are computed lazily.
**
** For constexpr index classes (i.e. `IsConstexpr = true`), there is only one
** version of `operator()` (marked `const`), since the result is always
** available during compile-time. Hence the `Result` template parameter is never
** used.
*/
template <
	size_t Dims, bool IsConstexpr,
	class Result, class ConstResult,
	class Derived
>
class index_base;

template <size_t Dims, class Result, class ConstResult, class Derived>
class index_base<Dims, false, Result, ConstResult, Derived>
{
public:
	using index_type = std::decay_t<Result>;
private:
	using self       = index_base<Dims, false, Result, ConstResult, Derived>;
	using index_list = std::initializer_list<index_type>;
public:
	static constexpr auto is_constexpr = false;

	/*
	** We call these typedefs `result` and `const_result` instead of `value`
	** and `const_value`, because the latter names would imply that the
	** types must be values, and not references.
	*/
	using result         = Result;
	using const_result   = ConstResult;
	using iterator       = index_iterator<self>;
	using const_iterator = index_iterator<const self>;

	static CC_ALWAYS_INLINE CC_CONST
	constexpr auto dims() noexcept { return Dims; }

	CC_ALWAYS_INLINE CC_CONST constexpr
	auto& operator()() noexcept
	{ return static_cast<Derived&>(*this); }

	CC_ALWAYS_INLINE CC_CONST constexpr
	const auto& operator()() const noexcept
	{ return static_cast<const Derived&>(*this); }

	CC_ALWAYS_INLINE index_base&
	operator=(const index_base& rhs) noexcept
	{ boost::copy(rhs, begin()); return *this; }

	template <
		size_t Dims_,
		bool IsConstexpr_,
		class Result_,
		class ConstResult_,
		class Derived_
	>
	CC_ALWAYS_INLINE index_base&
	operator=(const index_base<Dims_, IsConstexpr_, Result_, ConstResult_, Derived_>& rhs)
	noexcept { boost::copy(rhs, begin()); return *this; }

	CC_ALWAYS_INLINE index_base&
	operator=(const index_list& rhs) noexcept
	{ boost::copy(rhs, begin()); return *this; }

	/*
	** Element accessors.
	*/

	template <class T>
	CC_ALWAYS_INLINE
	Result operator()(const T& n)
	noexcept { return (*this)()(n); }

	template <class T>
	CC_ALWAYS_INLINE
	ConstResult operator()(const T& n)
	const noexcept { return (*this)()(n); }

	template <class D>
	CC_ALWAYS_INLINE
	Result operator()(const location_base<D>& l)
	noexcept { return (*this)(l(dims() - 1)); }

	template <class D>
	CC_ALWAYS_INLINE
	ConstResult operator()(const location_base<D>& l)
	const noexcept { return (*this)(l(dims() - 1)); }

	CC_ALWAYS_INLINE
	Result first() noexcept
	{ return (*this)(size_t{0}); }

	CC_ALWAYS_INLINE
	ConstResult first() const noexcept
	{ return (*this)(size_t{0}); }

	CC_ALWAYS_INLINE
	Result last() noexcept
	{ return (*this)(dims() - 1); }

	CC_ALWAYS_INLINE
	ConstResult last() const noexcept
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

template <size_t Dims, class Result, class ConstResult, class Derived>
class index_base<Dims, true, Result, ConstResult, Derived>
{
public:
	using index_type = std::decay_t<Result>;
private:
	using self = index_base<Dims, true, Result, ConstResult, Derived>;
public:
	static constexpr auto is_constexpr = true;

	using result         = Result;
	using const_result   = ConstResult;
	using iterator       = index_iterator<const self>;
	using const_iterator = iterator;

	static CC_ALWAYS_INLINE CC_CONST
	constexpr auto dims() noexcept { return Dims; }

	CC_ALWAYS_INLINE CC_CONST constexpr
	const auto& operator()() const noexcept
	{ return static_cast<const Derived&>(*this); }

	/*
	** Element accessors.
	*/

	template <class T>
	CC_ALWAYS_INLINE CC_CONST
	constexpr ConstResult operator()(const T& n)
	const noexcept { return (*this)()(n); }

	template <class D>
	CC_ALWAYS_INLINE CC_CONST
	constexpr ConstResult operator()(const location_base<D>& l)
	const noexcept { return (*this)(l(dims() - 1)); }

	CC_ALWAYS_INLINE CC_CONST
	constexpr ConstResult first() const noexcept
	{ return (*this)(size_t{0}); }

	CC_ALWAYS_INLINE CC_CONST
	constexpr ConstResult last() const noexcept
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
	class Result1,
	class Result2,
	class ConstResult1,
	class ConstResult2,
	class Derived1,
	class Derived2
>
CC_ALWAYS_INLINE auto
operator==(
	const index_base<Dims1, IsConstexpr1, Result1, ConstResult1, Derived1>& lhs,
	const index_base<Dims2, IsConstexpr2, Result2, ConstResult2, Derived2>& rhs
) noexcept { return boost::equal(lhs, rhs); }

template <
	size_t Dims1,
	size_t Dims2,
	class Result1,
	class Result2,
	class ConstResult1,
	class ConstResult2,
	class Derived1,
	class Derived2
>
CC_ALWAYS_INLINE CC_CONST constexpr
auto operator==(
	const index_base<Dims1, true, Result1, ConstResult1, Derived1>& lhs,
	const index_base<Dims2, true, Result2, ConstResult2, Derived2>& rhs
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
	class Result1,
	class Result2,
	class ConstResult1,
	class ConstResult2,
	class Derived1,
	class Derived2
>
CC_ALWAYS_INLINE auto
operator!=(
	const index_base<Dims1, IsConstexpr1, Result1, ConstResult1, Derived1>& lhs,
	const index_base<Dims2, IsConstexpr2, Result2, ConstResult2, Derived2>& rhs
) noexcept { return !(lhs == rhs); }

template <
	size_t Dims1,
	size_t Dims2,
	class Result1,
	class Result2,
	class ConstResult1,
	class ConstResult2,
	class Derived1,
	class Derived2
>
CC_ALWAYS_INLINE CC_CONST constexpr
auto operator!=(
	const index_base<Dims1, true, Result1, ConstResult1, Derived1>& lhs,
	const index_base<Dims2, true, Result2, ConstResult2, Derived2>& rhs
) noexcept { return !(lhs == rhs); }

template <
	size_t Dims,
	bool IsConstexpr,
	class Result,
	class ConstResult,
	class Derived
>
CC_ALWAYS_INLINE auto
begin(index_base<Dims, IsConstexpr, Result, ConstResult, Derived>& b)
noexcept { return b.begin(); }

template <
	size_t Dims,
	bool IsConstexpr,
	class Result,
	class ConstResult,
	class Derived
>
CC_ALWAYS_INLINE auto
begin(const index_base<Dims, IsConstexpr, Result, ConstResult, Derived>& b)
noexcept { return b.begin(); }

template <
	size_t Dims,
	class Result,
	class ConstResult,
	class Derived
>
CC_ALWAYS_INLINE CC_CONST constexpr
auto begin(const index_base<Dims, true, Result, ConstResult, Derived>& b)
noexcept { return b.begin(); }

template <
	size_t Dims,
	bool IsConstexpr,
	class Result,
	class ConstResult,
	class Derived
>
CC_ALWAYS_INLINE auto
end(index_base<Dims, IsConstexpr, Result, ConstResult, Derived>& b)
noexcept { return b.end(); }

template <
	size_t Dims,
	bool IsConstexpr,
	class Result,
	class ConstResult,
	class Derived
>
CC_ALWAYS_INLINE auto
end(const index_base<Dims, IsConstexpr, Result, ConstResult, Derived>& b)
noexcept { return b.end(); }

template <
	size_t Dims,
	class Result,
	class ConstResult,
	class Derived
>
CC_ALWAYS_INLINE CC_CONST constexpr
auto end(const index_base<Dims, true, Result, ConstResult, Derived>& b)
noexcept { return b.end(); }

template <
	class Char,
	class Traits,
	size_t Dims,
	bool IsConstexpr,
	class Result,
	class ConstResult,
	class Derived
>
auto& operator<<(
	std::basic_ostream<Char, Traits>& os,
	const index_base<Dims, IsConstexpr, Result, ConstResult, Derived>& b
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
