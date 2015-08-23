/*
** File Name: array_traits.hpp
** Author:    Aditya Ramesh
** Date:      03/17/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z3F960A62_10C1_475D_A1A7_E6635DA0DF77
#define Z3F960A62_10C1_475D_A1A7_E6635DA0DF77

#include <iterator>
#include <ndmath/common.hpp>
#include <ndmath/range.hpp>

namespace nd {
namespace detail {

template <class T, class Seq>
struct element_access_helper;

template <class T, size_t... Ts>
struct element_access_helper<T, std::index_sequence<Ts...>>
{
	using reference       = decltype(std::declval<T>().at(Ts...));
	using const_reference = decltype(std::declval<const T>().at(Ts...));

	static constexpr auto is_noexcept_accessible =
	noexcept(std::declval<T>().at(Ts...));
};

template <class T, size_t Dims>
struct element_access_traits
{
	using seq = mpl::to_values<
		mpl::repeat_nc<Dims, std::integral_constant<size_t, 0>>
	>;

	using traits           = element_access_helper<T, seq>;
	using reference        = typename traits::reference;
	using const_reference  = typename traits::const_reference;

	static constexpr auto is_noexcept_accessible =
	traits::is_noexcept_accessible;
};

template <class T, class Ref, class CRef, bool SupportsFlatView>
struct flat_view_traits;

template <class T, class Ref, class CRef>
struct flat_view_traits<T, Ref, CRef, true>
{
	using range          = decltype(std::declval<T>().flat_view());
	using const_range    = decltype(std::declval<const T>().flat_view());
	using iterator       = decltype(std::declval<range>().begin());
	using const_iterator = decltype(std::declval<const_range>().begin());
	using traits         = std::iterator_traits<iterator>;
	using const_traits   = std::iterator_traits<const_iterator>;

	static_assert(
		std::is_same<typename traits::reference, Ref>::value,
		"Flat view must provide the same functionality as the "
		"subscript operator."
	);

	static_assert(
		std::is_same<typename const_traits::reference, CRef>::value,
		"Flat view must provide the same functionality as the "
		"subscript operator."
	);
};

template <class T, class Ref, class CRef>
struct flat_view_traits<T, Ref, CRef, false>
{
	using iterator       = void;
	using const_iterator = void;
};

template <class T, class ValueType, bool SupportsUnderlyingView>
struct underlying_view_traits;

template <class T, class ValueType>
struct underlying_view_traits<T, ValueType, true>
{
	using range           = decltype(std::declval<T>().underlying_view());
	using const_range     = decltype(std::declval<const T>().underlying_view());
	using iterator        = decltype(std::declval<range>().begin());
	using const_iterator  = decltype(std::declval<const_range>().begin());
	using traits          = std::iterator_traits<iterator>;
	using underlying_type = std::decay_t<typename traits::value_type>;
};

template <class T, class ValueType>
struct underlying_view_traits<T, ValueType, false>
{
	using iterator        = void;
	using const_iterator  = void;
	using underlying_type = ValueType;
};

}

/*
** A version of `array_traits` that does not invoke `flat_view()` or
** `construction_view()`. The reason for this is that some of the structs used
** by the generic implementation of `flat_view()` and `construction_view()` may
** rely on `array_traits`. Since `array_traits` calls `flat_view()`, this would
** cause a circular dependency, and lead to a cryptic error about usage of an
** incomplete type.
*/
template <class T>
struct array_traits_no_view
{
	using storage_order = std::decay_t<
		decltype(std::declval<T>().storage_order())>;

	static constexpr auto dims = storage_order::dims();

	/*
	** An array is resizable only if the range of the RHS is compatible with
	** the range of the LHS. That is, the following conditions must hold:
	** - Both ranges have the same number of dimensions.
	** - If a coordinate of the LHS is a static constant, then the
	** corresponding coordinate of the RHS is the same static constant.
	**
	** Our goal is to determine if the wrapped type is _ever_ resizable. The
	** two conditions given above always hold when we try to resize RHS
	** using its own extents. The code below checks to see if we can do
	** this.
	*/
	template <class U>
	static constexpr auto check_conservative_resize(U*) ->
	decltype(std::declval<U>().conservative_resize(std::declval<U>().extents()), bool{})
	{ return true; }

	template <class U>
	static constexpr auto check_conservative_resize(...)
	{ return false; }

	template <class U>
	static constexpr auto check_destructive_resize(U*) ->
	decltype(std::declval<U>().destructive_resize(std::declval<U>().extents()), bool{})
	{ return true; }

	template <class U>
	static constexpr auto check_destructive_resize(...)
	{ return false; }

	template <class U>
	static constexpr auto check_memory_size(U*) ->
	decltype(std::declval<U>().memory_size(), bool{})
	{ return true; }

	template <class U>
	static constexpr auto check_memory_size(...)
	{ return false; }

	template <class U>
	static constexpr auto check_allocator(U*) ->
	decltype(std::declval<U>().allocator(), bool{})
	{ return true; }

	template <class U>
	static constexpr auto check_allocator(...)
	{ return false; }

	static constexpr auto is_lazy                      = T::is_lazy;
	static constexpr auto is_conservatively_resizable  = check_conservative_resize<T>(0);
	static constexpr auto is_destructively_resizable   = check_destructive_resize<T>(0);
	static constexpr auto provides_memory_size         = check_memory_size<T>(0);
	static constexpr auto provides_allocator           = check_allocator<T>(0);

	using et = detail::element_access_traits<T, dims>;

	using external_type    = typename T::external_type;
	using size_type        = typename T::size_type;
	using reference        = typename et::reference;
	using const_reference  = typename et::const_reference;

	static constexpr auto is_noexcept_accessible =
	et::is_noexcept_accessible;
};

template <class T>
struct array_traits : array_traits_no_view<T>
{
	template <class U>
	static constexpr auto check_underlying_view(U*) ->
	decltype(std::declval<U>().underlying_view(), bool{})
	{ return true; }

	template <class U>
	static constexpr auto check_underlying_view(...)
	{ return false; }

	template <class U>
	static constexpr auto check_construction_view(U*) ->
	decltype(std::declval<U>().construction_view(), bool{})
	{ return true; }

	template <class U>
	static constexpr auto check_construction_view(...)
	{ return false; }

	template <class U>
	static constexpr auto check_flat_view(U*) ->
	decltype(std::declval<U>().flat_view(), bool{})
	{ return true; }

	template <class U>
	static constexpr auto check_flat_view(...)
	{ return false; }

	static constexpr auto provides_underlying_view     = check_underlying_view<T>(0);
	static constexpr auto provides_fast_flat_view      = check_flat_view<T>(0);
	static constexpr auto supports_fast_initialization = check_construction_view<T>(0);

	using base            = array_traits_no_view<T>;
	using reference       = typename base::reference;
	using const_reference = typename base::const_reference;

	using fvt = detail::flat_view_traits<
		T, reference, const_reference, provides_fast_flat_view>;

	using flat_iterator       = typename fvt::iterator;
	using const_flat_iterator = typename fvt::const_iterator;

	using dvt = detail::underlying_view_traits<T, std::decay_t<reference>,
	      provides_underlying_view>;

	using underlying_iterator       = typename dvt::iterator;
	using const_underlying_iterator = typename dvt::const_iterator;
	using underlying_type       = typename dvt::underlying_type;
};


}

#endif
