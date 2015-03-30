/*
** File Name: array_wrapper.hpp
** Author:    Aditya Ramesh
** Date:      02/15/2015
** Contact:   _@adityaramesh.com
**
** The wrapped type must define the following types:
** - is_lazy: Indicates whether the array type performs lazy computation when
** accessing elements. This influences how assignment and copy construction is
** performed.
** - size_type: The integral type used by the array to store information related
** to memory.
**
** If the wrapped type implements a constructor that accepts an array along with
** additional arguments, then the array must be the first argument. Otherwise,
** `array_wrapped` will fail to discover that the wrapped type supports this
** form of construction.
**
** The wrapped type must also define the following member functions:
** - extents()
** - storage_order()
** - at()            (const and non-const variants)
** - memory_size()   (optional)
** - flat_view()     (optional)
** - direct_view()   (optional)
** - resize()        (optional)
**
** If late initialization is supported:
** - Any constructor (except the copy and move constructor, but including the
** default constructor) must be accompanied by a variant that accepts
** `uninitialized_t` at the first argument, followed by the same arguments as
** the original constructor.
**   - The "uninitialized" version of the constructor is not required to
**   initialize the elements of the array. However, the storage necessary to
**   contain the elements must be allocated.
** - uninitialized_at()  (non-const only)
** - construction_view() (non-const only)
*/

#ifndef Z9FD66BF0_E92D_4CAE_A49B_8D7708927910
#define Z9FD66BF0_E92D_4CAE_A49B_8D7708927910

#include <ndmath/array/array_traits.hpp>
#include <ndmath/array/array_assignment.hpp>
#include <ndmath/array/array_construction.hpp>
#include <ndmath/array/flat_iterator.hpp>
#include <ndmath/array/element_from_offset.hpp>

namespace nd {

struct uninitialized_t {};
static constexpr auto uninitialized = uninitialized_t{};

namespace detail {

template <class T>
struct is_array
{ static constexpr auto value = false; };

template <class T>
struct is_array<array_wrapper<T>>
{ static constexpr auto value = true; };

/*
** The purpose of the base class is to conditionally call constructors of the
** wrapped type in contexts where they could not be disabled by the derived
** class using `enable_if`.
*/

template <
	class T,
	bool SupportsLateInitialization,
	bool IsCopyConstructible,
	bool IsMoveConstructible
>
class array_wrapper_base;

template <class T, bool SupportsLateInitialization>
class array_wrapper_base<T, SupportsLateInitialization, false, false>
{
protected:
	T m_wrapped;
public:
	template <class... Args>
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper_base(Args&&... args)
	noexcept(std::is_nothrow_constructible<T, Args...>::value)
	: m_wrapped{std::forward<Args>(args)...} {}

	template <class... Args, nd_enable_if(!SupportsLateInitialization)>
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper_base(uninitialized_t, Args&&... args)
	noexcept(std::is_nothrow_default_constructible<T>::value)
	: m_wrapped{std::forward<Args>(args)...} {}

	template <class... Args, nd_enable_if(SupportsLateInitialization)>
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper_base(uninitialized_t, Args&&... args)
	noexcept(std::is_nothrow_default_constructible<T>::value)
	: m_wrapped{/*uninitialized, */std::forward<Args>(args)...} {}

	CC_ALWAYS_INLINE constexpr
	array_wrapper_base(const T&)
	noexcept {}

	CC_ALWAYS_INLINE constexpr
	array_wrapper_base(T&&)
	noexcept {}

	/*
	** The generic copy and move constructors need to be implemented here,
	** because the base class cannot initialize `m_wrapped` directly. It's
	** safe to call the wrapped type's generic copy or move constructors,
	** even though they may not exist, because the derived class will only
	** invoke these functions under the appropriate conditions.
	*/

	// TODO remove the enable_ifs below

	template <class U, class... Args, nd_enable_if((
		std::is_constructible<T, const U&, Args...>::value))>
	CC_ALWAYS_INLINE constexpr
	array_wrapper_base(const U& rhs, Args&&... args)
	noexcept(std::is_nothrow_constructible<T, const U&, Args...>::value)
	: m_wrapped{rhs, std::forward<Args>(args)...} {}

