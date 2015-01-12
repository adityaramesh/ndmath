/*
** File Name: range_iterator.hpp
** Author:    Aditya Ramesh
** Date:      01/10/2015
** Contact:   _@adityaramesh.com
**
** Don't use `range_iterator` if you want the fastest code possible (read note
** 2). Use `range.operator()` or `for_each` instead.
**
** Note 1: comparing two `range_iterator`s does *not* really check whether they
** are equal. The comparison operation only checks whether the first index of
** the LHS is equal to the first extent of the RHS. This decision is motivated
** by the range-for's requirement that `begin` and `end` return the same type.
** Since the comparison operation is performed at each iteration, it must be
** very efficient if we are to compete with the performance of real for loops.
** Instead of doing a real comparison, I use this cheat to improve performance.
** For this reason, `range_iterator`s are *not* safe to use in general
** algorithms.
**
** Note 2: Using the `range_iterator` tends to generate slightly worse code than
** nested for loops. Clang-3.5 creates a label for each for loop in a nest and
** uses a series of inc/dec, cmp, and jne instructions to perform the
** iterations. When `range_iterator` is used, clang-3.5 is unable to recognize
** that the if statements in `increment_helper` are structured in the same way.
** Instead of directly using jne, clang-3.5 uses cmp, followed by cmov/movzb to
** eliminate the branch in the if statement. Unfortunately, this results in
** slightly slower code (on average, about 2--3% slower when comparing
** equivalent loops that have empty `asm` statements in their bodies to prevent
** optimizations). If you want the best code possible or wish to perform
** specific loop optimizations, then do not use `range_iterator`.
*/

#ifndef ZFD9FC7F1_55E8_4F94_B4E3_AF47F504AABB
#define ZFD9FC7F1_55E8_4F94_B4E3_AF47F504AABB

#include <boost/range/algorithm.hpp>
#include <ccbase/platform.hpp>
#include <ccbase/utility/sequence_operations.hpp>
#include <ndmath/index.hpp>

namespace nd {

template <class Base, class Extents, class Stride>
class range;

namespace detail {

template <size_t CurDim>
struct increment_helper
{
	using next = increment_helper<CurDim - 1>;

	template <class Index, class Range>
	CC_ALWAYS_INLINE static void
	apply(Index& i, const Range& r) noexcept
	{
		i(CurDim) += r.stride(CurDim);
		if (i(CurDim) == r.extent(CurDim)) {
			i(CurDim) = r.base(CurDim);
			next::apply(i, r);
		}
	}
};

template <>
struct increment_helper<0>
{
	template <class Index, class Range>
	CC_ALWAYS_INLINE static void
	apply(Index& i, const Range& r)
	noexcept { i(0) += r.stride(0); }
};

template <size_t CurDim>
struct decrement_helper
{
	using next = decrement_helper<CurDim - 1>;

	template <class Index, class Range>
	CC_ALWAYS_INLINE static void
	apply(Index& i, const Range& r)
	noexcept
	{
		if (i(CurDim) == r.base(CurDim)) {
			i(CurDim) = r.extent(CurDim);
			return next::apply(i, r);
		}
		i(CurDim) -= r.stride(CurDim);
	}
};

template <>
struct decrement_helper<0>
{
	template <class Index, class Range>
	CC_ALWAYS_INLINE static void
	apply(Index& i, const Range& r)
	noexcept { i(0) -= r.stride(0); }
};

}

template <class Range>
class range_iterator
{
	static constexpr auto dims = Range::dims();
	using index_type           = typename Range::index_type;
	using position_type        = index<index_type, dims>;
	using increment_helper     = detail::increment_helper<dims - 1>;
	using decrement_helper     = detail::decrement_helper<dims - 1>;

	position_type m_pos;
	const Range& m_range;
public:
	using difference_type   = void;
	using value_type        = const position_type&;
	using pointer           = void;
	using reference         = void;
	using iterator_category = std::bidirectional_iterator_tag;

