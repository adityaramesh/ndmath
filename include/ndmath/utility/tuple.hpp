/*
** File Name: tuple.hpp
** Author:    Aditya Ramesh
** Date:      08/20/2015
** Contact:   _@adityaramesh.com
**
** Custom implementation of tuple, as libc++'s implementation causes clang-3.6
** to generate spurious compiler error messages and clang-3.7 to ICE. Boost's
** implementation uses workarounds for old compilers that are causing compiler
** errors that I really don't want to debug.
**
** Missing features that I don't need to implement:
** - Relational operators.
** - Allocator functionality.
** - Reference wrappers (I'd rather just define the correct tuple type
**   beforehand).
** - Separate `tuple_size` struct -- instead, `tuple` has a static member called
**   `size`.
** - Nested tuples are disallowed, in order to make disabling the perfect
**   forwarding constructor easier in certain circumstances.
*/

#ifndef Z4A217EC0_9998_4BBF_A6A0_19A9F7AF31F1
#define Z4A217EC0_9998_4BBF_A6A0_19A9F7AF31F1

#include <ndmath/common.hpp>

namespace nd {

template <class... Ts>
class tuple;

namespace detail {

struct lvalue_reference_tag {};
struct rvalue_reference_tag {};

static constexpr auto lvalue_ref = lvalue_reference_tag{};
static constexpr auto rvalue_ref = rvalue_reference_tag{};

template <size_t N, class T>
class tuple_element
{
	static constexpr auto index = N;
	using type = T;
private:
	T m_data;
protected:
	CC_ALWAYS_INLINE constexpr
	tuple_element()
	noexcept(std::is_nothrow_default_constructible<T>::value) :
	m_data{} {}

	template <class U>
	CC_ALWAYS_INLINE constexpr explicit
	tuple_element(U&& u)
	noexcept(std::is_nothrow_constructible<T, U&&>::value) :
	m_data{std::forward<U>(u)} {}

	template <class U>
	CC_ALWAYS_INLINE
	tuple_element& operator=(U&& u)
	noexcept(std::is_nothrow_assignable<T, U&&>::value)
	{ m_data = std::forward<U>(u); return *this; }

	CC_ALWAYS_INLINE
	auto& value(lvalue_reference_tag) noexcept
	{ return m_data; }

	CC_ALWAYS_INLINE constexpr
	const auto& value(lvalue_reference_tag) const noexcept
	{ return m_data; }

	CC_ALWAYS_INLINE
	decltype(auto) value(rvalue_reference_tag) noexcept
	{ return std::move(m_data); }
};

template <class List, class... Ts>
class tuple_data;

template <size_t... Indices, class... Ts>
class tuple_data<std::index_sequence<Indices...>, Ts...> :
tuple_element<Indices, Ts>...
{
	using types = mpl::list<Ts...>;
	static constexpr auto size = types::size;

	template <size_t N>
	using base = tuple_element<N, mpl::at_c<N, types>>;
protected:
	CC_ALWAYS_INLINE constexpr
	tuple_data() noexcept(mpl::and_c<
		std::is_nothrow_default_constructible<Ts>::value...
	>::value) {}

	template <class... Us, nd_enable_if((
		sizeof...(Us) == size &&
		!mpl::or_c<
			mpl::is_specialization_of<tuple, std::decay_t<Us>>::value...
		>::value
	))>
	CC_ALWAYS_INLINE constexpr
	explicit tuple_data(Us&&... us)
	noexcept(mpl::and_c<
		std::is_nothrow_constructible<Ts, Us&&>::value...
	>::value) : base<Indices>{std::forward<Us>(us)}... {}

	template <class... Us, nd_enable_if((sizeof...(Us) == size))>
	CC_ALWAYS_INLINE constexpr
	tuple_data(const tuple_data<Us...>& rhs)
	noexcept(mpl::and_c<
		std::is_nothrow_constructible<Ts, const Us&>::value...
	>::value) : base<Indices>{rhs.template get<Indices>()}... {}

	template <class... Us, nd_enable_if((sizeof...(Us) == size))>
	CC_ALWAYS_INLINE
	tuple_data(tuple_data<Us...>&& rhs)
	noexcept(mpl::and_c<
		std::is_nothrow_constructible<Ts, Us&&>::value...
	>::value) : base<Indices>{std::move(rhs).template get<Indices>()}... {}

	template <class... Us, nd_enable_if((sizeof...(Us) == size))>
	CC_ALWAYS_INLINE
	tuple_data& operator=(const tuple<Us...>& rhs)
	noexcept(mpl::and_c<
		std::is_nothrow_assignable<Ts, const Us&>::value...
	>::value)
	{
		using expander = int[];
		(void)expander{0, ((void)base<Indices>::operator=(
			rhs.template get<Indices>()), 0)...};
		return *this;
	}

	template <class... Us, nd_enable_if((sizeof...(Us) == size))>
	CC_ALWAYS_INLINE
	tuple_data& operator=(tuple<Us...>&& rhs)
	noexcept(mpl::and_c<
		std::is_nothrow_assignable<Ts, Us&&>::value...
	>::value)
	{
		using expander = int[];
		(void)expander{0, ((void)base<Indices>::operator=(
			std::move(rhs).template get<Indices>()), 0)...};
		return *this;
	}
public:
	template <size_t N>
	CC_ALWAYS_INLINE
	decltype(auto) get() & noexcept
	{
		static_assert(N < size, "Index out of bounds.");
		return base<N>::value(lvalue_ref);
	}

	template <size_t N>
	CC_ALWAYS_INLINE constexpr
	decltype(auto) get() const & noexcept
	{
		static_assert(N < size, "Index out of bounds.");
		return base<N>::value(lvalue_ref);
	}

	template <size_t N>
	CC_ALWAYS_INLINE
	decltype(auto) get() && noexcept
	{
		static_assert(N < size, "Index out of bounds.");
		return base<N>::value(rvalue_ref);
	}
};

}

template <class... Ts>
class tuple : detail::tuple_data<std::make_index_sequence<sizeof...(Ts)>, Ts...>
{
	using base = detail::tuple_data<std::make_index_sequence<sizeof...(Ts)>, Ts...>;
public:
	using base::get;
	static constexpr auto size = sizeof...(Ts);

	CC_ALWAYS_INLINE constexpr
	tuple() noexcept(std::is_nothrow_default_constructible<base>::value) {}

	template <class... Us>
	CC_ALWAYS_INLINE constexpr
	tuple(Us&&... us)
	noexcept(std::is_nothrow_constructible<base, Us&&...>::value) :
	base{std::forward<Us>(us)...} {}

	template <class T>
	CC_ALWAYS_INLINE
	tuple& operator=(T&& t)
	noexcept(std::is_nothrow_assignable<base, T&&>::value)
	{ base::operator=(std::forward<T>(t)); return *this; }
};

template <class... Ts>
CC_ALWAYS_INLINE constexpr
auto make_tuple(Ts&&... ts)
noexcept(std::is_nothrow_constructible<tuple<Ts&&...>, Ts&&...>::value)
{ return tuple<std::decay_t<Ts>...>{std::forward<Ts>(ts)...}; }

template <size_t N, class Tuple, nd_enable_if((
	mpl::is_specialization_of<tuple, std::decay_t<Tuple>>::value
))>
CC_ALWAYS_INLINE constexpr
decltype(auto) get(Tuple&& t) noexcept
{ return std::forward<Tuple>(t).template get<N>(); }

}

#endif
