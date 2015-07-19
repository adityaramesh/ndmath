/*
** File Name: array_wrapper.hpp
** Author:    Aditya Ramesh
** Date:      02/15/2015
** Contact:   _@adityaramesh.com
**
** # Requirements of Wrapped Type
**
** ## Requirement 1: Type Definitions
**
** The wrapped type must define the following types:
** - is_lazy: Indicates whether the array type performs lazy computation when
** accessing elements. This influences how assignment and copy construction is
** performed.
** - exterior_type: The type of the storage underlying the array originally
** requested by the user. Why do we need this? Consider a dense_storage of
** booleans. The underlying_type is unsigned, and the actual return type is
** boolean_proxy, but there's no way for us to query the original type requested
** by the user. This type is necesssary for copy construction.
** - size_type: The integral type used by the array to store information related
** to memory.
**
** ## Requirement 2: Either Support Copy and Move Construction Explicitly, or
** Support Fast Initializaiton
**
** If each array type in the library had to explicitly implement copy and move
** construction and assignment both from itself and all other array types in the
** library, the implementations of the storage classes would become unmanageably
** complex. The `array_wrapper` class cuts down on this complexity by providing
** generic versions of these operations if the wrapped type does not implement
** them explicitly. This process is described in more detail in
** `array_memory_traits.hpp`.
**
** If the wrapped type does not implement a copy constructor, then it must
** provide a constructor of the form `wrapped_type(partial_init_t, const
** wrapped_type&)`. If the wrapped type does not implement a move constructor,
** then it must implement `wrapped_type(uninitialized_t, const wrapped_type&)`.
**
** Similarly, if the wrapped type does not support generic copy construction
** from types `array_wrapper<U>`, then it must provide a constructor of the form
** `wrapped_type(partial_init_t, const array_wrapper<U>&)`. If the wrapped type
** does not implement generic move construction, then it must provide a
** constructor of the form `wrapped_type(uninitialized_t, array_wrapper<U>&&)`.
**
** ## Requirement 3: Member Functions
**
** The wrapped type must also define the following member functions:
** - extents()       (const)
** - storage_order() (const)
** - at()            (const and non-const)
** - memory_size()   (optional, const)
** - allocator()     (optional, const and non-const)
** - flat_view()     (optional, const and non-const)
** - direct_view()   (optional, const and non-const)
** - resize()        (optional, const and non-const)
**
** ## Requirement 4: Optional Support for "Late Initialization"
**
** In Requirement 2, we described how `array_wrapper` provides generic
** implementations of construction and assignment operators if they are not
** implemented explicitly by the wrapped type. The generic construction and
** assignment operations check various traits in order to use the most efficient
** mechanism for data transfer supported by both the source and destination
** arrays. The last resort taken by the generic construction procedure is to
** default-initialize the destination array, and assign the source array to it.
**
** This "last resort" strategy for construction may not always be efficient. But
** if the destination array supports "fast initialization", then copy and move
** construction can be implemented efficiently using assignment operations.
** Specifically, "fast initialization" is a way for the wrapped type to declare
** that it is capable of the following:
**   1. Initializing the storage necessary to contain the elements without
**   initializing the elements themselves (e.g. an array of primitives). This is
**   "partial initialization".
**   2. Initializing the minimal amount of state information necessary, after
**   which it can "steal" the internals of another array using move assignment.
**   This is "uninitialized" construction.
**
** If fast initialization is supported:
** - Any constructor (except the default constructor) that puts the wrapped type
** in an initialized state but *without* explicitly specified initial values for
** its elements must be accompanied by two variants. They should accept
** `partial_init_t` and `uninitialized_t` as their first arguments,
** respectively. The other arguments are those of the original constructor.
**   - This requirement excludes the copy and move constructors, if they are
**   implemented.
**   - The "partial initialization" variant of the constructor is not required to
**   construct the elements of the array (i.e. put the elements in well-defined
**   states). However, the storage necessary to contain the elements must be
**   allocated.
**   - The "uninitialized" variant of the constructor must initialize enough
**   state information such that it becomes a valid object after being move
**   assigned to another array.
** - The following member functions must be implemented:
**   - uninitialized_at()  (non-const only)
**   - construction_view() (non-const only)
*/

