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
#include <ndmath/coord.hpp>
#include <ndmath/index/fusion.hpp>

namespace nd {

template <class T>
class index_wrapper;

template <uint_fast32_t A, uint_fast32_t B, class Index>
CC_ALWAYS_INLINE constexpr
auto make_subindex(index_wrapper<Index>& w)
noexcept;

template <uint_fast32_t A, uint_fast32_t B, class Index>
CC_ALWAYS_INLINE constexpr
auto make_const_subindex(const index_wrapper<Index>& w)
noexcept;

template <class T>
class index_wrapper final
{
	using self = index_wrapper<T>;

	T m_wrapped;
public:
	CC_ALWAYS_INLINE CC_CONST constexpr
	explicit index_wrapper()
	noexcept : m_wrapped{} {}

	template <class... Args>
	CC_ALWAYS_INLINE constexpr
	explicit index_wrapper(in_place_t, Args&&... args)
	noexcept : m_wrapped(std::forward<Args>(args)...) {}

	template <class Integer, nd_enable_if(std::is_integral<Integer>::value)>
	CC_ALWAYS_INLINE
	auto& operator=(const std::initializer_list<Integer> rhs) noexcept
	{
		fusion::copy(rhs.begin(), rhs.end(), *this);
		return *this;
	}

	template <class Index>
	CC_ALWAYS_INLINE
	auto& operator=(const index_wrapper<Index> rhs) noexcept
	{
		fusion::copy(rhs, *this);
		return *this;
	}

	/*
	** General accessors.
	*/

	CC_ALWAYS_INLINE CC_CONST constexpr
	auto wrapped() const noexcept
	{ return m_wrapped; }

	CC_ALWAYS_INLINE CC_CONST constexpr
	static auto dims() noexcept
	{ return tokens::c<T::dims>; }

	/*
	** Element accessors.
	*/

	template <class Loc>
	CC_ALWAYS_INLINE constexpr
	auto at(const coord_wrapper<Loc> c) noexcept ->
	decltype(std::declval<T>().template get<c.value(T::dims - 1)>())
	{
		constexpr auto value = c.value(T::dims - 1);
		return m_wrapped.template get<value>();
	}

	template <class Loc>
	CC_ALWAYS_INLINE constexpr
	auto at(const coord_wrapper<Loc> c) const noexcept ->
	decltype(std::declval<const T>().template get<c.value(T::dims - 1)>())
	{
		constexpr auto value = c.value(T::dims - 1);
		return m_wrapped.template get<value>();
	}

	template <class Loc>
	CC_ALWAYS_INLINE constexpr
	auto operator()(const coord_wrapper<Loc> c) noexcept ->
	decltype(std::declval<T>().template get<c.value(T::dims - 1)>())
	{ return at(c); }

	template <class Loc>
	CC_ALWAYS_INLINE constexpr
	auto operator()(const coord_wrapper<Loc> c) const noexcept ->
	decltype(std::declval<const T>().template get<c.value(T::dims - 1)>())
	{ return at(c); }

	CC_ALWAYS_INLINE constexpr
	auto first() noexcept ->
	decltype(std::declval<T>().template get<0>())
	{ return at(tokens::c<0>); }

	CC_ALWAYS_INLINE constexpr
	auto first() const noexcept ->
	decltype(std::declval<const T>().template get<0>())
	{ return at(tokens::c<0>); }

	CC_ALWAYS_INLINE constexpr
	auto last() noexcept ->
	decltype(std::declval<T>().template get<T::dims - 1>())
	{ return at(tokens::c<T::dims - 1>); }

	CC_ALWAYS_INLINE constexpr
	auto last() const noexcept ->
	decltype(std::declval<const T>().template get<T::dims - 1>())
	{ return at(tokens::c<T::dims - 1>); }

	/*
	** Subindex creation.
	*/

	template <class D1, class D2>
	CC_ALWAYS_INLINE constexpr
	auto operator()(
		const coord_wrapper<D1> l1,
		const coord_wrapper<D2> l2
	) noexcept
	{
		constexpr auto a = l1.value(T::dims - 1);
		constexpr auto b = l2.value(T::dims - 1);
		return make_subindex<a, b>(*this);
	}

	template <class D1, class D2>
	CC_ALWAYS_INLINE constexpr
	auto operator()(
		const coord_wrapper<D1> l1,
		const coord_wrapper<D2> l2
	) const noexcept
	{
		constexpr auto a = l1.value(T::dims - 1);
		constexpr auto b = l2.value(T::dims - 1);
		return make_const_subindex<a, b>(*this);
	}

	CC_ALWAYS_INLINE constexpr
	auto head() noexcept
	{
		using namespace tokens;
		return (*this)(c<0>, end - c<1>);
	}

	CC_ALWAYS_INLINE constexpr
	auto head() const noexcept
	{
		using namespace tokens;
		return (*this)(c<0>, end - c<1>);
	}

	CC_ALWAYS_INLINE constexpr
	auto tail() noexcept
	{
		using namespace tokens;
		return (*this)(c<1>, end);
	}

	CC_ALWAYS_INLINE constexpr
	auto tail() const noexcept
	{
		using namespace tokens;
		return (*this)(c<1>, end);
	}
};

#define nd_define_relational_op(name, symbol)     \
                                                  \
template <class Index1, class Index2>             \
CC_ALWAYS_INLINE constexpr                        \
auto operator symbol (                            \
	const index_wrapper<Index1>& lhs,         \
	const index_wrapper<Index2>& rhs          \
) noexcept                                        \
{                                                 \
	return fusion:: name ## _range(lhs, rhs); \
}

nd_define_relational_op(equal, ==)
nd_define_relational_op(not_equal, !=)
nd_define_relational_op(greater_equal, >=)
nd_define_relational_op(less_equal, <=)
nd_define_relational_op(greater, >)
nd_define_relational_op(less, <)

#undef nd_define_relational_op

template <class Char, class Traits, class Index>
auto& operator<<(
	std::basic_ostream<Char, Traits>& os,
	const index_wrapper<Index>& w
) noexcept
{
	using fusion::for_each;

	os << "[";
	for_each(w.head(),
		[&] (const auto& x)
		CC_ALWAYS_INLINE noexcept {
			os << x.value() << ", ";
		});
	return os << w.last() << "]";
}

}

#endif
