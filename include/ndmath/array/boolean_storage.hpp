/*
** File Name: boolean_storage.hpp
** Author:    Aditya Ramesh
** Date:      08/21/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z4CD60484_3805_4A28_9172_516E237F8BBF
#define Z4CD60484_3805_4A28_9172_516E237F8BBF

#include <ndmath/common.hpp>

namespace nd {

template <class T>
struct boolean_storage final
{
	static_assert(
		std::is_unsigned<T>::value,
		"Storage type must be unsigned."
	);

	using storage_type = T;
private:
	T m_data;
public:
	CC_ALWAYS_INLINE constexpr
	boolean_storage() noexcept {}

	CC_ALWAYS_INLINE constexpr
	explicit boolean_storage(const T& rhs) noexcept
	: m_data{rhs} {}

	CC_ALWAYS_INLINE constexpr
	auto& value() noexcept { return m_data; }

	CC_ALWAYS_INLINE constexpr
	const auto& value() const noexcept { return m_data; }
};

template <class Storage, class Integer>
class boolean_proxy;

namespace detail {

struct fast_not
{
	template <class T>
	CC_ALWAYS_INLINE constexpr
	auto operator()(const boolean_storage<T>& x)
	const noexcept { return boolean_storage<T>{~x.value()}; }

	/*
	** We also define the operation for boolean proxies, because this
	** function object still needs to compile for the reference type of
	** `dense_storage<bool>` in order for the elementwise operations to
	** work. The overload below should never actually be invoked.
	*/
	template <class Storage, class Integer>
	CC_ALWAYS_INLINE constexpr
	auto operator()(const boolean_proxy<Storage, Integer>& x)
	const noexcept { return !x; }

	CC_ALWAYS_INLINE constexpr
	auto operator()(const bool& x)
	const noexcept { return !x; }
};

struct fast_and
{
	template <class T>
	CC_ALWAYS_INLINE constexpr
	auto operator()(const boolean_storage<T>& x, const boolean_storage<T>& y)
	const noexcept { return boolean_storage<T>{x.value() & y.value()}; }

	template <class S1, class I1, class S2, class I2>
	CC_ALWAYS_INLINE constexpr
	auto operator()(
		const boolean_proxy<S1, I1>& x,
		const boolean_proxy<S2, I2>& y
	) const noexcept { return x && y; }

	CC_ALWAYS_INLINE constexpr
	auto operator()(const bool& x, const bool& y)
	const noexcept { return x && y; }
};

struct fast_or
{
	template <class T>
	CC_ALWAYS_INLINE constexpr
	auto operator()(const boolean_storage<T>& x, const boolean_storage<T>& y)
	const noexcept { return boolean_storage<T>{x.value() | y.value()}; }

	template <class S1, class I1, class S2, class I2>
	CC_ALWAYS_INLINE constexpr
	auto operator()(
		const boolean_proxy<S1, I1>& x,
		const boolean_proxy<S2, I2>& y
	) const noexcept { return x || y; }

	CC_ALWAYS_INLINE constexpr
	auto operator()(const bool& x, const bool& y)
	const noexcept { return x || y; }
};

}

}

#endif
