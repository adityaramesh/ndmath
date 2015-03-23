/*
** File Name: array_wrapper.hpp
** Author:    Aditya Ramesh
** Date:      02/15/2015
** Contact:   _@adityaramesh.com
**
** The wrapped type must define the following types:
** - is_view
** - size_type
**
** The wrapped type must also define the following member functions:
** - extents()
** - storage_order()
** - at()            (const and non-const variants)
** - memory_size()   (optional)
** - flat_view()     (optional)
** - direct_view()   (optional)
** - resize()        (optional)
*/

#ifndef Z9FD66BF0_E92D_4CAE_A49B_8D7708927910
#define Z9FD66BF0_E92D_4CAE_A49B_8D7708927910

#include <ndmath/array/array_traits.hpp>
#include <ndmath/array/array_assignment.hpp>
#include <ndmath/array/flat_iterator.hpp>
#include <ndmath/array/element_from_offset.hpp>

namespace nd {
namespace detail {

struct copy_construct_t {};
struct move_construct_t {};
static constexpr auto copy_construct = copy_construct_t{};
static constexpr auto move_construct = move_construct_t{};

template <class T, bool IsCopyConstructible, bool IsMoveConstructible>
class array_wrapper_base;

template <class T>
class array_wrapper_base<T, false, false>
{
protected:
	T m_wrapped;
public:
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper_base()
	noexcept(std::is_nothrow_default_constructible<T>::value) {}

	template <class... Args>
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper_base(Args&&... args)
	noexcept(std::is_nothrow_constructible<T, Args...>::value)
	: m_wrapped{std::forward<Args>(args)...} {}

	CC_ALWAYS_INLINE constexpr
	array_wrapper_base(const T&)
	noexcept {}

	CC_ALWAYS_INLINE constexpr
	array_wrapper_base(T&&)
	noexcept {}

	template <class U>
	CC_ALWAYS_INLINE constexpr
	array_wrapper_base(copy_construct_t, const U& rhs)
	noexcept(std::is_nothrow_constructible<T, const U&>::value)
	: m_wrapped{rhs} {}

	template <class U>
	CC_ALWAYS_INLINE constexpr
	array_wrapper_base(move_construct_t, U&& rhs)
	noexcept(std::is_nothrow_constructible<T, U&&>::value)
	: m_wrapped{std::move(rhs)} {}
};

template <class T>
class array_wrapper_base<T, true, false>
{
protected:
	T m_wrapped;
public:
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper_base()
	noexcept(std::is_nothrow_default_constructible<T>::value) {}

	template <class... Args>
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper_base(Args&&... args)
	noexcept(std::is_nothrow_constructible<T, Args...>::value)
	: m_wrapped{std::forward<Args>(args)...} {}

	CC_ALWAYS_INLINE constexpr
	array_wrapper_base(const T& rhs)
	noexcept(std::is_nothrow_copy_constructible<T>::value)
	: m_wrapped{rhs} {}

	CC_ALWAYS_INLINE constexpr
	array_wrapper_base(T&&)
	noexcept {}

	template <class U>
	CC_ALWAYS_INLINE constexpr
	array_wrapper_base(copy_construct_t, const U& rhs)
	noexcept(std::is_nothrow_constructible<T, const U&>::value)
	: m_wrapped{rhs} {}

	template <class U>
	CC_ALWAYS_INLINE constexpr
	array_wrapper_base(move_construct_t, U&& rhs)
	noexcept(std::is_nothrow_constructible<T, U&&>::value)
	: m_wrapped{std::move(rhs)} {}
};

template <class T>
class array_wrapper_base<T, false, true>
{
protected:
	T m_wrapped;
public:
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper_base()
	noexcept(std::is_nothrow_default_constructible<T>::value) {}

	template <class... Args>
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper_base(Args&&... args)
	noexcept(std::is_nothrow_constructible<T, Args...>::value)
	: m_wrapped{std::forward<Args>(args)...} {}

	CC_ALWAYS_INLINE constexpr
	array_wrapper_base(const T&)
	noexcept {}

	CC_ALWAYS_INLINE constexpr
	array_wrapper_base(T&& rhs)
	noexcept(std::is_nothrow_move_constructible<T>::value)
	: m_wrapped{std::move(rhs)} {}