#ifndef Z9FD66BF0_E92D_4CAE_A49B_8D7708927910
#define Z9FD66BF0_E92D_4CAE_A49B_8D7708927910

#include <ndmath/array/array_traits.hpp>
#include <ndmath/array/array_assignment.hpp>
#include <ndmath/array/array_construction.hpp>
#include <ndmath/array/flat_iterator.hpp>
#include <ndmath/array/element_from_offset.hpp>
#include <ndmath/array/initializer_list.hpp>

namespace nd {

// Used to prepare the wrapped type for copy construction when it does not
// support it directly. In this case, the wrapped type is constructed in a
// "partially initialized" state, and copy assigned to the source.
struct partial_init_t {};
static constexpr auto partial_init = partial_init_t{};

// Used to prepare the wrapped type for move construction when it does not
// support it directly. In this case, the wrapped type is constructed in an
// "uninitialized" state, and move assigned to the source.
struct uninitialized_t {};
static constexpr auto uninitialized = uninitialized_t{};

namespace detail {

/*
** The base class has two purposes:
**   1. To conditionally call constructors of the wrapped type in contexts where
**   they cannot be disabled by the derived class using `enable_if`.
**   2. To call the appropriate functions of the wrapped type when
**   `array_wrapper` is copy- or move-constructed. If the wrapped type supports
**   direct copy construction, then we simply call its copy constructor.
**   Otherwise, we 
*/

template <
	class T,
	bool SupportsFastInitialization,
	bool SupportsDirectCopyConstruction,
	bool SupportsDirectMoveConstruction
>
class array_wrapper_base;

template <class T, bool SupportsFastInitialization>
class array_wrapper_base<T, SupportsFastInitialization, false, false>
{
protected:
	T m_wrapped;
public:
	template <class... Args>
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper_base(Args&&... args)
	noexcept(std::is_nothrow_constructible<T, Args...>::value)
	: m_wrapped{std::forward<Args>(args)...} {}

	template <class... Args, nd_enable_if(!SupportsFastInitialization)>
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper_base(partial_init_t, Args&&... args)
	noexcept(std::is_nothrow_constructible<T, Args...>::value)
	: m_wrapped{std::forward<Args>(args)...} {}

	template <class... Args, nd_enable_if(!SupportsFastInitialization)>
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper_base(uninitialized_t, Args&&... args)
	noexcept(std::is_nothrow_constructible<T, Args...>::value)
	: m_wrapped{std::forward<Args>(args)...} {}

	template <class... Args, nd_enable_if(SupportsFastInitialization)>
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper_base(partial_init_t, Args&&... args)
	noexcept(std::is_nothrow_constructible<T, partial_init_t, Args...>::value)
	: m_wrapped{partial_init, std::forward<Args>(args)...} {}

	template <class... Args, nd_enable_if(SupportsFastInitialization)>
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper_base(uninitialized_t, Args&&... args)
	noexcept(std::is_nothrow_constructible<T, uninitialized_t, Args...>::value)
	: m_wrapped{uninitialized, std::forward<Args>(args)...} {}

	CC_ALWAYS_INLINE constexpr
	array_wrapper_base(const T& rhs)
	noexcept(std::is_nothrow_constructible<T, partial_init_t, const T&>::value)
	: m_wrapped{partial_init, rhs} {}

	CC_ALWAYS_INLINE constexpr
	array_wrapper_base(T&& rhs)
	noexcept(std::is_nothrow_constructible<T, partial_init_t, T&&>::value)
	: m_wrapped{uninitialized, std::move(rhs)} {}
};

template <class T, bool SupportsFastInitialization>
class array_wrapper_base<T, SupportsFastInitialization, true, false>
{
protected:
	T m_wrapped;
public:
	template <class... Args>
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper_base(Args&&... args)
	noexcept(std::is_nothrow_constructible<T, Args...>::value)
	: m_wrapped{std::forward<Args>(args)...} {}

	template <class... Args, nd_enable_if(!SupportsFastInitialization)>
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper_base(partial_init_t, Args&&... args)
	noexcept(std::is_nothrow_constructible<T, Args...>::value)
	: m_wrapped{std::forward<Args>(args)...} {}

