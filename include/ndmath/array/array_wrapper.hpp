/*
** File Name: array_wrapper.hpp
** Author:    Aditya Ramesh
** Date:      01/12/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z4D613F07_81EE_463E_994F_7F5528B07D3E
#define Z4D613F07_81EE_463E_994F_7F5528B07D3E

#include <ndmath/range.hpp>

namespace nd {

struct new_dim_t {};
struct greedy_all_t {};

struct all_t
{
	/*
	** Equivalent of numpy's `newaxis`. `_()` is indicative of the action of
	** adding a new dimension.
	*/
	CC_ALWAYS_INLINE constexpr
	auto operator()() const noexcept
	{ return new_dim_t{}; }

	/*
	** Equivalent of numpy's `...`. The star in `*_` suggests that we
	** greedily select as many dimensions as possible. (Ok, it's a stretch,
	** but I couldn't think of a better concise way to express this action.)
	*/
	CC_ALWAYS_INLINE constexpr
	auto operator*() const noexcept
	{ return greedy_all_t{}; }
};

namespace tokens {

static constexpr auto _ = all_t{};

}

template <class T>
class array_wrapper final
{
public:
	/*
	** TODO: add support for iteration.
	**
	** using iterator = ...
	** using const_iterator = ...
	*/

	static constexpr auto uses_storage =
	T::uses_storage;

	static constexpr auto is_literal =
	std::is_literal_type<T>::value;
private:
	T m_wrapped;
public:
	template <class... Args, nd_enable_if(!is_literal)>
	CC_ALWAYS_INLINE
	explicit array_wrapper(Args&&... args)
	noexcept : m_wrapped(std::forward<Args>(args)...) {}

	template <class... Args, nd_enable_if(is_literal)>
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper(Args&&... args)
	noexcept : m_wrapped(std::forward<Args>(args)...) {}

	/*
	** TODO assignment
	*/

	/*
	** General accessors.
	*/

	CC_ALWAYS_INLINE constexpr
	auto wrapped() const noexcept
	{ return m_wrapped; }

	CC_ALWAYS_INLINE constexpr
	static auto dims() noexcept
	{ return T::dims; }

	CC_ALWAYS_INLINE constexpr
	auto range() const noexcept
	{ return m_wrapped.range(); }

	CC_ALWAYS_INLINE constexpr
	auto extents() const noexcept
	{ return range().finish(); }

	CC_ALWAYS_INLINE constexpr
	auto strides() const noexcept
	{ return range().strides(); }

	CC_ALWAYS_INLINE constexpr
	auto count() const noexcept
	{ return range().count(); }

	template <nd_enable_if(uses_storage)>
	CC_ALWAYS_INLINE constexpr
	auto storage() noexcept
	nd_deduce_return_type(m_wrapped.storage())

	template <nd_enable_if(uses_storage)>
	CC_ALWAYS_INLINE constexpr
	auto storage() const noexcept
	nd_deduce_const_return_type(m_wrapped.storage())

	/*
	** Element accessors.
	*/

	template <class... Args>
	CC_ALWAYS_INLINE constexpr
	auto operator()(const Args&... args) noexcept
	// ->
	//{ ... }

	template <class... Args>
	CC_ALWAYS_INLINE constexpr
	auto operator()(const Args&... args) const noexcept
	// ->
	//{ ... }

	/*
	** TODO: interface for unchecked array access via at().
	*/

	/*
	** TODO: first and last for 1D arrays. Generate a sequence of c<0>s for
	** first(), and sequence of `end` for last().
	*/

	/*
	** TODO: reshape() returns a reshape view, so that the underlying
	** elements can be accessed using indices from a different extent range.
	** Two cases:
	** - If the wrapped type has an underlying store, then the reshape view
	** can fetch the element directly from the storage by computing the
	** offset.
	** - Otherwise, we must go from reshape view coordinate -> 1D offset ->
	** coordinate of original array.
	**
	** Special case of reshape: flatten().
	*/

	/*
	** TODO: elementwise views via operator(). Comparison of elementwise
	** views produces a boolean array expression.
	*/

	/*
	** TODO: reduction views via operator()(func, args...). Make an instance
	** of `all_t` called `_` in the tokens namespace. Use enable_if on
	** decltype(func(scalar)) to disambiguate from element access.
	*/

	/*
	** TODO: iteration.
	*/
};

/*
** TODO: for_each (iterates over elements instead of over range). Use
** for_each(arr.range) for the latter functionality.
*/

/*
** TODO: relational operators. When used without the comma, they return whether
** *all* elements of a satisfy the given predicate over the corresponding
** elements of b.
*/

// TODO: use numpy's format for printing arrays.

}

#endif
