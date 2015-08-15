/*
** File Name: flat_iterator.hpp
** Author:    Aditya Ramesh
** Date:      03/17/2015
** Contact:   _@adityaramesh.com
** 
** Allows one to iterate over an multidimensional array as if it were a 1D
** array.
**
** Technical note: this header may be used by unwrapped array types, so we
** cannot assume that the typedefs and functions provided by `array_wrapper` are
** present.
*/

#ifndef ZC7A16085_DE65_42E8_BBCD_804DD37DAF07
#define ZC7A16085_DE65_42E8_BBCD_804DD37DAF07

namespace nd {

template <class T, class AccessFunc>
class flat_iterator final
{
	using size_type = typename T::size_type;
public:
	using difference_type   = size_type;
	using reference         = decltype(std::declval<AccessFunc>()
					(size_type{}, std::declval<T&>()));
	using const_reference   = decltype(std::declval<AccessFunc>()
					(size_type{}, std::declval<const T&>()));
	using value_type        = std::decay_t<reference>;
	using pointer           = value_type*;
	using const_pointer     = const value_type*;
	using iterator_category = std::random_access_iterator_tag;
private:
	size_type m_pos{};
	T& m_ref;
	const AccessFunc& m_func;
public:
	CC_ALWAYS_INLINE constexpr
	explicit flat_iterator(T& src, const AccessFunc& func)
	noexcept : m_ref{src}, m_func{func} {}

	CC_ALWAYS_INLINE constexpr
	explicit flat_iterator(T& src, const size_type size, const AccessFunc& func)
	noexcept : m_pos{size}, m_ref{src}, m_func{func} {}

	CC_ALWAYS_INLINE constexpr
	flat_iterator(const flat_iterator& rhs)
	noexcept : m_pos{rhs.m_pos}, m_ref{rhs.m_ref}, m_func{rhs.m_func} {}

	CC_ALWAYS_INLINE constexpr
	flat_iterator(flat_iterator&& rhs)
	noexcept : m_pos{rhs.m_pos}, m_ref{rhs.m_ref}, m_func{rhs.m_func} {}

	CC_ALWAYS_INLINE
	auto& operator=(const flat_iterator& rhs)
	noexcept { m_pos = rhs.m_pos; return *this; }

	CC_ALWAYS_INLINE
	auto& operator=(flat_iterator&& rhs)
	noexcept { m_pos = rhs.m_pos; return *this; }

	CC_ALWAYS_INLINE
	auto operator*()
	nd_deduce_noexcept_and_return_type(m_func(m_pos, m_ref))

	CC_ALWAYS_INLINE constexpr
	auto operator*() const
	nd_deduce_noexcept_and_return_type(m_func(m_pos, m_ref))

	CC_ALWAYS_INLINE
	pointer operator->()
	noexcept(noexcept(m_func(m_pos, m_ref)))
	{ return &m_func(m_pos, m_ref); }

	CC_ALWAYS_INLINE constexpr
	const_pointer operator->() const
	noexcept(noexcept(m_func(m_pos, m_ref)))
	{ return &m_func(m_pos, m_ref); }

	CC_ALWAYS_INLINE 
	auto operator[](const size_type n)
	nd_deduce_noexcept_and_return_type(m_func(m_pos + n, m_ref))

	CC_ALWAYS_INLINE constexpr
	auto operator[](const size_type n) const
	nd_deduce_noexcept_and_return_type(m_func(m_pos + n, m_ref))

	CC_ALWAYS_INLINE auto
	operator++(int) noexcept
	{ auto t = *this; ++(*this); return t; }

	CC_ALWAYS_INLINE auto
	operator--(int) noexcept
	{ auto t = *this; --(*this); return t; }

	CC_ALWAYS_INLINE auto&
	operator++() noexcept
	{
		++m_pos;
		return *this;
	}

	CC_ALWAYS_INLINE auto&
	operator--() noexcept
	{
		--m_pos;
		return *this;
	}

