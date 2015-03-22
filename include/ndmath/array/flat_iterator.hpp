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
namespace detail {

template <class T>
class flat_iterator_base
{
	using size_type = typename T::size_type;
protected:
	size_type m_pos{};
public:
	CC_ALWAYS_INLINE constexpr
	flat_iterator_base() noexcept {}

	CC_ALWAYS_INLINE constexpr
	flat_iterator_base(const size_type pos)
	noexcept : m_pos{pos} {}

	/*
	** Accessors.
	*/

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
	operator==(const flat_iterator_base& rhs)
	const noexcept { return m_pos == rhs.m_pos; }

	CC_ALWAYS_INLINE constexpr bool
	operator!=(const flat_iterator_base& rhs)
	const noexcept { return m_pos != rhs.m_pos; }

	CC_ALWAYS_INLINE constexpr bool
	operator>=(const flat_iterator_base& rhs)
	const noexcept { return m_pos >= rhs.m_pos; }

	CC_ALWAYS_INLINE constexpr bool
	operator<=(const flat_iterator_base& rhs)
	const noexcept { return m_pos <= rhs.m_pos; }

	CC_ALWAYS_INLINE constexpr bool
	operator>(const flat_iterator_base& rhs)
	const noexcept { return m_pos > rhs.m_pos; }

	CC_ALWAYS_INLINE constexpr bool
	operator<(const flat_iterator_base& rhs)
	const noexcept { return m_pos < rhs.m_pos; }

	CC_ALWAYS_INLINE constexpr auto
	operator-(const flat_iterator_base& rhs)
	const noexcept { return m_pos - rhs.m_pos; }
};

template <class T>
CC_ALWAYS_INLINE constexpr
auto operator+(
	const flat_iterator_base<T>& base,
	const typename T::size_type n
) noexcept
{
	auto t = base;
	t += n;
	return t;
}

template <class T>
CC_ALWAYS_INLINE constexpr
auto operator+(
	const typename T::size_type n,
	const flat_iterator_base<T>& base
) noexcept
{
	auto t = base;
	t += n;
	return t;
}

template <class T>
CC_ALWAYS_INLINE constexpr
auto operator-(
	const flat_iterator_base<T>& base,
	const typename T::size_type n
) noexcept
{
	auto t = base;
	t -= n;
	return t;
}

}

struct end_t {};
static constexpr auto end = end_t{};

template <class T, class AccessFunc>
class flat_iterator final : public detail::flat_iterator_base<T>
{
	static constexpr auto is_const =
	std::is_const<T>::value;

	using size_type = typename T::size_type;
	using base      = detail::flat_iterator_base<T>;
public:
	using difference_type   = size_type;
	using reference         = decltype(AccessFunc{}(size_type{}, std::declval<T&>()));
	using const_reference   = decltype(AccessFunc{}(size_type{}, std::declval<const T&>()));
	using value_type        = std::decay_t<reference>;
	using const_value_type  = std::decay_t<const_reference>;
	using pointer           = value_type*;
	using const_pointer     = const value_type*;
	using iterator_category = std::random_access_iterator_tag;

	using base::operator++;
	using base::operator--;
	using base::operator+=;
	using base::operator-=;
	using base::operator==;
	using base::operator!=;
	using base::operator>=;
	using base::operator<=;
	using base::operator>;
	using base::operator<;
private:
	using base::m_pos;
	T& m_ref;
public:
	CC_ALWAYS_INLINE constexpr
	explicit flat_iterator(T& src)
	noexcept : m_ref{src} {}

	CC_ALWAYS_INLINE constexpr
	explicit flat_iterator(end_t, T& src)
	noexcept : base{src.extents().size()}, m_ref{src} {}

	CC_ALWAYS_INLINE constexpr
	flat_iterator(const flat_iterator& rhs)
	noexcept : base{rhs.m_pos}, m_ref{rhs.m_ref} {}

	CC_ALWAYS_INLINE constexpr
	flat_iterator(flat_iterator&& rhs)
	noexcept : base{rhs.m_pos}, m_ref{rhs.m_ref} {}

	CC_ALWAYS_INLINE
	auto& operator=(const flat_iterator& rhs)
	noexcept { m_pos = rhs.m_pos; return *this; }

	CC_ALWAYS_INLINE
	auto& operator=(flat_iterator&& rhs)
	noexcept { m_pos = rhs.m_pos; return *this; }

	CC_ALWAYS_INLINE
	auto operator*()
	nd_deduce_noexcept_and_return_type(AccessFunc{}(m_pos, m_ref))

	CC_ALWAYS_INLINE constexpr
	auto operator*() const
	nd_deduce_noexcept_and_return_type(AccessFunc{}(m_pos, m_ref))

	template <nd_enable_if((std::is_same<reference, float&>::value))>
	CC_ALWAYS_INLINE
	pointer operator->()
	noexcept(noexcept(AccessFunc{}(m_pos, m_ref)))
	{ return &AccessFunc{}(m_pos, m_ref); }

	template <nd_enable_if((std::is_same<const_reference, const float&>::value))>
	CC_ALWAYS_INLINE constexpr
	const_pointer operator->() const
	noexcept(noexcept(AccessFunc{}(m_pos, m_ref)))
	{ return &AccessFunc{}(m_pos, m_ref); }

	CC_ALWAYS_INLINE 
	auto operator[](const size_type n)
	nd_deduce_noexcept_and_return_type(AccessFunc{}(m_pos + n, m_ref))

	CC_ALWAYS_INLINE constexpr
	auto operator[](const size_type n) const
	nd_deduce_noexcept_and_return_type(AccessFunc{}(m_pos + n, m_ref))
};

template <class AccessFunc, class T>
CC_ALWAYS_INLINE constexpr
auto make_flat_view(T& t) noexcept
{
	using iterator = flat_iterator<T, AccessFunc>;
	return boost::make_iterator_range(
		iterator{t},
		iterator{end, t}
	);
}

}

#endif
