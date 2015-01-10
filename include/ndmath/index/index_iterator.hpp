/*
** File Name: index_iterator.hpp
** Author:    Aditya Ramesh
** Date:      01/09/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z987B5FF7_DC26_4072_9190_450E247E29E4
#define Z987B5FF7_DC26_4072_9190_450E247E29E4

#include <cstddef>
#include <ccbase/platform.hpp>
#include <ndmath/utility.hpp>

namespace nd {

template <class Index>
class index_iterator final
{
	using index_type = std::remove_const_t<Index>;

	static constexpr auto dims         = index_type::dims();
	static constexpr auto is_const     = std::is_const<Index>::value;
	static constexpr auto is_constexpr = index_type::is_constexpr;

	using result       = typename index_type::value;
	using const_result = typename index_type::const_value;
public:
	using difference_type   = size_t;
	using value_type        = size_t;
	using pointer           = std::conditional_t<is_const, const size_t*, size_t*>;
	using reference         = std::conditional_t<is_const, const size_t&, size_t&>;
	using iterator_category = std::random_access_iterator_tag;
public:
	size_t m_pos;
	Index& m_index;
public:
	CC_ALWAYS_INLINE constexpr
	explicit index_iterator(Index& index, const size_t& pos = 0)
	noexcept : m_pos{pos}, m_index{index} {}

	CC_ALWAYS_INLINE constexpr
	index_iterator(const index_iterator&) = default;

	CC_ALWAYS_INLINE constexpr
	index_iterator(index_iterator&&) = default;

	CC_ALWAYS_INLINE auto
	operator=(const index_iterator& rhs) noexcept
	{ m_pos = rhs.m_pos; return *this; }

	CC_ALWAYS_INLINE auto
	operator=(index_iterator&& rhs) noexcept
	{ m_pos = rhs.m_pos; return *this; }

	/*
	** Accessors.
	*/

	template <nd_enable_if(!is_constexpr && !is_const)>
	CC_ALWAYS_INLINE result
	operator*() noexcept
	{ return m_index(m_pos); }

	template <nd_enable_if(!is_constexpr)>
	CC_ALWAYS_INLINE const_result
	operator*() const noexcept
	{ return m_index(m_pos); }

	template <nd_enable_if(is_constexpr)>
	CC_ALWAYS_INLINE CC_CONST constexpr
	const_result operator*() const noexcept
	{ return m_index(m_pos); }

	template <nd_enable_if(!is_constexpr && !is_const)>
	CC_ALWAYS_INLINE result
	operator[](const size_t& n)
	noexcept { return m_index(n); }

	template <nd_enable_if(!is_constexpr)>
	CC_ALWAYS_INLINE const_result
	operator[](const size_t& n)
	const noexcept { return m_index(n); }

	template <nd_enable_if(is_constexpr)>
	CC_ALWAYS_INLINE CC_CONST constexpr
	const_result operator[](const size_t& n)
	const noexcept { return m_index(n); }

	/*
	** Arithmetic and relational operators.
	*/

	CC_ALWAYS_INLINE auto
	operator-(const index_iterator& rhs)
	{ auto t = *this; t.m_pos -= rhs.m_pos; return t; }

	CC_ALWAYS_INLINE auto
	operator++(int) noexcept
	{ auto t = *this; ++m_pos; return t; }

	CC_ALWAYS_INLINE auto
	operator--(int) noexcept
	{ auto t = *this; --m_pos; return t; }

	CC_ALWAYS_INLINE auto&
	operator++() noexcept
	{ ++m_pos; return *this; }

	CC_ALWAYS_INLINE auto&
	operator--() noexcept
	{ --m_pos; return *this; }

	CC_ALWAYS_INLINE auto&
	operator+=(const size_t& n) noexcept
	{ m_pos += n; return *this; }

	CC_ALWAYS_INLINE auto&
	operator-=(const size_t& n) noexcept
	{ m_pos -= n; return *this; }

	#define nd_make_friend(symb)                   \
		template <class Index1, class Index2>  \
		friend bool operator symb (            \
			const index_iterator<Index1>&, \
			const index_iterator<Index2>&  \
		) noexcept;

	nd_make_friend(==)
	nd_make_friend(!=)
	nd_make_friend(>)
	nd_make_friend(>=)
	nd_make_friend(<)
	nd_make_friend(<=)

	#undef nd_make_friend
};

#define nd_define_relational_op(symbol)                   \
	template <class Index1, class Index2>             \
	CC_ALWAYS_INLINE bool                             \
	operator symbol (                                 \
		const index_iterator<Index1>& lhs,        \
		const index_iterator<Index2>& rhs         \
	) noexcept { return lhs.m_pos symbol rhs.m_pos; } \

nd_define_relational_op(==)
nd_define_relational_op(!=)
nd_define_relational_op(>)
nd_define_relational_op(>=)
nd_define_relational_op(<)
nd_define_relational_op(<=)

#undef nd_define_relational_op

template <class Index>
CC_ALWAYS_INLINE auto
operator+(const index_iterator<Index>& it, const size_t& n) 
noexcept { auto t = it; t += n; return t; }

template <class Index>
CC_ALWAYS_INLINE auto
operator-(const index_iterator<Index>& it, const size_t& n) 
noexcept { auto t = it; t -= n; return t; }

template <class Index>
CC_ALWAYS_INLINE auto
operator+(const size_t& n, const index_iterator<Index>& it) 
noexcept { auto t = it; t += n; return t; }

}

#endif
