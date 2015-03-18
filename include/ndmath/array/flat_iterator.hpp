/*
** File Name: flat_iterator.hpp
** Author:    Aditya Ramesh
** Date:      03/17/2015
** Contact:   _@adityaramesh.com
** 
** Allows one to iterate over an multidimensional array as if it were a 1D
** array.
*/

#ifndef ZC7A16085_DE65_42E8_BBCD_804DD37DAF07
#define ZC7A16085_DE65_42E8_BBCD_804DD37DAF07

namespace nd {
namespace detail {

template <class T>
class flat_iterator_base
{
	using array = std::decay_t<T>;
	using size_type = typename array::size_type;

	template <class U>
	friend auto operator-(
		const flat_iterator_base<U>& lhs,
		const flat_iterator_base<U>& rhs
	) noexcept;
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

	CC_ALWAYS_INLINE bool
	operator==(const flat_iterator_base& rhs) noexcept
	{ return m_pos == rhs.m_pos; }

	CC_ALWAYS_INLINE bool
	operator!=(const flat_iterator_base& rhs) noexcept
	{ return m_pos != rhs.m_pos; }

	CC_ALWAYS_INLINE bool
	operator>=(const flat_iterator_base& rhs) noexcept
	{ return m_pos >= rhs.m_pos; }

	CC_ALWAYS_INLINE bool
	operator<=(const flat_iterator_base& rhs) noexcept
	{ return m_pos <= rhs.m_pos; }

	CC_ALWAYS_INLINE bool
	operator>(const flat_iterator_base& rhs) noexcept
	{ return m_pos > rhs.m_pos; }

	CC_ALWAYS_INLINE bool
	operator<(const flat_iterator_base& rhs) noexcept
	{ return m_pos < rhs.m_pos; }
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

template <class T>
CC_ALWAYS_INLINE constexpr
auto operator-(
	const flat_iterator_base<T>& lhs,
	const flat_iterator_base<T>& rhs
) noexcept
{ return lhs.m_pos - rhs.m_pos; }

}

struct end_t {};
static constexpr auto end = end_t{};

namespace detail {

template <class T, class AccessFunc, bool IsConst>
class basic_flat_iterator final : flat_iterator_base<T>
{
	using array     = std::conditional_t<IsConst, T, const T>;
	using size_type = typename T::size_type;
	using base      = flat_iterator_base<T>;
public:
	using difference_type   = size_type;
	using value_type        = typename array::value_type;
	using const_pointer     = const value_type*;
	using const_reference   = typename array::const_reference;
	using iterator_category = std::random_access_iterator_tag;

	using pointer = std::conditional_t<
		IsConst, const_pointer, value_type*
	>;
	using reference = std::conditional_t<
		IsConst, const_reference, typename T::reference
	>;
private:
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
	using base::m_pos;

	array& m_ref;
public:
	CC_ALWAYS_INLINE constexpr
	explicit basic_flat_iterator(array& src)
	noexcept : m_ref{src} {}

	CC_ALWAYS_INLINE constexpr
	explicit basic_flat_iterator(end_t, array& src)
	noexcept : base{src.size()}, m_ref{src} {}

	CC_ALWAYS_INLINE
	auto& operator=(const basic_flat_iterator& rhs)
	noexcept { m_pos = rhs.m_pos; return *this; }

	CC_ALWAYS_INLINE
	auto& operator=(basic_flat_iterator&& rhs)
	noexcept { m_pos = rhs.m_pos; return *this; }

	CC_ALWAYS_INLINE
	auto operator*()
	nd_deduce_noexcept(AccessFunc{}(m_pos, m_ref))

	CC_ALWAYS_INLINE constexpr
	auto operator*() const
	nd_deduce_noexcept(AccessFunc{}(m_pos, m_ref))

	CC_ALWAYS_INLINE
	auto operator->()
	nd_deduce_noexcept(&AccessFunc{}(m_pos, m_ref))

	CC_ALWAYS_INLINE constexpr
	auto operator->() const
	nd_deduce_noexcept(&AccessFunc{}(m_pos, m_ref))

	CC_ALWAYS_INLINE 
	auto operator[](const size_type n)
	nd_deduce_noexcept(AccessFunc{}(m_pos + n, m_ref))

	CC_ALWAYS_INLINE constexpr
	auto operator[](const size_type n) const
	nd_deduce_noexcept(AccessFunc{}(m_pos + n, m_ref))
};

}

template <class T, class AccessFunc>
using flat_iterator = detail::basic_flat_iterator<T, AccessFunc, false>;

template <class T, class AccessFunc>
using const_flat_iterator = detail::basic_flat_iterator<T, AccessFunc, true>;

}

#endif
