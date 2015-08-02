/*
** File Name: boolean_proxy.hpp
** Author:    Aditya Ramesh
** Date:      03/19/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z9D344B31_49FD_41B8_9355_22E3CACB845E
#define Z9D344B31_49FD_41B8_9355_22E3CACB845E

namespace nd {

template <class Storage, class Integer>
class boolean_proxy final
{
	static_assert(
		std::is_unsigned<Storage>::value,
		"Boolean must be packed in unsigned integer storage type."
	);

	static_assert(
		std::is_integral<Integer>::value,
		"The offset must be an integral type."
	);

	Storage& m_ref;

	/*
	** The offset stores the position of the bit in m_ref mod (8 *
	** sizeof(Storage)). Previously, I attempted to use the rotate left
	** instruction (rol), which allows us to take the modulus of m_off by 8
	** * sizeof(Storage) for free. However, there is a bug in clang which
	** causes incorrect code to be generated if this instruction is used in
	** inline assembly. I wasn't able to produce a quick MWE, so it has not
	** yet been reported to my knowledge. Hopefully, the compiler recognizes
	** that the AND is redundant and decides to use rol anyway.
	*/
	Integer m_off;
public:
	CC_ALWAYS_INLINE constexpr
	explicit boolean_proxy(Storage& src, Integer off)
	noexcept : m_ref{src}, m_off{off} {}

	CC_ALWAYS_INLINE
	auto& operator=(const boolean_proxy& rhs) noexcept
	{
		*this = static_cast<bool>(rhs);
		return *this;
	}

	template <nd_enable_if((!std::is_const<Storage>::value))>
	CC_ALWAYS_INLINE
	auto& operator=(const boolean_proxy<const Storage, Integer>& rhs)
	noexcept
	{
		*this = static_cast<bool>(rhs);
		return *this;
	}

	template <nd_enable_if((!std::is_const<Storage>::value))>
	CC_ALWAYS_INLINE
	auto& operator=(const bool val) noexcept
	{
		m_ref ^= (m_ref ^ -Storage{val}) & (1 << (m_off % sizeof(Storage)));
		return *this;
	}

	CC_ALWAYS_INLINE constexpr
	operator bool() const noexcept
	{ return m_ref & (1 << (m_off % sizeof(Storage))); }
};

}

#endif
