/*
** File Name: index_wrapper.hpp
** Author:    Aditya Ramesh
** Date:      01/13/2015
** Contact:   _@adityaramesh.com
**
** # A very technical note regarding the use of const qualifiers in return
** types.
**
** Note that the const qualifiers added to the return types in the index module
** serve an important purpose. To see why, suppose that we did not add the const
** qualifier to the return types of the const accessor functions returning
** `coord` objects. Consider a const subindex whose integer value at coordinate
** `n` is used in a static assertion (e.g. `i(n) == 0`). The _const_ accessor
** functions of the index object underlying the subindex will still return
** _non-const_ `coord` objects. When we call `operator()` on the subindex, the
** subindex will invoke `at` on the underlying index object. `at` will return a
** _non-const_ `coord` object, and `operator()` will invoke `value` on this
** object. This call should be constexpr since `operator()` is constexpr, but
** the call to `value` is not constexpr. (The reason being that a non-const
** `coord` objects may return a non-const lvalue references, so that the `coord`
** object can be modified.)
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

template <unsigned A, unsigned B, class Index>
CC_ALWAYS_INLINE constexpr
auto make_subindex(index_wrapper<Index>& w)
noexcept;

template <unsigned A, unsigned B, class Index>
CC_ALWAYS_INLINE constexpr
auto make_const_subindex(const index_wrapper<Index>& w)
noexcept;

namespace detail {

template <class Index, size_t N>
struct index_return_type_helper
{
	using coord        = decltype(std::declval<Index>().template get<N>());
	using coord_type   = std::decay_t<coord>;
	using integer      = decltype(std::declval<coord>().value());
	using integer_type = std::decay_t<integer>;

	static constexpr auto allows_static_access =
	coord_type::allows_static_access;
};

template <class Index, class Seq>
struct index_traits;

template <class Index, class Integer, Integer... Ts>
struct index_traits<Index, std::integer_sequence<Integer, Ts...>>
{
	/*
	** The integer type common to all of the `coord` objects stored by the
	** index.
	*/
	using integer = std::common_type_t<
		typename index_return_type_helper<Index, Ts>::integer_type...>;

	static constexpr auto allows_static_access = mpl::all_true<
		index_return_type_helper<Index, Ts>::allows_static_access...>;
};

}

template <class T>
class index_wrapper final
{
	using self   = index_wrapper<T>;
	using seq    = std::make_index_sequence<T::dims>;
	using traits = detail::index_traits<T, seq>;
public:
	static constexpr auto allows_static_access =
	traits::allows_static_access;

	using integer = typename traits::integer;
private:
	T m_wrapped;
public:
	CC_ALWAYS_INLINE constexpr
	explicit index_wrapper()
	noexcept : m_wrapped{} {}

	template <class... Args>
	CC_ALWAYS_INLINE constexpr
	explicit index_wrapper(in_place_t, Args&&... args)
	noexcept : m_wrapped(std::forward<Args>(args)...) {}

	template <class Integer, nd_enable_if((
		std::is_integral<Integer>::value &&
		!allows_static_access
	))>
	CC_ALWAYS_INLINE
	auto& operator=(const std::initializer_list<Integer> rhs) noexcept
	{
		nd_assert(
			rhs.size() == dims(),
			"cannot assign initializer list to index of unequal "
			"size. Size of index: $; size of initializer list: $",
			dims(), rhs.size()
		);
		fusion::copy(rhs.begin(), rhs.end(), *this);
		return *this;
	}

	CC_ALWAYS_INLINE
	auto& operator=(const index_wrapper& rhs) noexcept
	{
		fusion::copy(rhs, *this);
		return *this;
	}

	template <class Index, nd_enable_if(T::dims == Index::dims)>
	CC_ALWAYS_INLINE
	auto& operator=(const index_wrapper<Index>& rhs) noexcept
	{
		fusion::copy(rhs, *this);
		return *this;
	}

	/*
	** General accessors.
	*/

	CC_ALWAYS_INLINE constexpr
	auto wrapped() const noexcept
	{ return m_wrapped; }

	CC_ALWAYS_INLINE constexpr
	static auto dims() noexcept
	{ return T::dims; }

	/*
	** Element accessors.
	*/

	template <class Coord>
	CC_ALWAYS_INLINE constexpr
	decltype(auto) at_c(const coord_wrapper<Coord> c) noexcept
	{ return m_wrapped.template get<c.value(dims() - 1)>(); }

