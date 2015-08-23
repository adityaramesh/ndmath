/*
** File Name: zip_with_iterator.hpp
** Author:    Aditya Ramesh
** Date:      08/17/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z6097DA22_2BEE_4CEF_90B2_1FD8B3235A33
#define Z6097DA22_2BEE_4CEF_90B2_1FD8B3235A33

#include <ndmath/common.hpp>
#include <ndmath/utility/fusion.hpp>

namespace nd {
namespace detail {

template <class T>
using difference_type = typename T::difference_type;

template <class T>
using reference = typename T::reference;

template <class T>
using iterator_category = typename T::iterator_category;

}

/*
** XXX: Currently, the implementation assumes that all iterators `Ts` are of
** random-access type. Note: the template parameter `Ts` is used for perfect
** forwarding, so it's safe to have const references and rvalues.
*/
template <class Func, class... Ts>
class zip_with_iterator final
{
	using traits_list = mpl::list<std::iterator_traits<std::decay_t<Ts>>...>;
	using dt_list     = mpl::transform<traits_list, mpl::quote<detail::difference_type>>;
	using ref_list    = mpl::transform<traits_list, mpl::quote<detail::reference>>;
	using cat_list    = mpl::transform<traits_list, mpl::quote<detail::iterator_category>>;
	
	static_assert(
		mpl::apply_list<
			mpl::bind_front<
				mpl::quote<mpl::is_same>,
				std::random_access_iterator_tag
			>,
			cat_list
		>::value,
		"Implementation of zip_with_iterator currently assumes that "
		"both iterators are of random-access type."
	);
public:
	using reference = std::result_of_t<Func(
		typename std::iterator_traits<std::decay_t<Ts>>::reference...
	)>;

	using difference_type   = mpl::apply_list<mpl::quote<std::common_type_t>, dt_list>;
	using value_type        = std::decay_t<reference>;
	using pointer           = value_type*;
	using const_pointer     = const value_type*;
	using iterator_category = std::random_access_iterator_tag;
private:
	tuple<Ts...> m_iters;
	const Func& m_func;
public:
	CC_ALWAYS_INLINE constexpr
	explicit zip_with_iterator(Ts&&... ts, const Func& func)
	noexcept : m_iters{std::forward<Ts>(ts)...}, m_func{func} {}

	CC_ALWAYS_INLINE constexpr
	zip_with_iterator(const zip_with_iterator& rhs)
	noexcept : m_iters{rhs.m_iters}, m_func{rhs.m_func} {}

	CC_ALWAYS_INLINE constexpr
	zip_with_iterator(zip_with_iterator&& rhs)
	noexcept : m_iters{std::move(rhs.m_iters)}, m_func{std::move(rhs.m_func)} {}

	CC_ALWAYS_INLINE
	auto& operator=(const zip_with_iterator& rhs)
	noexcept
	{
		m_iters = rhs.m_iters;
		return *this;
	}

	CC_ALWAYS_INLINE
	auto& operator=(zip_with_iterator&& rhs)
	noexcept
	{
		m_iters = std::move(rhs.m_iters);
		return *this;
	}

	CC_ALWAYS_INLINE
	auto operator*()
	nd_deduce_noexcept_and_return_type(expand_with(m_iters,
		[] (auto& x) CC_ALWAYS_INLINE noexcept { return *x; },
	m_func))

	CC_ALWAYS_INLINE constexpr
	auto operator*() const
	nd_deduce_noexcept_and_return_type(expand_with(m_iters,
		[] (auto& x) CC_ALWAYS_INLINE noexcept { return *x; },
	m_func))

	CC_ALWAYS_INLINE
	pointer operator->()
	noexcept(noexcept(expand_with(m_iters,
		[] (auto& x) CC_ALWAYS_INLINE noexcept { return *x; },
	m_func)))
	{
		return &expand_with(m_iters,
			[] (auto& x) CC_ALWAYS_INLINE noexcept { return *x; },
			m_func);
	}

	CC_ALWAYS_INLINE
	const_pointer operator->() const
	noexcept(noexcept(expand_with(m_iters,
		[] (auto& x) CC_ALWAYS_INLINE noexcept { return *x; },
	m_func)))
	{
		return &expand_with(m_iters,
			[] (auto& x) CC_ALWAYS_INLINE noexcept { return *x; },
			m_func);
	}

	/*
	** We don't use subscripting in the noexcept specifications below,
	** because the capture of the parameter `n` by a lambda within the
	** noexcept specification does not compile under Clang 3.6.
	*/
	CC_ALWAYS_INLINE 
	auto operator[](const difference_type n)
	noexcept(noexcept(expand_with(m_iters,
		[] (auto& x) CC_ALWAYS_INLINE noexcept { return *x; },
	m_func)))
	{
		return expand_with(m_iters,
			[&] (auto& x) CC_ALWAYS_INLINE noexcept { return x[n]; },
		m_func);
	}