	template <class... Args, nd_enable_if(!SupportsFastInitialization)>
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper_base(uninitialized_t, Args&&... args)
	noexcept(std::is_nothrow_constructible<T, Args...>::value)
	: m_wrapped{std::forward<Args>(args)...} {}

	template <class... Args, nd_enable_if(SupportsFastInitialization)>
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper_base(partial_init_t, Args&&... args)
	noexcept(std::is_nothrow_constructible<T, partial_init_t, Args...>::value)
	: m_wrapped{partial_init, std::forward<Args>(args)...} {}

	template <class... Args, nd_enable_if(SupportsFastInitialization)>
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper_base(uninitialized_t, Args&&... args)
	noexcept(std::is_nothrow_constructible<T, uninitialized_t, Args...>::value)
	: m_wrapped{uninitialized, std::forward<Args>(args)...} {}

	CC_ALWAYS_INLINE constexpr
	array_wrapper_base(const T& rhs)
	noexcept(std::is_nothrow_copy_constructible<T>::value)
	: m_wrapped{rhs} {}

	CC_ALWAYS_INLINE constexpr
	array_wrapper_base(T&& rhs)
	noexcept(std::is_nothrow_constructible<T, uninitialized_t, T&&>::value)
	: m_wrapped{uninitialized, std::move(rhs)} {}
};

template <class T, bool SupportsFastInitialization>
class array_wrapper_base<T, SupportsFastInitialization, false, true>
{
protected:
	T m_wrapped;
public:
	template <class... Args>
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper_base(Args&&... args)
	noexcept(std::is_nothrow_constructible<T, Args...>::value)
	: m_wrapped{std::forward<Args>(args)...} {}

	template <class... Args, nd_enable_if(!SupportsFastInitialization)>
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper_base(partial_init_t, Args&&... args)
	noexcept(std::is_nothrow_constructible<T, Args...>::value)
	: m_wrapped{std::forward<Args>(args)...} {}

	template <class... Args, nd_enable_if(!SupportsFastInitialization)>
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper_base(uninitialized_t, Args&&... args)
	noexcept(std::is_nothrow_constructible<T, Args...>::value)
	: m_wrapped{std::forward<Args>(args)...} {}

	template <class... Args, nd_enable_if(SupportsFastInitialization)>
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper_base(partial_init_t, Args&&... args)
	noexcept(std::is_nothrow_constructible<T, partial_init_t, Args...>::value)
	: m_wrapped{partial_init, std::forward<Args>(args)...} {}

	template <class... Args, nd_enable_if(SupportsFastInitialization)>
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper_base(uninitialized_t, Args&&... args)
	noexcept(std::is_nothrow_constructible<T, uninitialized_t, Args...>::value)
	: m_wrapped{uninitialized, std::forward<Args>(args)...} {}

	CC_ALWAYS_INLINE constexpr
	array_wrapper_base(const T& rhs)
	noexcept(std::is_nothrow_constructible<T, partial_init_t, const T&>::value)
	: m_wrapped{partial_init, rhs} {}

	CC_ALWAYS_INLINE constexpr
	array_wrapper_base(T&& rhs)
	noexcept(std::is_nothrow_move_constructible<T>::value)
	: m_wrapped{std::move(rhs)} {}
};

template <class T, bool SupportsFastInitialization>
class array_wrapper_base<T, SupportsFastInitialization, true, true>
{
protected:
	T m_wrapped;
public:
	template <class... Args>
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper_base(Args&&... args)
	noexcept(std::is_nothrow_constructible<T, Args...>::value)
	: m_wrapped{std::forward<Args>(args)...} {}

	template <class... Args, nd_enable_if(!SupportsFastInitialization)>
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper_base(partial_init_t, Args&&... args)
	noexcept(std::is_nothrow_constructible<T, Args...>::value)
	: m_wrapped{std::forward<Args>(args)...} {}

	template <class... Args, nd_enable_if(!SupportsFastInitialization)>
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper_base(uninitialized_t, Args&&... args)
	noexcept(std::is_nothrow_constructible<T, Args...>::value)
	: m_wrapped{std::forward<Args>(args)...} {}