	template <class Coord>
	CC_ALWAYS_INLINE constexpr
	decltype(auto) at_c(const coord_wrapper<Coord> c) const noexcept
	{ return m_wrapped.template get<c.value(dims() - 1)>(); }

	template <class Coord>
	CC_ALWAYS_INLINE constexpr
	decltype(auto) at(const coord_wrapper<Coord> c) noexcept
	{ return m_wrapped.template get<c.value(dims() - 1)>().value(); }

	template <class Coord>
	CC_ALWAYS_INLINE constexpr
	decltype(auto) at(const coord_wrapper<Coord> c) const noexcept
	{ return m_wrapped.template get<c.value(dims() - 1)>().value(); }

	template <class Coord>
	CC_ALWAYS_INLINE constexpr
	decltype(auto) operator()(const coord_wrapper<Coord> c) noexcept
	{ return at(c); }

	template <class Coord>
	CC_ALWAYS_INLINE constexpr
	decltype(auto) operator()(const coord_wrapper<Coord> c) const noexcept
	{ return at(c); }

	CC_ALWAYS_INLINE constexpr
	decltype(auto) first_c() noexcept
	{ return at_c(tokens::c<0>); }

	CC_ALWAYS_INLINE constexpr
	decltype(auto) first_c() const noexcept
	{ return at_c(tokens::c<0>); }

	CC_ALWAYS_INLINE constexpr
	decltype(auto) last_c() noexcept
	{ return at_c(tokens::c<dims() - 1>); }

	CC_ALWAYS_INLINE constexpr
	decltype(auto) last_c() const noexcept
	{ return at_c(tokens::c<dims() - 1>); }

	CC_ALWAYS_INLINE constexpr
	decltype(auto) first() noexcept
	{ return at(tokens::c<0>); }

	CC_ALWAYS_INLINE constexpr
	decltype(auto) first() const noexcept
	{ return at(tokens::c<0>); }

	CC_ALWAYS_INLINE constexpr
	decltype(auto) last() noexcept
	{ return at(tokens::c<dims() - 1>); }

	CC_ALWAYS_INLINE constexpr
	decltype(auto) last() const noexcept
	{ return at(tokens::c<dims() - 1>); }

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
		constexpr auto a = l1.value(dims() - 1);
		constexpr auto b = l2.value(dims() - 1);
		return make_subindex<a, b>(*this);
	}

	template <class D1, class D2>
	CC_ALWAYS_INLINE constexpr
	auto operator()(
		const coord_wrapper<D1> l1,
		const coord_wrapper<D2> l2
	) const noexcept
	{
		constexpr auto a = l1.value(dims() - 1);
		constexpr auto b = l2.value(dims() - 1);
		return make_const_subindex<a, b>(*this);
	}

	template <nd_enable_if((dims() > 1))>
	CC_ALWAYS_INLINE constexpr
	auto head() noexcept
	{
		using namespace tokens;
		return (*this)(c<0>, end - c<1>);
	}

	template <nd_enable_if((dims() > 1))>
	CC_ALWAYS_INLINE constexpr
	auto head() const noexcept
	{
		using namespace tokens;
		return (*this)(c<0>, end - c<1>);
	}

	template <nd_enable_if((dims() > 1))>
	CC_ALWAYS_INLINE constexpr
	auto tail() noexcept
	{
		using namespace tokens;
		return (*this)(c<1>, end);
	}

	template <nd_enable_if((dims() > 1))>
	CC_ALWAYS_INLINE constexpr
	auto tail() const noexcept
	{
		using namespace tokens;
		return (*this)(c<1>, end);
	}

	template <nd_enable_if(dims() == 1)>
	CC_ALWAYS_INLINE constexpr
	auto head() noexcept
	{ return *this; }

	template <nd_enable_if(dims() == 1)>
	CC_ALWAYS_INLINE constexpr
	auto head() const noexcept
	{ return *this; }

	template <nd_enable_if(dims() == 1)>
	CC_ALWAYS_INLINE constexpr
	auto tail() noexcept
	{ return *this; }

	template <nd_enable_if(dims() == 1)>
	CC_ALWAYS_INLINE constexpr
	auto tail() const noexcept
	{ return *this; }
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

using fusion::prod;

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
			os << x << ", ";
		});
	return os << w.last() << "]";
}

}

#endif