	template <class U>
	CC_ALWAYS_INLINE constexpr
	array_wrapper_base(copy_construct_t, const U& rhs)
	noexcept(std::is_nothrow_constructible<T, const U&>::value)
	: m_wrapped{rhs} {}

	template <class U>
	CC_ALWAYS_INLINE constexpr
	array_wrapper_base(move_construct_t, U&& rhs)
	noexcept(std::is_nothrow_constructible<T, U&&>::value)
	: m_wrapped{std::move(rhs)} {}
};

template <class T>
class array_wrapper_base<T, true, true>
{
protected:
	T m_wrapped;
public:
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper_base()
	noexcept(std::is_nothrow_default_constructible<T>::value) {}

	template <class... Args>
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper_base(Args&&... args)
	noexcept(std::is_nothrow_constructible<T, Args...>::value)
	: m_wrapped{std::forward<Args>(args)...} {}

	CC_ALWAYS_INLINE constexpr
	array_wrapper_base(const T& rhs)
	noexcept(std::is_nothrow_copy_constructible<T>::value)
	: m_wrapped{rhs} {}

	CC_ALWAYS_INLINE constexpr
	array_wrapper_base(T&& rhs)
	noexcept(std::is_nothrow_move_constructible<T>::value)
	: m_wrapped{std::move(rhs)} {}

	template <class U>
	CC_ALWAYS_INLINE constexpr
	array_wrapper_base(copy_construct_t, const U& rhs)
	noexcept(std::is_nothrow_constructible<T, const U&>::value)
	: m_wrapped{rhs} {}

