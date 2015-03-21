/*
** File Name: traits_test.cpp
** Author:    Aditya Ramesh
** Date:      03/20/2015
** Contact:   _@adityaramesh.com
*/

#include <ccbase/unit_test.hpp>
#include <ndmath/array/array_traits.hpp>
#include <ndmath/array/element_from_offset.hpp>
#include <ndmath/array/flat_iterator.hpp>
#include <ndmath/array/dense_storage.hpp>

struct dummy_array
{
	using size_type       = int;
	using reference       = int;
	using const_reference = int;
	static constexpr auto is_view = true;

	auto extents() const noexcept
	{ return nd::sc_range<0>; }

	auto storage_order() const noexcept
	{ return nd::sc_index<0>; }

	template <class... Ts>
	auto at(const Ts... ts) const noexcept
	{ return 0; }
};

module("test dummy_array")
{
	using traits = nd::array_traits<dummy_array>;
	static_assert(traits::is_view, "");
	static_assert(!traits::is_safe_resizable, "");
	static_assert(!traits::is_unsafe_resizable, "");
	static_assert(!traits::supports_direct_view, "");
	static_assert(!traits::supports_fast_flat_view, "");
	static_assert(!traits::provides_memory_size, "");
}

module("test dense_storage")
{
	namespace mpl = cc::mpl;
	using extents       = std::decay_t<decltype(nd::sc_range<100, 100>)>;
	using storage_order = std::decay_t<decltype(nd::default_storage_order<2>)>;
	using allocator     = mpl::quote<std::allocator>;
	using array_type    = nd::dense_storage<float, extents, storage_order, allocator>;
	using traits        = nd::array_traits<array_type>;

	static_assert(!traits::is_view, "");
	static_assert(!traits::is_safe_resizable, "");
	static_assert(traits::is_unsafe_resizable, "");
	static_assert(traits::supports_direct_view, "");
	static_assert(traits::supports_fast_flat_view, "");
	static_assert(traits::provides_memory_size, "");
}

suite("array traits test")
