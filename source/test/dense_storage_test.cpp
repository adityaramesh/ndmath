/*
** File Name: dense_storage_test.cpp
** Author:    Aditya Ramesh
** Date:      03/20/2015
** Contact:   _@adityaramesh.com
*/

#include <ccbase/unit_test.hpp>
#include <ndmath/array/dense_storage.hpp>
#include <ndmath/array/array_literal.hpp>

module("test dynamic construction")
{
	using namespace nd::tokens;

	// Extents only:
	auto a1 = nd::make_darray<float>(20, 20);
	auto a2 = nd::make_darray<float>(20_c, 20_c);

	// Extents with allocator and/or storage order:
	auto alloc = std::allocator<nd::underlying_type<float>>{};
	auto order = nd::default_storage_order<2>;
	auto a3 = nd::make_darray<float>(nd::extents(20, 20), alloc, order);
	auto a4 = nd::make_darray<float>(nd::extents(20_c, 20_c), alloc, order);

	// Extents with initial value:
	auto a5 = nd::make_darray<float>(0, nd::extents(20_c, 20_c));
	auto a6 = nd::make_darray<float>(0, nd::extents(20, 20), alloc, order);

	// Initializer list with extents:
	auto a7 = nd::make_darray<float>({{1, 2}, {3, 4}},
		nd::extents(2_c, 2_c));
	auto a8 = nd::make_darray<bool>({{true, false}, {false, true}},
		nd::extents(2_c, 2_c));
}

module("test static construction")
{
	using namespace nd::tokens;

	// Extents only:
	auto a1 = nd::make_sarray<float>(20_c, 20_c);

	// Extents with storage order:
	auto order = nd::default_storage_order<2>;
	auto a2 = nd::make_sarray<float>(nd::extents(20_c, 20_c), order);

	// Extents with initial value:
	auto a3 = nd::make_sarray<float>(0, nd::extents(20_c, 20_c));

	// Initializer list with extents:
	auto a4 = nd::make_sarray<float>({{1, 2}, {3, 4}},
		nd::extents(2_c, 2_c));
	auto a5 = nd::make_sarray<bool>({{true, false}, {false, true}},
		nd::extents(2_c, 2_c));

	// Array literal syntax with implicit type:
	auto a6 = nd_array([1 2; 3 4]);
	auto a7 = nd_array([t f; f t]);

	// Array literal syntax with explicit type:
	auto a9 = nd_array(float, [1 2; 3 4]);
}

module("test accessors")
{
	using namespace nd::tokens;
	auto arr = nd::make_darray<float>(20_c, 20_c);

	static_assert(arr.dims() == 2, "");
	require(arr.size() == 400);
}

module("test regular indexing")
{
	using namespace nd::tokens;

	auto arr = nd_darray(float, [1 2; 3 4]);

	require(arr.memory_size() == 4 * sizeof(float));
	require(arr == nd_array([1 2; 3 4]));
}

module("test bool indexing")
{
	using namespace nd::tokens;

	auto arr = nd_darray([t f; f t]);
	require(arr == nd_array([t f; f t]));

	require(arr.memory_size() == sizeof(unsigned));
	require(arr == nd_array([t f; f t]));
}

module("test copy assignment dynamic dynamic")
{
	using namespace nd::tokens;

	auto a = nd_darray(float, [1 2; 3 4]);

	auto b = nd::make_darray<float>(2_c, 2_c);
	b = a;

	require(b == nd_array([1 2; 3 4]));

	auto c = nd_darray([t f; f t]);

	auto d = nd::make_darray<bool>(2_c, 2_c);
	d = c;

	require(d == nd_array([t f; f t]));
}

module("test copy assignment mixed")
{
	using namespace nd::tokens;

	auto a = nd_array(float, [1 2; 3 4]);

	auto b = nd::make_darray<float>(2_c, 2_c);
	b = a;

	require(b == nd_array([1 2; 3 4]));

	auto c = nd_darray(float, [1 2; 3 4]);

	auto d = nd::make_sarray<float>(2_c, 2_c);
	d = c;

	require(d == nd_array([1 2; 3 4]));
}

module("test move assignment dynamic dynamic")
{
	using namespace nd::tokens;

	auto a = nd_darray(float, [1 2; 3 4]);

	auto b = nd::make_darray<float>(2_c, 2_c);
	b = std::move(a);

	require(a.underlying_view().begin() == nullptr);
	require(b == nd_array([1 2; 3 4]));

	auto c = nd_darray([t f; f t]);

	auto d = nd::make_darray<bool>(2_c, 2_c);
	d = std::move(c);

	require(c.underlying_view().begin() == nullptr);
	require(d == nd_array([t f; f t]));
}

module("test copy construction dynamic dynamic")
{
	using namespace nd::tokens;

	auto a = nd_darray(float, [1 2; 3 4]);

	auto b = nd::make_darray(a);
	require(b == nd_array([1 2; 3 4]));

	auto c = a;
	require(c == nd_array([1 2; 3 4]));

	auto d = nd_darray([t f; f t]);

	auto e = nd::make_darray(d);
	require(e == nd_array([t f; f t]));

	auto f = d;
	require(f == nd_array([t f; f t]));
}

module("test copy construction static static")
{
	using namespace nd::tokens;

	auto a = nd_array(float, [1 2; 3 4]);

	auto b = nd::make_sarray(a);
	require(b == nd_array([1 2; 3 4]));

	auto c = a;
	require(c == nd_array([1 2; 3 4]));

	auto d = nd_array([t f; f t]);

	auto e = nd::make_sarray(d);
	require(e == nd_array([t f; f t]));

	auto f = d;
	require(f == nd_array([t f; f t]));
}

module("test copy construction mixed")
{
	using namespace nd::tokens;

	auto a = nd_array(float, [1 2; 3 4]);

	auto b = nd::make_darray(a);
	require(b == nd_array([1 2; 3 4]));

	auto c = nd_darray(float, [1 2; 3 4]);

	auto d = nd::make_sarray(c);
	require(d == nd_array([1 2; 3 4]));

	decltype(b) e = a;
	require(e == nd_array([1 2; 3 4]));

	decltype(a) f = b;
	require(f == nd_array([1 2; 3 4]));
}

module("test move construction dynamic dynamic")
{
	using namespace nd::tokens;

	auto a = nd_darray(float, [1 2; 3 4]);

	auto b = nd::make_darray(std::move(a));
	require(a.underlying_view().begin() == nullptr);
	require(b == nd_array([1 2; 3 4]));

	auto c = std::move(b);
	require(b.underlying_view().begin() == nullptr);
	require(c == nd_array([1 2; 3 4]));
	
	auto d = nd_darray([t f; f t]);

	auto e = nd::make_darray(std::move(d));
	require(d.underlying_view().begin() == nullptr);
	require(e == nd_array([t f; f t]));

	auto f = std::move(e);
	require(e.underlying_view().begin() == nullptr);
	require(f == nd_array([t f; f t]));
}

module("test move construction static static")
{
	using namespace nd::tokens;

	auto a = nd_array(float, [1 2; 3 4]);

	auto b = nd::make_sarray(std::move(a));
	require(b == nd_array([1 2; 3 4]));

	auto c = std::move(b);
	require(c == nd_array([1 2; 3 4]));
	
	auto d = nd_darray([t f; f t]);

	auto e = nd::make_darray(std::move(d));
	require(e == nd_array([t f; f t]));

	auto f = std::move(e);
	require(f == nd_array([t f; f t]));
}

// TODO test move construction mixed

suite("dense storage test")
