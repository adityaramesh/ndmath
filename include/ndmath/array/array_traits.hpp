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
	using value_type      = std::decay_t<reference>;

	/*
	** We need a distinct type alias for const_value_type, because `at()`
	** may return a proxy rvalue (e.g. in the case of the bool
	** specialization). This proxy rvalue may have different type depending
	** on whether the access was const or non-const.
	*/
	using const_value_type = std::decay_t<const_reference>;

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
	using value_type       = typename traits::value_type;
	using const_value_type = typename traits::const_value_type;

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

template <class T, class ValueType, bool SupportsDirectView>
struct direct_view_traits;

template <class T, class ValueType>
struct direct_view_traits<T, ValueType, true>
{
	using range           = decltype(std::declval<T>().direct_view());
	using const_range     = decltype(std::declval<const T>().direct_view());
	using iterator        = decltype(std::declval<range>().begin());
	using const_iterator  = decltype(std::declval<const_range>().begin());
	using traits          = std::iterator_traits<iterator>;
	using underlying_type = std::decay_t<typename traits::value_type>;
};

template <class T, class ValueType>
struct direct_view_traits<T, ValueType, false>
{
	using iterator        = void;
	using const_iterator  = void;
	using underlying_type = ValueType;
};

}

template <class T>
struct array_traits
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
	static constexpr auto check_direct_view(U*) ->
	decltype(std::declval<U>().direct_view(), bool{})
	{ return true; }

	template <class U>
	static constexpr auto check_direct_view(...)
	{ return false; }

	template <class U>
	static constexpr auto check_flat_view(U*) ->
	decltype(std::declval<U>().flat_view(), bool{})
	{ return true; }

	template <class U>
	static constexpr auto check_flat_view(...)
	{ return false; }

	template <class U>
	static constexpr auto check_memory_size(U*) ->
	decltype(std::declval<U>().memory_size(), bool{})
	{ return true; }

	template <class U>
	static constexpr auto check_memory_size(...)
	{ return false; }

	static constexpr auto is_view                     = T::is_view;
	static constexpr auto is_conservatively_resizable = check_conservative_resize<T>(0);
	static constexpr auto is_destructively_resizable  = check_destructive_resize<T>(0);
	static constexpr auto provides_direct_view        = check_direct_view<T>(0);
	static constexpr auto provides_fast_flat_view     = check_flat_view<T>(0);
	static constexpr auto provides_memory_size        = check_memory_size<T>(0);

	using et = detail::element_access_traits<T, dims>;

	using size_type        = typename T::size_type;
	using reference        = typename et::reference;
	using const_reference  = typename et::const_reference;
	using value_type       = typename et::value_type;
	using const_value_type = typename et::const_value_type;

	static constexpr auto is_noexcept_accessible =
	et::is_noexcept_accessible;

	using fvt = detail::flat_view_traits<
		T, reference, const_reference, provides_fast_flat_view>;

	using flat_iterator       = typename fvt::iterator;
	using const_flat_iterator = typename fvt::const_iterator;

	using dvt = detail::direct_view_traits<
		T, value_type, provides_direct_view>;

	using direct_iterator       = typename dvt::iterator;
	using const_direct_iterator = typename dvt::const_iterator;
	using underlying_type       = typename dvt::underlying_type;
};

}

#endif