	CC_ALWAYS_INLINE auto&
	operator+=(const size_type n) noexcept
	{
		m_pos += n;
		return *this;
	}

	CC_ALWAYS_INLINE auto&
	operator-=(const size_type n) noexcept
	{
		m_pos -= n;
		return *this;
	}

	CC_ALWAYS_INLINE constexpr bool
	operator==(const flat_iterator& rhs)
	const noexcept { return m_pos == rhs.m_pos; }

	CC_ALWAYS_INLINE constexpr bool
	operator!=(const flat_iterator& rhs)
	const noexcept { return m_pos != rhs.m_pos; }

	CC_ALWAYS_INLINE constexpr bool
	operator>=(const flat_iterator& rhs)
	const noexcept { return m_pos >= rhs.m_pos; }

	CC_ALWAYS_INLINE constexpr bool
	operator<=(const flat_iterator& rhs)
	const noexcept { return m_pos <= rhs.m_pos; }

	CC_ALWAYS_INLINE constexpr bool
	operator>(const flat_iterator& rhs)
	const noexcept { return m_pos > rhs.m_pos; }

	CC_ALWAYS_INLINE constexpr bool
	operator<(const flat_iterator& rhs)
	const noexcept { return m_pos < rhs.m_pos; }

	CC_ALWAYS_INLINE constexpr auto
	operator-(const flat_iterator& rhs)
	const noexcept { return m_pos - rhs.m_pos; }
};

template <class T, class AccessFunc>
CC_ALWAYS_INLINE constexpr
auto operator+(
	const flat_iterator<T, AccessFunc>& x,
	const typename T::size_type n
) noexcept
{
	auto t = x;
	t += n;
	return t;
}

template <class T, class AccessFunc>
CC_ALWAYS_INLINE constexpr
auto operator+(
	const typename T::size_type n,
	const flat_iterator<T, AccessFunc>& x
) noexcept
{
	auto t = x;
	t += n;
	return t;
}

template <class T, class AccessFunc>
CC_ALWAYS_INLINE constexpr
auto operator-(
	const flat_iterator<T, AccessFunc>& x,
	const typename T::size_type n
) noexcept
{
	auto t = x;
	t -= n;
	return t;
}

template <class T, class AccessFunc>
class construction_iterator final
{
	using size_type = typename T::size_type;
public:
	using difference_type   = size_type;
	using reference         = decltype(AccessFunc{}(size_type{}, std::declval<T&>()));
	using const_reference   = reference;
	using value_type        = std::decay_t<reference>;
	using pointer           = value_type*;
	using const_pointer     = pointer;
	using iterator_category = std::random_access_iterator_tag;
private:
	size_type m_pos{};
	T& m_ref;
	const AccessFunc& m_func;
public:
	CC_ALWAYS_INLINE constexpr
	explicit construction_iterator(T& src, const AccessFunc& func)
	noexcept : m_ref{src}, m_func{func} {}

	CC_ALWAYS_INLINE constexpr
	explicit construction_iterator(T& src, const size_type size, const AccessFunc& func)
	noexcept : m_pos{size}, m_ref{src}, m_func{func} {}

	CC_ALWAYS_INLINE constexpr
	construction_iterator(const construction_iterator& rhs)
	noexcept : m_pos{rhs.m_pos}, m_ref{rhs.m_ref}, m_func{rhs.m_func} {}

	CC_ALWAYS_INLINE constexpr
	construction_iterator(construction_iterator&& rhs)
	noexcept : m_pos{rhs.m_pos}, m_ref{rhs.m_ref}, m_func{rhs.m_func} {}

	CC_ALWAYS_INLINE
	auto& operator=(const construction_iterator& rhs)
	noexcept { m_pos = rhs.m_pos; return *this; }

	CC_ALWAYS_INLINE
	auto& operator=(construction_iterator&& rhs)
	noexcept { m_pos = rhs.m_pos; return *this; }