	template <class U>
	CC_ALWAYS_INLINE constexpr
	array_wrapper_base(move_construct_t, U&& rhs)
	noexcept(std::is_nothrow_constructible<T, U&&>::value)
	: m_wrapped{std::move(rhs)} {}
};

}

template <class T>
class array_wrapper final :
detail::array_wrapper_base<
	T, std::is_copy_constructible<T>::value,
	std::is_move_constructible<T>::value
>
{
	static constexpr auto is_copy_constructible =
	std::is_copy_constructible<T>::value;

	static constexpr auto is_move_constructible =
	std::is_move_constructible<T>::value;

	using self = array_wrapper<T>;
	using traits = array_traits<T>;

	using base = detail::array_wrapper_base<T, is_copy_constructible,
		is_move_constructible>;
	using base::m_wrapped;
public:
	using size_type        = typename traits::size_type;
	using reference        = typename traits::reference;
	using const_reference  = typename traits::const_reference;
	using value_type       = typename traits::value_type;
	using const_value_type = typename traits::value_type;

	using direct_iterator       = typename traits::direct_iterator;
	using const_direct_iterator = typename traits::const_direct_iterator;
	using underlying_type       = typename traits::underlying_type;

	static constexpr auto is_view                 = traits::is_view;
	static constexpr auto is_safe_resizable       = traits::is_safe_resizable;
	static constexpr auto is_unsafe_resizable     = traits::is_unsafe_resizable;
	static constexpr auto is_noexcept_accessible  = traits::is_noexcept_accessible;
	static constexpr auto provides_direct_view    = traits::provides_direct_view;
	static constexpr auto provides_fast_flat_view = traits::provides_fast_flat_view;
	static constexpr auto provides_memory_size    = traits::provides_memory_size;

	using flat_iterator = std::conditional_t<
		std::is_same<typename traits::flat_iterator, void>::value,
		nd::flat_iterator<self, element_from_offset>,
		typename traits::flat_iterator
	>;
	using const_flat_iterator = std::conditional_t<
		std::is_same<typename traits::const_flat_iterator, void>::value,
		nd::flat_iterator<const self, element_from_offset>,
		typename traits::const_flat_iterator
	>;

	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper()
	noexcept(std::is_nothrow_default_constructible<base>::value) {}

	template <class... Args>
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper(Args&&... args)
	noexcept(std::is_nothrow_constructible<base, Args...>::value)
	: base(std::forward<Args>(args)...) {}

	/*
	** Copy and move constructors.
	*/

	/*
	** We would like our copy constructor to call T's copy constructor if it
	** is provided, or restort to default-initialization and copy assignment
	** otherwise. In order to do this, we need to delegate the
	** responsibility of conditionally calling T's copy constructor to the
	** base class.
	*/
	CC_ALWAYS_INLINE constexpr
	array_wrapper(const array_wrapper& rhs)
	noexcept(
		(is_copy_constructible && std::is_nothrow_constructible<
		 	base, const T&>::value) ||
		(!is_copy_constructible && noexcept(*this = rhs))
	) : base{rhs.wrapped()}
	{
		/*
		** If T doesn't provide a copy constructor, then we resort to
		** using default-initialization (which the base class has now
		** done for us), and copy-assignment (which we do below).
		*/
		if (!is_copy_constructible)
			*this = rhs;
	}

	/*
	** Refer to comments for the copy constructor above.
	*/
	CC_ALWAYS_INLINE constexpr
	array_wrapper(array_wrapper&& rhs)
	noexcept(
		(is_move_constructible && std::is_nothrow_constructible<
		 	base, T&&>::value) ||
		(!is_move_constructible && noexcept(*this = std::move(rhs)))
	) : base{std::move(rhs.wrapped())}
	{
		if (!is_move_constructible)
			*this = std::move(rhs);
	}

	/*
	** If the wrapped type has a copy constructor for U, then use it.
	*/
	template <class U, nd_enable_if((
		std::is_constructible<T, const U&>::value
	))>
	CC_ALWAYS_INLINE constexpr
	array_wrapper(const array_wrapper<U>& rhs)
	noexcept(std::is_nothrow_constructible<T, const U&>::value)
	: base{detail::copy_construct, rhs.wrapped()} {}

	/*
	** If the wrapped type does not have a copy constructor for U, then we
	** use copy assignment.
	*/
	template <class U, nd_enable_if((
		!std::is_constructible<T, const U&>::value
	))>
	CC_ALWAYS_INLINE constexpr
	array_wrapper(const array_wrapper<U>& rhs)
	noexcept(std::is_nothrow_default_constructible<T>::value &&
		noexcept(*this = rhs))
	{ *this = rhs; }

	/*
	** If the wrapped type has a move constructor for U, then use it.
	*/
	template <class U, nd_enable_if((
		std::is_constructible<T, U&&>::value
	))>
	CC_ALWAYS_INLINE constexpr
	array_wrapper(array_wrapper<U>&& rhs)
	noexcept(std::is_nothrow_constructible<T, U&&>::value)
	: base{detail::move_construct, std::move(rhs.wrapped())} {}

	/*
	** If the wrapped type does not have a move constructor for U, then we
	** use move assignment.
	*/
	template <class U, nd_enable_if((
		!std::is_constructible<T, U&&>::value
	))>
	CC_ALWAYS_INLINE constexpr
	array_wrapper(array_wrapper<U>&& rhs)
	noexcept(std::is_nothrow_default_constructible<T>::value &&
		noexcept(*this = std::move(rhs)))
	{ *this = std::move(rhs); }

	/*
	** Assignment operators.
	*/

	CC_ALWAYS_INLINE auto&
	operator=(const array_wrapper& rhs)
	noexcept(noexcept(detail::assignment_helper::copy_assign(*this, rhs)))
	{
		detail::assignment_helper::copy_assign(*this, rhs);
		return *this;
	}

	/*
	** XXX: Using the enable_if here results in bad codegen in clang-3.5.
	** An invalid assignment that should have never compiled due to the
	** enable_if ends up working anyway, and running the program results in
	** a segfault. I don't have the time to make an MWE.
	*/
	template <class U/*, nd_enable_if((
		std::is_assignable<
			reference,
			typename array_wrapper<U>::reference
		>::value
	))*/>
	CC_ALWAYS_INLINE auto&
	operator=(const array_wrapper<U>& rhs)
	noexcept(noexcept(detail::assignment_helper::copy_assign(*this, rhs)))
	{
		detail::assignment_helper::copy_assign(*this, rhs);
		return *this;
	}

	CC_ALWAYS_INLINE auto&
	operator=(array_wrapper&& rhs)
	noexcept(noexcept(
		detail::assignment_helper::move_assign(*this, std::move(rhs))
	))
	{
		detail::assignment_helper::move_assign(*this, std::move(rhs));
		return *this;
	}

	template <class U, nd_enable_if((
		std::is_assignable<
			reference,
			std::remove_reference_t<
				typename array_wrapper<U>::reference
			>&&
		>::value
	))>
	CC_ALWAYS_INLINE auto&
	operator=(array_wrapper<U>&& rhs)
	noexcept(noexcept(
		detail::assignment_helper::move_assign(*this, std::move(rhs))
	))
	{
		detail::assignment_helper::move_assign(*this, std::move(rhs));
		return *this;
	}

	/*
	** General accessors.
	*/

	CC_ALWAYS_INLINE constexpr
	static auto dims() noexcept
	{ return traits::dims; }

	CC_ALWAYS_INLINE
	auto& wrapped() noexcept
	{ return m_wrapped; }

	CC_ALWAYS_INLINE
	auto& wrapped() const noexcept
	{ return m_wrapped; }

	CC_ALWAYS_INLINE constexpr
	auto extents() const noexcept
	{ return m_wrapped.extents(); }

	CC_ALWAYS_INLINE constexpr
	auto storage_order() const noexcept
	{ return m_wrapped.storage_order(); }

	CC_ALWAYS_INLINE constexpr
	auto size() const noexcept
	{ return extents().size(); }

	template <nd_enable_if(provides_memory_size)>
	CC_ALWAYS_INLINE constexpr
	auto memory_size() const noexcept
	{ return m_wrapped.memory_size(); }

	/*
	** Views.
	*/

	template <nd_enable_if(provides_fast_flat_view)>
	CC_ALWAYS_INLINE
	auto flat_view() noexcept
	{ return m_wrapped.flat_view(); }

	template <nd_enable_if(provides_fast_flat_view)>
	CC_ALWAYS_INLINE
	auto flat_view() const noexcept
	{ return m_wrapped.flat_view(); }

	template <nd_enable_if(!provides_fast_flat_view)>
	CC_ALWAYS_INLINE
	auto flat_view() noexcept
	{ return make_flat_view<element_from_offset>(*this); }

	template <nd_enable_if(!provides_fast_flat_view)>
	CC_ALWAYS_INLINE
	auto flat_view() const noexcept
	{ return make_flat_view<element_from_offset>(*this); }

	template <nd_enable_if(provides_direct_view)>
	CC_ALWAYS_INLINE
	auto direct_view() noexcept
	{ return m_wrapped.direct_view(); }

	template <nd_enable_if(provides_direct_view)>
	CC_ALWAYS_INLINE
	auto direct_view() const noexcept
	{ return m_wrapped.direct_view(); }

	/*
	** Element accessors.
	*/

	template <class... Ts>
	CC_ALWAYS_INLINE
	auto at(const Ts&... ts)
	noexcept(noexcept(is_noexcept_accessible)) ->
	decltype(m_wrapped.at(ts...))
	{
		nd_assert(
			make_index(ts...) >= extents().start() &&
			make_index(ts...) <= extents().finish(),
			"index out of bounds. Index: $; extents: $",
			make_index(ts...), extents()
		);
		return m_wrapped.at(ts...);
	}

	template <class... Ts>
	CC_ALWAYS_INLINE constexpr
	auto at(const Ts&... ts) const
	noexcept(noexcept(is_noexcept_accessible)) ->
	decltype(m_wrapped.at(ts...))
	{
		nd_assert(
			make_index(ts...) >= extents().start() &&
			make_index(ts...) <= extents().finish(),
			"index out of bounds. Index: $; extents: $",
			make_index(ts...), extents()
		);
		return m_wrapped.at(ts...);
	}

	/*
	** XXX: Don't move these functions above `at`. `at` needs to be defined
	** first so that the noexcept specifications can work here. I don't know
	** why. Clang ICE's if we don't include the `this->`.
	*/

	template <class... Ts>
	CC_ALWAYS_INLINE
	auto operator()(const Ts&... ts)
	nd_deduce_noexcept_and_return_type(this->at(ts...))

	template <class... Ts>
	CC_ALWAYS_INLINE constexpr
	auto operator()(const Ts&... ts) const
	nd_deduce_noexcept_and_return_type(this->at(ts...))

	/*
	** Mutating operations.
	*/

	template <class Range, nd_enable_if(is_safe_resizable)>
	CC_ALWAYS_INLINE
	void safe_resize(const Range& r)
	nd_deduce_noexcept(m_wrapped.safe_resize(r))

	template <class Range, nd_enable_if(is_unsafe_resizable)>
	CC_ALWAYS_INLINE
	void unsafe_resize(const Range& r)
	nd_deduce_noexcept(m_wrapped.unsafe_resize(r))
};

}

#endif
