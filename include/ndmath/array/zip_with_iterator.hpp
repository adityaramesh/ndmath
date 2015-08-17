/*
** File Name: zip_with_iterator.hpp
** Author:    Aditya Ramesh
** Date:      08/17/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z6097DA22_2BEE_4CEF_90B2_1FD8B3235A33
#define Z6097DA22_2BEE_4CEF_90B2_1FD8B3235A33

namespace nd {

/*
** XXX: Currently, the implementation assumes that both `It1` and `It2` are
** random-access iterators.
*/
template <class It1, class It2, class Func>
class zip_with_iterator final
{
	using traits_1 = std::iterator_traits<It1>;
	using traits_2 = std::iterator_traits<It2>;
	using dt_1     = typename traits_1::difference_type;
	using dt_2     = typename traits_2::difference_type;
	using ref_1    = typename traits_1::reference;
	using ref_2    = typename traits_2::reference;
	using cat_1    = typename traits_1::iterator_category;
	using cat_2    = typename traits_2::iterator_category;

	static_assert(
		std::is_same<cat_1, std::random_access_iterator_tag>::value &&
		std::is_same<cat_2, std::random_access_iterator_tag>::value,
		"Implementation of zip_with_iterator currently assumes that "
		"both iterators are of random-access type."
	);
public:
	using difference_type   = std::common_type_t<dt_1, dt_2>;
	using reference         = std::result_of_t<Func(ref_1, ref_2)>;
	using value_type        = std::decay_t<reference>;
	using pointer           = value_type*;
	using const_pointer     = const value_type*;
	using iterator_category = std::random_access_iterator_tag;
private:
	It1 m_it1;
	It2 m_it2;
	const Func& m_func;
public:
	CC_ALWAYS_INLINE constexpr
	explicit zip_with_iterator(It1&& it1, It2&& it2, const Func& func)
	noexcept : m_it1{std::move(it1)}, m_it2{std::move(it2)}, m_func{func} {}

	CC_ALWAYS_INLINE constexpr
	zip_with_iterator(const zip_with_iterator& rhs)
	noexcept : m_it1{rhs.m_it1}, m_it2{rhs.m_it2}, m_func{rhs.m_func} {}

	CC_ALWAYS_INLINE constexpr
	zip_with_iterator(zip_with_iterator&& rhs)
	noexcept : m_it1{std::move(rhs.m_it1)}, m_it2{std::move(rhs.m_it2)},
	m_func{std::move(rhs.m_func)} {}

	CC_ALWAYS_INLINE
	auto& operator=(const zip_with_iterator& rhs)
	noexcept
	{
		m_it1 = rhs.m_it1;
		m_it2 = rhs.m_it2;
		return *this;
	}

	CC_ALWAYS_INLINE
	auto& operator=(zip_with_iterator&& rhs)
	noexcept
	{
		m_it1 = std::move(rhs.m_it1);
		m_it2 = std::move(rhs.m_it2);
		return *this;
	}

	CC_ALWAYS_INLINE
	auto operator*()
	nd_deduce_noexcept_and_return_type(m_func(*m_it1, *m_it2))

	CC_ALWAYS_INLINE constexpr
	auto operator*() const
	nd_deduce_noexcept_and_return_type(m_func(*m_it1, *m_it2))

	CC_ALWAYS_INLINE
	pointer operator->()
	noexcept(noexcept(m_func(*m_it1, *m_it2)))
	{ return &m_func(*m_it1, *m_it2); }

	CC_ALWAYS_INLINE constexpr
	const_pointer operator->() const
	noexcept(noexcept(m_func(*m_it1, *m_it2)))
	{ return &m_func(*m_it1, *m_it2); }

	CC_ALWAYS_INLINE 
	auto operator[](const difference_type n)
	nd_deduce_noexcept_and_return_type(m_func(m_it1[n], m_it2[n]))

	CC_ALWAYS_INLINE constexpr
	auto operator[](const difference_type n) const
	nd_deduce_noexcept_and_return_type(m_func(m_it1[n], m_it2[n]))

	CC_ALWAYS_INLINE auto
	operator++(int) noexcept
	{ auto t = *this; ++m_it1; ++m_it2; return t; }

	CC_ALWAYS_INLINE auto
	operator--(int) noexcept
	{ auto t = *this; --m_it1; --m_it2; return t; }

	CC_ALWAYS_INLINE auto&
	operator++() noexcept
	{
		++m_it1;
		++m_it2;
		return *this;
	}

	CC_ALWAYS_INLINE auto&
	operator--() noexcept
	{
		--m_it1;
		--m_it2;
		return *this;
	}

	CC_ALWAYS_INLINE auto&
	operator+=(const difference_type n) noexcept
	{
		m_it1 += n;
		m_it2 += n;
		return *this;
	}

	CC_ALWAYS_INLINE auto&
	operator-=(const difference_type n) noexcept
	{
		m_it1 -= n;
		m_it2 -= n;
		return *this;
	}

	CC_ALWAYS_INLINE constexpr bool
	operator==(const zip_with_iterator& rhs)
	const noexcept { return m_it1 == rhs.m_it1; }

	CC_ALWAYS_INLINE constexpr bool
	operator!=(const zip_with_iterator& rhs)
	const noexcept { return m_it1 != rhs.m_it1; }

	CC_ALWAYS_INLINE constexpr bool
	operator>=(const zip_with_iterator& rhs)
	const noexcept { return m_it1 >= rhs.m_it1; }

	CC_ALWAYS_INLINE constexpr bool
	operator<=(const zip_with_iterator& rhs)
	const noexcept { return m_it1 <= rhs.m_it1; }

	CC_ALWAYS_INLINE constexpr bool
	operator>(const zip_with_iterator& rhs)
	const noexcept { return m_it1 > rhs.m_it1; }

	CC_ALWAYS_INLINE constexpr bool
	operator<(const zip_with_iterator& rhs)
	const noexcept { return m_it1 < rhs.m_it1; }

	CC_ALWAYS_INLINE constexpr auto
	operator-(const zip_with_iterator& rhs)
	const noexcept { return m_it1 - rhs.m_it1; }
};

template <class It1, class It2, class Func>
CC_ALWAYS_INLINE constexpr
auto operator+(
	const zip_with_iterator<It1, It2, Func>& x,
	const typename zip_with_iterator<It1, It2, Func>::difference_type n
) noexcept
{
	auto t = x;
	t += n;
	return t;
}

template <class It1, class It2, class Func>
CC_ALWAYS_INLINE constexpr
auto operator+(
	const typename zip_with_iterator<It1, It2, Func>::difference_type n,
	const zip_with_iterator<It1, It2, Func>& x
) noexcept
{
	auto t = x;
	t += n;
	return t;
}

template <class It1, class It2, class Func>
CC_ALWAYS_INLINE constexpr
auto operator-(
	const zip_with_iterator<It1, It2, Func>& x,
	const typename zip_with_iterator<It1, It2, Func>::difference_type n
) noexcept
{
	auto t = x;
	t -= n;
	return t;
}

template <class R1, class R2, class Func>
CC_ALWAYS_INLINE constexpr
auto zip_with(const R1& r1, const R2& r2, const Func& func)
{
	using iter_1   = typename R1::iterator;
	using iter_2   = typename R2::iterator;
	using iterator = zip_with_iterator<iter_1, iter_2, Func>;

	return boost::make_iterator_range(
		iterator{r1.begin(), r2.begin(), func},
		iterator{r1.end(), r2.end(), func}
	);
}

}

#endif