	CC_ALWAYS_INLINE constexpr
	explicit range_iterator(const Range& r)
	noexcept : m_pos{r.bases()}, m_range{r} {}

	CC_ALWAYS_INLINE
	range_iterator(const range_iterator&) = default;

	CC_ALWAYS_INLINE
	range_iterator(range_iterator&&) = default;

	CC_ALWAYS_INLINE auto
	operator=(const range_iterator& rhs) noexcept
	{ m_pos = rhs.m_pos; return *this; }

	CC_ALWAYS_INLINE auto
	operator=(range_iterator&& rhs) noexcept
	{ m_pos = rhs.m_pos; return *this; }

	/*
	** Accessors.
	*/

	CC_ALWAYS_INLINE const auto&
	operator*() const noexcept
	{ return m_pos; }

	CC_ALWAYS_INLINE const auto*
	operator->() const noexcept
	{ return &m_pos; }

	CC_ALWAYS_INLINE auto
	operator++(int) noexcept
	{ auto t = *this; ++(*this); return t; }

	CC_ALWAYS_INLINE auto
	operator--(int) noexcept
	{ auto t = *this; --(*this); return t; }

	CC_ALWAYS_INLINE auto&
	operator++() noexcept
	{
		increment_helper::apply(m_pos, m_range);
		return *this;
	}

	CC_ALWAYS_INLINE auto&
	operator--() noexcept
	{
		decrement_helper::apply(m_pos, m_range);
		return *this;
	}

	template <class Range_>
	CC_ALWAYS_INLINE bool
	operator==(const range_iterator<Range_>& rhs)
	noexcept
	{
		return m_pos.first() == rhs.m_range.extents().first();
		// return boost::equal(m_pos, rhs.m_pos);
	}

	template <class Range_>
	CC_ALWAYS_INLINE bool
	operator!=(const range_iterator<Range_>& rhs)
	noexcept { return !(*this == rhs); }
};

/*
** XXX: The approach below does not work, because the range for loop requires
** that `begin` and `end` return the same type of iterator.
**
** When comparing two iterators for equality, we would normally have to check
** each index one by one. But when we know that the iterator being compared
** against is the end iterator, we only need to compare the first index against
** the first extent. So it makes sense to define a separate type just for the
** end iterator. Note that this type is not actually an iterator.
*/

/*
template <class Range>
class range_end_iterator
{
	const Range& m_range;
public:
	CC_ALWAYS_INLINE constexpr
	explicit range_end_iterator(const Range& range)
	noexcept : m_range{range} {}

	CC_ALWAYS_INLINE constexpr
	const auto& operator*() const noexcept
	{ return m_range.extents(); }

	CC_ALWAYS_INLINE constexpr
	const auto* operator->() const noexcept
	{ return &m_range.extents(); }
};

template <class Range>
CC_ALWAYS_INLINE bool
operator==(
	const range_iterator<Range>& lhs,
	const range_end_iterator<Range> rhs
) noexcept { return lhs->first() == rhs->first(); }

template <class Range>
CC_ALWAYS_INLINE bool
operator==(
	const range_end_iterator<Range>& lhs,
	const range_iterator<Range>& rhs
) noexcept { return lhs->first() == rhs->first(); }

template <class Range>
CC_ALWAYS_INLINE bool
operator!=(
	const range_iterator<Range>& lhs,
	const range_end_iterator<Range>& rhs
) noexcept { return !(lhs == rhs); }

template <class Range>
CC_ALWAYS_INLINE bool
operator!=(
	const range_end_iterator<Range>& lhs,
	const range_iterator<Range>& rhs
) noexcept { return !(lhs == rhs); }
*/

template <class Range>
CC_ALWAYS_INLINE constexpr
auto make_range_iterator(const Range& r)
noexcept { return range_iterator<Range>{r}; }

}

#endif