	template <class U, class... Args, nd_enable_if((
		std::is_constructible<T, U&&, Args...>::value))>
	CC_ALWAYS_INLINE constexpr
	array_wrapper_base(U&& rhs, Args&&... args)
	noexcept(std::is_nothrow_constructible<T, U&&, Args...>::value)
	: m_wrapped{std::move(rhs), std::forward<Args>(args)...} {}
};

template <class T, bool SupportsLateInitialization>
class array_wrapper_base<T, SupportsLateInitialization, true, false>
{
protected:
	T m_wrapped;
public:
	template <class... Args>
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper_base(Args&&... args)
	noexcept(std::is_nothrow_constructible<T, Args...>::value)
	: m_wrapped{std::forward<Args>(args)...} {}

	template <class... Args, nd_enable_if(!SupportsLateInitialization)>
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper_base(uninitialized_t, Args&&... args)
	noexcept(std::is_nothrow_default_constructible<T>::value)
	: m_wrapped{std::forward<Args>(args)...} {}

	template <class... Args, nd_enable_if(SupportsLateInitialization)>
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper_base(uninitialized_t, Args&&... args)
	noexcept(std::is_nothrow_default_constructible<T>::value)
	: m_wrapped{/*uninitialized, */std::forward<Args>(args)...} {}

	CC_ALWAYS_INLINE constexpr
	array_wrapper_base(const T& rhs)
	noexcept(std::is_nothrow_copy_constructible<T>::value)
	: m_wrapped{rhs} {}

	CC_ALWAYS_INLINE constexpr
	array_wrapper_base(T&&)
	noexcept {}

	template <class U, class... Args>
	CC_ALWAYS_INLINE constexpr
	array_wrapper_base(const U& rhs, Args&&... args)
	noexcept(std::is_nothrow_constructible<T, const U&, Args...>::value)
	: m_wrapped{rhs, std::forward<Args>(args)...} {}

	template <class U, class... Args>
	CC_ALWAYS_INLINE constexpr
	array_wrapper_base(U&& rhs, Args&&... args)
	noexcept(std::is_nothrow_constructible<T, U&&, Args...>::value)
	: m_wrapped{std::move(rhs), std::forward<Args>(args)...} {}
};

template <class T, bool SupportsLateInitialization>
class array_wrapper_base<T, SupportsLateInitialization, false, true>
{
protected:
	T m_wrapped;
public:
	template <class... Args>
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper_base(Args&&... args)
	noexcept(std::is_nothrow_constructible<T, Args...>::value)
	: m_wrapped{std::forward<Args>(args)...} {}

	template <class... Args, nd_enable_if(!SupportsLateInitialization)>
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper_base(uninitialized_t, Args&&... args)
	noexcept(std::is_nothrow_default_constructible<T>::value)
	: m_wrapped{std::forward<Args>(args)...} {}

	template <class... Args, nd_enable_if(SupportsLateInitialization)>
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper_base(uninitialized_t, Args&&... args)
	noexcept(std::is_nothrow_default_constructible<T>::value)
	: m_wrapped{/*uninitialized,*/std::forward<Args>(args)...} {}

	CC_ALWAYS_INLINE constexpr
	array_wrapper_base(const T&)
	noexcept {}

	CC_ALWAYS_INLINE constexpr
	array_wrapper_base(T&& rhs)
	noexcept(std::is_nothrow_move_constructible<T>::value)
	: m_wrapped{std::move(rhs)} {}

	template <class U, class... Args>
	CC_ALWAYS_INLINE constexpr
	array_wrapper_base(const U& rhs, Args&&... args)
	noexcept(std::is_nothrow_constructible<T, const U&, Args...>::value)
	: m_wrapped{rhs, std::forward<Args>(args)...} {}

	template <class U, class... Args>
	CC_ALWAYS_INLINE constexpr
	array_wrapper_base(U&& rhs, Args&&... args)
	noexcept(std::is_nothrow_constructible<T, U&&, Args...>::value)
	: m_wrapped{std::move(rhs), std::forward<Args>(args)...} {}
};

template <class T, bool SupportsLateInitialization>
class array_wrapper_base<T, SupportsLateInitialization, true, true>
{
protected:
	T m_wrapped;
public:
	template <class... Args>
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper_base(Args&&... args)
	noexcept(std::is_nothrow_constructible<T, Args...>::value)
	: m_wrapped{std::forward<Args>(args)...} {}

