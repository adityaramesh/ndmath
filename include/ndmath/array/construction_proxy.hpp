/*
** File Name: construction_proxy.hpp
** Author:    Aditya Ramesh
** Date:      03/28/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z3F7B13D3_51D4_4246_8E4B_A1E18E721B85
#define Z3F7B13D3_51D4_4246_8E4B_A1E18E721B85

namespace nd {

template <class T, class Alloc>
class construction_proxy;

template <class T>
class construction_proxy<T, void> final
{
	static_assert(
		!std::is_const<T>::value,
		"Reference type cannot be const, because it is constructed "
		"in-place when the proxy class is assigned."
	);

	T& m_ref;
public:
	CC_ALWAYS_INLINE constexpr
	explicit construction_proxy(T& src)
	noexcept : m_ref{src} {}

	template <class U>
	CC_ALWAYS_INLINE
	auto& operator=(const U& rhs)
	noexcept(std::is_nothrow_constructible<T, const U&>::value)
	{
		::new (&m_ref) T{rhs};
		return *this;
	}

	template <class U>
	CC_ALWAYS_INLINE
	auto& operator=(U&& rhs)
	noexcept(std::is_nothrow_constructible<T, U&&>::value)
	{
		::new (&m_ref) T{std::move(rhs)};
		return *this;
	}
};

template <class T, class Alloc>
class construction_proxy final
{
	static_assert(
		!std::is_const<T>::value,
		"Reference type cannot be const, because it is constructed "
		"in-place when the proxy class is assigned."
	);

	T& m_ref;
	Alloc& m_alloc;
public:
	CC_ALWAYS_INLINE constexpr
	explicit construction_proxy(T& src, Alloc& alloc)
	noexcept : m_ref{src}, m_alloc{alloc} {}

	template <class U>
	CC_ALWAYS_INLINE
	auto& operator=(const U& rhs)
	noexcept(std::is_nothrow_constructible<T, const U&>::value)
	{
		m_alloc.construct(&m_ref, rhs);
		return *this;
	}

	template <class U>
	CC_ALWAYS_INLINE
	auto& operator=(U&& rhs)
	noexcept(std::is_nothrow_constructible<T, U&&>::value)
	{
		m_alloc.construct(&m_ref, std::move(rhs));
		return *this;
	}
};

}

#endif