	template <class... Args, nd_enable_if(SupportsFastInitialization)>
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper_base(partial_init_t, Args&&... args)
	noexcept(std::is_nothrow_constructible<T, partial_init_t, Args...>::value)
	: m_wrapped{partial_init, std::forward<Args>(args)...} {}

	template <class... Args, nd_enable_if(SupportsFastInitialization)>
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper_base(uninitialized_t, Args&&... args)
	noexcept(std::is_nothrow_constructible<T, uninitialized_t, Args...>::value)
	: m_wrapped{uninitialized, std::forward<Args>(args)...} {}

	CC_ALWAYS_INLINE constexpr
	array_wrapper_base(const T& rhs)
	noexcept(std::is_nothrow_copy_constructible<T>::value)
	: m_wrapped{rhs} {}

	CC_ALWAYS_INLINE constexpr
	array_wrapper_base(T&& rhs)
	noexcept(std::is_nothrow_move_constructible<T>::value)
	: m_wrapped{std::move(rhs)} {}
};

}

template <class T>
class array_wrapper final :
detail::array_wrapper_base<
	T,
	array_traits<T>::supports_fast_initialization,
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
		traits::supports_fast_initialization,
		is_copy_constructible,
		is_move_constructible
	>;
	using base::m_wrapped;

	using assignment_helper = detail::assignment_helper;
	using construction_helper = detail::construction_helper;
public:
	using wrapped_type     = T;
	using exterior_type    = typename traits::exterior_type;
	using size_type        = typename traits::size_type;
	using reference        = typename traits::reference;
	using const_reference  = typename traits::const_reference;

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
	static constexpr auto provides_allocator           = traits::provides_allocator;
	static constexpr auto supports_fast_initialization = traits::supports_fast_initialization;

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
		!mpl::is_specialization_of<
			nd::array_wrapper,
			std::decay_t<mpl::at_c<0, mpl::list<Args...>>>
		>::value
	))>
	CC_ALWAYS_INLINE constexpr
	explicit array_wrapper(Args&&... args)
	noexcept(std::is_nothrow_constructible<base, Args...>::value)
	: base(std::forward<Args>(args)...) {}

	/*
	** Copy and move constructors.
	*/

	/*
	** If T is directly copy constructible, the base class will call T's
	** copy constructor, and the body of this function will do nothing.
	** Otherwise:
	**   - If T supports fast initialization, the base class will construct
	**   T in an partially-initialized state.
	**   - Else, the base class will default-construct T.
	**   - Afterwards, the body of this function will invoke the
	**   construction helper.
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
			noexcept(construction_helper::move_construct(*this, std::move(rhs)))
		)
	) : base{std::move(rhs.wrapped())}
	{ construction_helper::move_construct(*this, std::move(rhs)); }

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
	** - If T supports fast initialization, the base class will construct T
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
		std::is_nothrow_constructible<T, partial_init_t, Args...>::value &&
		noexcept(construction_helper::copy_construct(*this, rhs))
	) : base{partial_init, std::forward<Args>(args)...}
	{ construction_helper::copy_construct(*this, rhs); }

	/*
	** This overload allows copy construction of the form `some_type b = a`,
	** where `some_type != decltype(a)`.
	*/
	template <class U, nd_enable_if((
		!std::is_constructible<T, const U&>::value))>
	CC_ALWAYS_INLINE constexpr
	array_wrapper(const array_wrapper<U>& rhs)
	noexcept(
		std::is_nothrow_constructible<T, partial_init_t, decltype(rhs)>::value &&
		noexcept(construction_helper::copy_construct(*this, rhs))
	) : base{partial_init, rhs}
	{ construction_helper::copy_construct(*this, rhs); }

	/*
	** See comments for generic copy construction.
	*/
	template <class U, class... Args, nd_enable_if((
		std::is_constructible<T, U&&>::value))>
	CC_ALWAYS_INLINE constexpr
	array_wrapper(array_wrapper<U>&& rhs, Args&&... args)
	noexcept(std::is_nothrow_constructible<T, U&&, Args...>::value)
	: base{std::move(rhs.wrapped()), std::forward<Args>(args)...} {}

	/*
	** See comments for generic copy construction.
	*/
	template <class U, class... Args, nd_enable_if((
		!std::is_constructible<T, U&&>::value))>
	CC_ALWAYS_INLINE constexpr
	array_wrapper(array_wrapper<U>&& rhs, Args&&... args)
	noexcept(
		std::is_nothrow_constructible<T, uninitialized_t, Args...>::value &&
		noexcept(construction_helper::move_construct(*this, std::move(rhs)))
	) : base{uninitialized, std::forward<Args>(args)...}
	{ construction_helper::move_construct(*this, std::move(rhs)); }

	/*
	** See comments for generic copy construction.
	*/
	template <class U, nd_enable_if((
		!std::is_constructible<T, U&&>::value))>
	CC_ALWAYS_INLINE constexpr
	array_wrapper(array_wrapper<U>&& rhs)
	noexcept(
		std::is_nothrow_constructible<T, uninitialized_t, decltype(rhs)>::value &&
		noexcept(construction_helper::move_construct(*this, std::move(rhs)))
	) : base{uninitialized, std::move(rhs)}
	{ construction_helper::move_construct(*this, std::move(rhs)); }

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

	template <class U>
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

	template <class U>
	CC_ALWAYS_INLINE auto&
	operator=(array_wrapper<U>&& rhs)
	noexcept(noexcept(
		assignment_helper::move_assign(*this, std::move(rhs))
	))
	{
		assignment_helper::move_assign(*this, std::move(rhs));
		return *this;
	}

	CC_ALWAYS_INLINE auto&
	operator=(const nested_initializer_list<exterior_type, traits::dims>& list)
	noexcept(noexcept(std::is_nothrow_assignable<exterior_type, exterior_type>::value))
	{
		for_each(extents(), [&] (const auto& i)
			CC_ALWAYS_INLINE noexcept {
				/*
				** XXX: For some reason, moving the assignment
				** to `get_init_list_element` outside the inner
				** lambda results in a compiler error regarding
				** the expression not being assignable. But it
				** works in `dense_storage`. I suspect that this
				** is a compiler bug.
				*/
				expand_index([&] (auto... ts) CC_ALWAYS_INLINE noexcept {
					at(ts...) =
					get_init_list_element<exterior_type, dims()>(list, i);
				}, i);
			});
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

	template <nd_enable_if(provides_allocator)>
	CC_ALWAYS_INLINE constexpr
	auto allocator() noexcept
	{ return m_wrapped.allocator(); }

	template <nd_enable_if(provides_allocator)>
	CC_ALWAYS_INLINE constexpr
	auto allocator() const noexcept
	{ return m_wrapped.allocator(); }

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

	template <nd_enable_if(supports_fast_initialization)>
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
	decltype(auto) at(const Ts&... ts)
	noexcept(noexcept(is_noexcept_accessible))
	{
		nd_assert(
			c_index(ts...) >= extents().start() &&
			c_index(ts...) <= extents().finish(),
			"index out of bounds.\n▶ $ ∉ range $",
			c_index(ts...), extents()
		);
		return m_wrapped.at(ts...);
	}

	template <class... Ts, nd_enable_if((
		sizeof...(Ts) == dims() &&
		mpl::all_true<std::is_integral<Ts>::value...>
	))>
	CC_ALWAYS_INLINE constexpr
	decltype(auto) at(const Ts&... ts) const
	noexcept(noexcept(is_noexcept_accessible))
	{
		nd_assert(
			c_index(ts...) >= extents().start() &&
			c_index(ts...) <= extents().finish(),
			"index out of bounds.\n▶ $ ∉ range $",
			c_index(ts...), extents()
		);
		return m_wrapped.at(ts...);
	}

	template <class... Ts, nd_enable_if((
		supports_fast_initialization &&
		sizeof...(Ts) == dims()      &&
		mpl::all_true<std::is_integral<Ts>::value...>
	))>
	CC_ALWAYS_INLINE
	decltype(auto) uninitialized_at(const Ts&... ts)
	noexcept(noexcept(is_noexcept_accessible))
	{
		nd_assert(
			c_index(ts...) >= extents().start() &&
			c_index(ts...) <= extents().finish(),
			"index out of bounds.\n▶ $ ∉ range $",
			c_index(ts...), extents()
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

	/*
	** Relational operations.
	*/

	//#define nd_define_relational_op(symbol)
};

}

#endif
