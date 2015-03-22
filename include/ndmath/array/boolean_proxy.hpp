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
	** sizeof(Storage)). The modulus doesn't matter, because the rotate left
	** instruction effectively ignores it. Using the rotate instruction
	** allows us to avoid using a redundant AND operation.
	*/
	Integer m_off;
public:
	CC_ALWAYS_INLINE constexpr
	explicit boolean_proxy(Storage& src, Integer off)
	noexcept : m_ref{src}, m_off{off} {}

	template <nd_enable_if((
		!std::is_const<Storage>::value
	))>
	CC_ALWAYS_INLINE
	auto& operator=(const bool val) noexcept
	{
		m_ref ^= (m_ref ^ -Storage{val}) & cc::rotl(Storage{1}, m_off);
		return *this;
	}

	CC_ALWAYS_INLINE constexpr
	operator bool() const noexcept
	{ return m_ref & cc::rotl(Storage{1}, m_off); }
};

}

#endif