	template <class... Args, nd_enable_if(!SupportsLateInitialization)>
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper_base(uninitialized_t, Args&&... args)
	noexcept(std::is_nothrow_default_constructible<T>::value)
	: m_wrapped{std::forward<Args>(args)...} {}

	template <class... Args, nd_enable_if(SupportsLateInitialization)>
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper_base(uninitialized_t, Args&&... args)
	noexcept(std::is_nothrow_default_constructible<T>::value)
	: m_wrapped{/*uninitialized, */std::forward<Args>(args)...} {}

	CC_ALWAYS_INLINE constexpr
	array_wrapper_base(const T& rhs)
	noexcept(std::is_nothrow_copy_constructible<T>::value)
	: m_wrapped{rhs} {}

	CC_ALWAYS_INLINE constexpr
	array_wrapper_base(T&& rhs)
	noexcept(std::is_nothrow_move_constructible<T>::value)
	: m_wrapped{std::move(rhs)} {}

	template <class U, class... Args>
	CC_ALWAYS_INLINE constexpr
	array_wrapper_base(const U& rhs, Args&&... args)
	noexcept(std::is_nothrow_constructible<T, const U&, Args...>::value)
	: m_wrapped{rhs, std::forward<Args>(args)...} {}

	template <class U, class... Args>
	CC_ALWAYS_INLINE constexpr
	array_wrapper_base(U&& rhs, Args&&... args)
	noexcept(std::is_nothrow_constructible<T, U&&, Args...>::value)
	: m_wrapped{std::move(rhs), std::forward<Args>(args)...} {}
};

}

template <class T>
class array_wrapper final :
detail::array_wrapper_base<
	T,
	array_traits<T>::supports_late_initialization,
	std::is_copy_constructible<T>::value,
	std::is_move_constructible<T>::value
>
{
	static constexpr auto is_copy_constructible =
	std::is_copy_constructible<T>::value;

	static constexpr auto is_move_constructible =
	std::is_move_constructible<T>::value;

	using self = array_wrapper<T>;
	using traits = array_traits<T>;

	using base = detail::array_wrapper_base<
		T,
		traits::supports_late_initialization,
		is_copy_constructible,
		is_move_constructible
	>;
	using base::m_wrapped;

	using assignment_helper = detail::assignment_helper;
	using construction_helper = detail::construction_helper;
public:
	using wrapped_type     = T;
	using size_type        = typename traits::size_type;
	using reference        = typename traits::reference;
	using const_reference  = typename traits::const_reference;
	using value_type       = typename traits::value_type;
	using const_value_type = typename traits::value_type;

	using direct_iterator       = typename traits::direct_iterator;
	using const_direct_iterator = typename traits::const_direct_iterator;
	using underlying_type       = typename traits::underlying_type;

	static constexpr auto is_lazy                      = traits::is_lazy;
	static constexpr auto is_conservatively_resizable  = traits::is_conservatively_resizable;
	static constexpr auto is_destructively_resizable   = traits::is_destructively_resizable;
	static constexpr auto is_noexcept_accessible       = traits::is_noexcept_accessible;
	static constexpr auto provides_direct_view         = traits::provides_direct_view;
	static constexpr auto provides_fast_flat_view      = traits::provides_fast_flat_view;
	static constexpr auto provides_memory_size         = traits::provides_memory_size;
	static constexpr auto supports_late_initialization = traits::supports_late_initialization;

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

	template <class... Args, nd_enable_if((
		!detail::is_array<std::decay_t<
			mpl::at_c<0, mpl::list<Args...>>
		>>::value
	))>
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper(Args&&... args)
	noexcept(std::is_nothrow_constructible<base, Args...>::value)
	: base(std::forward<Args>(args)...) {}

	/*
	** Copy and move constructors.
	*/

	/*
	** If T is copy constructible, the base class will call T's copy
	** constructor, and the body of this function will do nothing.
	** Otherwise:
	**   - If T supports late initialization, the base class will construct
	**   T in an uninitialized state.
	**   - Else, the base class will default construct T, and the body of
	**   this function will perform copy assignment.
	**   - Afterwards, the body of this function will invoke the assignment
	**   helper.
	*/
	CC_ALWAYS_INLINE constexpr
	array_wrapper(const array_wrapper& rhs)
	noexcept(
		(
			is_copy_constructible &&
			std::is_nothrow_constructible<base, const T&>::value
		) ||
		(
			!is_copy_constructible &&
			std::is_nothrow_constructible<base, const T&>::value &&
			noexcept(construction_helper::copy_construct(*this, rhs))
		)
	) : base{rhs.wrapped()}
	{ construction_helper::copy_construct(*this, rhs); }

	/*
	** Refer to comments for the copy constructor above.
	*/
	CC_ALWAYS_INLINE constexpr
	array_wrapper(array_wrapper&& rhs)
	noexcept(
		(
			is_move_constructible &&
			std::is_nothrow_constructible<base, T&&>::value
		) ||
		(
			!is_move_constructible &&
			std::is_nothrow_constructible<base, T&&>::value &&
			// TODO change below to use assignment helper
			noexcept(*this = std::move(rhs))
		)
	) : base{std::move(rhs.wrapped())}
	{
		// TODO: replace with construction_helper
		if (!is_move_constructible)
			*this = std::move(rhs);
	}

	/*
	** If the wrapped type has a copy constructor for U, then there is no
	** need for us to invoke the construction helper, since the base class
	** will take care of things for us.
	*/
	template <class U, class... Args, nd_enable_if((
		std::is_constructible<T, const U&>::value))>
	CC_ALWAYS_INLINE constexpr
	array_wrapper(const array_wrapper<U>& rhs, Args&&... args)
	noexcept(std::is_nothrow_constructible<T, const U&, Args...>::value)
	: base{rhs.wrapped(), std::forward<Args>(args)...} {}

	/*
	** If the wrapped type does not have a copy constructor for U, then we
	** need to invoke the assignment helper.
	** - If T supports late initialization, the base class will construct T
	** in an uninitialized state.
	** - Else, the base class will default construct T, and the body of this
	** function will perform copy assignment.
	** - Afterwards, the body of this function will invoke the assignment
	** helper.
	*/
	template <class U, class... Args, nd_enable_if((
		!std::is_constructible<T, const U&>::value))>
	CC_ALWAYS_INLINE constexpr
	array_wrapper(const array_wrapper<U>& rhs, Args&&... args)
	noexcept(
		std::is_nothrow_constructible<T, uninitialized_t, Args...>::value &&
		noexcept(construction_helper::copy_construct(*this, rhs))
	) : base{uninitialized, std::forward<Args>(args)...}
	{ construction_helper::copy_construct(*this, rhs); }

	/*
	** See comments for generic copy assignment.
	*/
	template <class U, class... Args, nd_enable_if((
		std::is_constructible<T, U&&>::value))>
	CC_ALWAYS_INLINE constexpr
	array_wrapper(array_wrapper<U>&& rhs, Args&&... args)
	noexcept(std::is_nothrow_constructible<T, U&&, Args...>::value)
	: base{std::move(rhs.wrapped()), std::forward<Args>(args)...} {}

	/*
	** See comments for generic copy assignment.
	*/
	template <class U, class... Args, nd_enable_if((
		!std::is_constructible<T, U&&>::value))>
	CC_ALWAYS_INLINE constexpr
	array_wrapper(array_wrapper<U>&& rhs, Args&&... args)
	noexcept(
		std::is_nothrow_constructible<T, uninitialized_t, Args...>::value &&
		// TODO replace below with construction helper
		noexcept(*this = std::move(rhs))
	) : base{uninitialized, std::forward<Args>(args)...}
	// TODO: replace with construction_helper
	{ *this = std::move(rhs); }

	/*
	** Assignment operators.
	*/

	CC_ALWAYS_INLINE auto&
	operator=(const array_wrapper& rhs)
	noexcept(noexcept(assignment_helper::copy_assign(*this, rhs)))
	{
		assignment_helper::copy_assign(*this, rhs);
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
	noexcept(noexcept(assignment_helper::copy_assign(*this, rhs)))
	{
		assignment_helper::copy_assign(*this, rhs);
		return *this;
	}

	CC_ALWAYS_INLINE auto&
	operator=(array_wrapper&& rhs)
	noexcept(noexcept(
		assignment_helper::move_assign(*this, std::move(rhs))
	))
	{
		assignment_helper::move_assign(*this, std::move(rhs));
		return *this;
	}

	/*
	** XXX: The enable_if here is commented out for the same reason that the
	** one for the generic copy assignment operator above is commented out.
	*/
	template <class U/*, nd_enable_if((
		std::is_assignable<
			reference,
			std::remove_reference_t<
				typename array_wrapper<U>::reference
			>&&
		>::value
	))*/>
	CC_ALWAYS_INLINE auto&
	operator=(array_wrapper<U>&& rhs)
	noexcept(noexcept(
		assignment_helper::move_assign(*this, std::move(rhs))
	))
	{
		assignment_helper::move_assign(*this, std::move(rhs));
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
	{ return make_flat_view<element_from_offset>(*this, size()); }

	template <nd_enable_if(!provides_fast_flat_view)>
	CC_ALWAYS_INLINE
	auto flat_view() const noexcept
	{ return make_flat_view<element_from_offset>(*this, size()); }

	template <nd_enable_if(provides_direct_view)>
	CC_ALWAYS_INLINE
	auto direct_view() noexcept
	{ return m_wrapped.direct_view(); }

	template <nd_enable_if(provides_direct_view)>
	CC_ALWAYS_INLINE
	auto direct_view() const noexcept
	{ return m_wrapped.direct_view(); }

	template <nd_enable_if(supports_late_initialization)>
	CC_ALWAYS_INLINE
	auto construction_view() noexcept
	{ return m_wrapped.construction_view(); }

	/*
	** Element accessors.
	*/

	template <class... Ts, nd_enable_if((
		sizeof...(Ts) == dims() &&
		mpl::all_true<std::is_integral<Ts>::value...>
	))>
	CC_ALWAYS_INLINE
	auto at(const Ts&... ts)
	noexcept(noexcept(is_noexcept_accessible)) ->
	decltype(m_wrapped.at(ts...))
	{
		nd_assert(
			make_index(ts...) >= extents().start() &&
			make_index(ts...) <= extents().finish(),
			"index out of bounds.\n▶ $ ∉ range $",
			make_index(ts...), extents()
		);
		return m_wrapped.at(ts...);
	}

	template <class... Ts, nd_enable_if((
		sizeof...(Ts) == dims() &&
		mpl::all_true<std::is_integral<Ts>::value...>
	))>
	CC_ALWAYS_INLINE constexpr
	auto at(const Ts&... ts) const
	noexcept(noexcept(is_noexcept_accessible)) ->
	decltype(m_wrapped.at(ts...))
	{
		nd_assert(
			make_index(ts...) >= extents().start() &&
			make_index(ts...) <= extents().finish(),
			"index out of bounds.\n▶ $ ∉ range $",
			make_index(ts...), extents()
		);
		return m_wrapped.at(ts...);
	}

	template <class... Ts, nd_enable_if((
		supports_late_initialization &&
		sizeof...(Ts) == dims()      &&
		mpl::all_true<std::is_integral<Ts>::value...>
	))>
	CC_ALWAYS_INLINE
	auto uninitialized_at(const Ts&... ts)
	noexcept(noexcept(is_noexcept_accessible)) ->
	decltype(m_wrapped.uninitialized_at(ts...))
	{
		nd_assert(
			make_index(ts...) >= extents().start() &&
			make_index(ts...) <= extents().finish(),
			"index out of bounds.\n▶ $ ∉ range $",
			make_index(ts...), extents()
		);
		return m_wrapped.uninitialized_at(ts...);
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

	template <class Range, nd_enable_if(is_conservatively_resizable)>
	CC_ALWAYS_INLINE
	void conservative_resize(const Range& r)
	nd_deduce_noexcept(m_wrapped.conservative_resize(r))

	template <class Range, nd_enable_if(is_destructively_resizable)>
	CC_ALWAYS_INLINE
	void destructive_resize(const Range& r)
	nd_deduce_noexcept(m_wrapped.destructive_resize(r))
};

}

#endif