	CC_ALWAYS_INLINE
	auto operator*()
	nd_deduce_noexcept_and_return_type(m_func(m_pos, m_ref))

	CC_ALWAYS_INLINE
	pointer operator->()
	noexcept(noexcept(m_func(m_pos, m_ref)))
	{ return &m_func(m_pos, m_ref); }

	CC_ALWAYS_INLINE 
	auto operator[](const size_type n)
	nd_deduce_noexcept_and_return_type(m_func(m_pos + n, m_ref))

	CC_ALWAYS_INLINE auto
	operator++(int) noexcept
	{ auto t = *this; ++(*this); return t; }

	CC_ALWAYS_INLINE auto
	operator--(int) noexcept
	{ auto t = *this; --(*this); return t; }

	CC_ALWAYS_INLINE auto&
	operator++() noexcept
	{
		++m_pos;
		return *this;
	}

	CC_ALWAYS_INLINE auto&
	operator--() noexcept
	{
		--m_pos;
		return *this;
	}

	CC_ALWAYS_INLINE auto&
	operator+=(const size_type n) noexcept
	{
		m_pos += n;
		return *this;
	}

	CC_ALWAYS_INLINE auto&
	operator-=(const size_type n) noexcept
	{
		m_pos -= n;
		return *this;
	}

	CC_ALWAYS_INLINE constexpr bool
	operator==(const construction_iterator& rhs)
	const noexcept { return m_pos == rhs.m_pos; }

	CC_ALWAYS_INLINE constexpr bool
	operator!=(const construction_iterator& rhs)
	const noexcept { return m_pos != rhs.m_pos; }

	CC_ALWAYS_INLINE constexpr bool
	operator>=(const construction_iterator& rhs)
	const noexcept { return m_pos >= rhs.m_pos; }

	CC_ALWAYS_INLINE constexpr bool
	operator<=(const construction_iterator& rhs)
	const noexcept { return m_pos <= rhs.m_pos; }

	CC_ALWAYS_INLINE constexpr bool
	operator>(const construction_iterator& rhs)
	const noexcept { return m_pos > rhs.m_pos; }

	CC_ALWAYS_INLINE constexpr bool
	operator<(const construction_iterator& rhs)
	const noexcept { return m_pos < rhs.m_pos; }

	CC_ALWAYS_INLINE constexpr auto
	operator-(const construction_iterator& rhs)
	const noexcept { return m_pos - rhs.m_pos; }
};

template <class T, class AccessFunc>
CC_ALWAYS_INLINE constexpr
auto operator+(
	const construction_iterator<T, AccessFunc>& x,
	const typename T::size_type n
) noexcept
{
	auto t = x;
	t += n;
	return t;
}

template <class T, class AccessFunc>
CC_ALWAYS_INLINE constexpr
auto operator+(
	const typename T::size_type n,
	const construction_iterator<T, AccessFunc>& x
) noexcept
{
	auto t = x;
	t += n;
	return t;
}

template <class T, class AccessFunc>
CC_ALWAYS_INLINE constexpr
auto operator-(
	const construction_iterator<T, AccessFunc>& x,
	const typename T::size_type n
) noexcept
{
	auto t = x;
	t -= n;
	return t;
}

template <class T, class SizeType, class AccessFunc>
CC_ALWAYS_INLINE constexpr
auto make_flat_view(T& t, const SizeType size, const AccessFunc& func)
noexcept
{
	using iterator = flat_iterator<T, AccessFunc>;
	return boost::make_iterator_range(
		iterator{t, func},
		iterator{t, size, func}
	);
}

template <class T, class SizeType, class AccessFunc>
CC_ALWAYS_INLINE constexpr
auto make_construction_view(T& t, const SizeType size, const AccessFunc& func)
noexcept
{
	using iterator = construction_iterator<T, AccessFunc>;
	return boost::make_iterator_range(
		iterator{t, func},
		iterator{t, size, func}
	);
}

}

#endif