	CC_ALWAYS_INLINE 
	auto operator[](const difference_type n) const
	noexcept(noexcept(expand_with(m_iters,
		[] (auto& x) CC_ALWAYS_INLINE noexcept { return *x; },
	m_func)))
	{
		return expand_with(m_iters,
			[&] (auto& x) CC_ALWAYS_INLINE noexcept { return x[n]; },
		m_func);
	}

	CC_ALWAYS_INLINE auto
	operator++(int) noexcept
	{ auto t = *this; ++(*this); return t; }

	CC_ALWAYS_INLINE auto
	operator--(int) noexcept
	{ auto t = *this; --(*this); return t; }

	CC_ALWAYS_INLINE auto&
	operator++() noexcept
	{
		for_each(m_iters, [] (auto& x) CC_ALWAYS_INLINE noexcept { ++x; });
		return *this;
	}

	CC_ALWAYS_INLINE auto&
	operator--() noexcept
	{
		for_each(m_iters, [] (auto& x) CC_ALWAYS_INLINE noexcept { --x; });
		return *this;
	}

	CC_ALWAYS_INLINE auto&
	operator+=(const difference_type n) noexcept
	{
		for_each(m_iters, [&] (auto& x) CC_ALWAYS_INLINE noexcept { x += n; });
		return *this;
	}

	CC_ALWAYS_INLINE auto&
	operator-=(const difference_type n) noexcept
	{
		for_each(m_iters, [&] (auto& x) CC_ALWAYS_INLINE noexcept { x -= n; });
		return *this;
	}

	CC_ALWAYS_INLINE constexpr bool
	operator==(const zip_with_iterator& rhs)
	const noexcept { return get<0>(m_iters) == get<0>(rhs.m_iters); }

	CC_ALWAYS_INLINE constexpr bool
	operator!=(const zip_with_iterator& rhs)
	const noexcept { return get<0>(m_iters) != get<0>(rhs.m_iters); }

	CC_ALWAYS_INLINE constexpr bool
	operator>=(const zip_with_iterator& rhs)
	const noexcept { return get<0>(m_iters) >= get<0>(rhs.m_iters); }

	CC_ALWAYS_INLINE constexpr bool
	operator<=(const zip_with_iterator& rhs)
	const noexcept { return get<0>(m_iters) <= get<0>(rhs.m_iters); }

	CC_ALWAYS_INLINE constexpr bool
	operator>(const zip_with_iterator& rhs)
	const noexcept { return get<0>(m_iters) > get<0>(rhs.m_iters); }

	CC_ALWAYS_INLINE constexpr bool
	operator<(const zip_with_iterator& rhs)
	const noexcept { return get<0>(m_iters) < get<0>(rhs.m_iters); }

	CC_ALWAYS_INLINE constexpr auto
	operator-(const zip_with_iterator& rhs)
	const noexcept { return get<0>(m_iters) - get<0>(rhs.m_iters); }
};

template <class Func, class... Ts>
CC_ALWAYS_INLINE constexpr
auto operator+(
	const zip_with_iterator<Func, Ts...>& x,
	const typename zip_with_iterator<Func, Ts...>::difference_type n
) noexcept
{
	auto t = x;
	t += n;
	return t;
}

template <class Func, class... Ts>
CC_ALWAYS_INLINE constexpr
auto operator+(
	const typename zip_with_iterator<Func, Ts...>::difference_type n,
	const zip_with_iterator<Func, Ts...>& x
) noexcept
{
	auto t = x;
	t += n;
	return t;
}

template <class Func, class... Ts>
CC_ALWAYS_INLINE constexpr
auto operator-(
	const zip_with_iterator<Func, Ts...>& x,
	const typename zip_with_iterator<Func, Ts...>::difference_type n
) noexcept
{
	auto t = x;
	t -= n;
	return t;
}

template <class Func, class... Ts, nd_enable_if((
	!mpl::and_c<
		mpl::is_specialization_of<array_wrapper, Ts>::value...
	>::value &&
	// Ensure that `Func` is defined for the reference types of the ranges
	// `Ts...`.
	std::is_same<
		decltype(std::declval<Func>()(*std::declval<Ts>().begin()...)),
		decltype(std::declval<Func>()(*std::declval<Ts>().begin()...))
	>::value
))>
CC_ALWAYS_INLINE constexpr
auto zip_with(const Func& f, const Ts&... ts)
{
	using iterator = zip_with_iterator<Func, std::decay_t<decltype(
		std::declval<Ts>().begin()
	)>...>;

	return boost::make_iterator_range(
		iterator{ts.begin()..., f},
		iterator{ts.end()..., f}
	);
}

}

#endif
