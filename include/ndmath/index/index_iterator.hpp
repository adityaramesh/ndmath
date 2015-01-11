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

/*
** TODO: At some point, we may have to parameterize out the integer type of
** `Dims`. If the restriction that `m_pos` is `size_t` (instead of `unsigned`)
** poses performance problems (which seems unlikely), this would need to be
** done.
*/
template <class Index>
class index_iterator final
{
	using index_value = std::remove_const_t<Index>;

	static constexpr auto dims         = index_value::dims();
	static constexpr auto is_const     = std::is_const<Index>::value;
	static constexpr auto is_constexpr = index_value::is_constexpr;

	using result = typename index_value::result;
	/*
	** We cannot assign this to `index_value::const_result`: if
	** `const_result` is of type `const T&`, then `std::equal_to` will fail
	** due to an ambiguous overload. Any standard (or Boost) algorithms that
	** use `std::equal_to` will fail to comile. Perhaps there's a
	** cosmetically better fix, but I don't know what that would be.
	*/
	using const_result = typename index_value::index_type;
public:
	using difference_type   = size_t;
	using value_type        = std::conditional_t<is_const, const_result, result>;
	using reference         = value_type;
	using iterator_category = std::random_access_iterator_tag;

	using pointer = std::conditional_t<
		std::is_reference<value_type>::value,
		std::add_pointer_t<std::remove_reference_t<value_type>>,
		value_type
	>;
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
