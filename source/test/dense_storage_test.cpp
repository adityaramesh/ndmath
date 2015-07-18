/*
** File Name: dense_storage_test.cpp
** Author:    Aditya Ramesh
** Date:      03/20/2015
** Contact:   _@adityaramesh.com
*/

#include <ccbase/unit_test.hpp>
#include <ndmath/array/dense_storage.hpp>

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

	auto arr = nd::make_darray<float>(2_c, 2_c);
	arr(0, 0) = 1;
	arr(0, 1) = 2;
	arr(1, 0) = 3;
	arr(1, 1) = 4;

	require(arr.memory_size() == 4 * sizeof(float));
	require(arr(0, 0) == 1);
	require(arr(0, 1) == 2);
	require(arr(1, 0) == 3);
	require(arr(1, 1) == 4);
}

module("test bool indexing")
{
	using namespace nd::tokens;

	auto arr = nd::make_darray<bool>(2_c, 2_c);
	arr(0, 0) = true;
	arr(0, 1) = false;
	arr(1, 0) = true;
	arr(1, 1) = false;

	require(arr.memory_size() == sizeof(unsigned));
	require(arr(0, 0));
	require(!arr(0, 1));
	require(arr(1, 0));
	require(!arr(1, 1));
}

module("test copy assignment dynamic dynamic")
{
	using namespace nd::tokens;

	auto a = nd::make_darray<float>(20_c, 20_c);
	a(0, 0) = 1;
	a(0, 1) = 2;
	a(1, 0) = 3;
	a(1, 1) = 4;

	auto b = nd::make_darray<float>(20_c, 20_c);
	b = a;
	require(b(0, 0) == 1);
	require(b(0, 1) == 2);
	require(b(1, 0) == 3);
	require(b(1, 1) == 4);

	auto c = nd::make_darray<bool>(20_c, 20_c);
	c(0, 0) = true;
	c(0, 1) = false;
	c(1, 0) = true;
	c(1, 1) = false;

	auto d = nd::make_darray<bool>(20_c, 20_c);
	d = c;
	require(d(0, 0));
	require(!d(0, 1));
	require(d(1, 0));
	require(!d(1, 1));
}

module("test copy assignment mixed")
{
	using namespace nd::tokens;

	auto a = nd::make_sarray<float>(20_c, 20_c);
	a(0, 0) = 1;
	a(0, 1) = 2;
	a(1, 0) = 3;
	a(1, 1) = 4;

	auto b = nd::make_darray<double>(20_c, 20_c);
	b = a;
	require(b(0, 0) == 1);
	require(b(0, 1) == 2);
	require(b(1, 0) == 3);
	require(b(1, 1) == 4);

	auto c = nd::make_darray<float>(20_c, 20_c);
	c(0, 0) = 1;
	c(0, 1) = 2;
	c(1, 0) = 3;
	c(1, 1) = 4;

	auto d = nd::make_sarray<double>(20_c, 20_c);
	d = c;
	require(d(0, 0) == 1);
	require(d(0, 1) == 2);
	require(d(1, 0) == 3);
	require(d(1, 1) == 4);
}

module("test move assignment dynamic dynamic")
{
	using namespace nd::tokens;

	auto a = nd::make_darray<float>(20_c, 20_c);
	a(0, 0) = 1;
	a(0, 1) = 2;
	a(1, 0) = 3;
	a(1, 1) = 4;

	auto b = nd::make_darray<float>(20_c, 20_c);
	b = std::move(a);
	require(a.direct_view().begin() == nullptr);
	require(b(0, 0) == 1);
	require(b(0, 1) == 2);
	require(b(1, 0) == 3);
	require(b(1, 1) == 4);

	auto c = nd::make_darray<bool>(20_c, 20_c);
	c(0, 0) = true;
	c(0, 1) = false;
	c(1, 0) = true;
	c(1, 1) = false;

	auto d = nd::make_darray<bool>(20_c, 20_c);
	d = std::move(c);
	require(c.direct_view().begin() == nullptr);
	require(d(0, 0));
	require(!d(0, 1));
	require(d(1, 0));
	require(!d(1, 1));
}

module("test copy construction dynamic dynamic")
{
	using namespace nd::tokens;

	auto a = nd::make_darray<float>(2_c, 2_c);
	a(0, 0) = 1;
	a(0, 0) = 1;
	a(0, 1) = 2;
	a(1, 0) = 3;
	a(1, 1) = 4;

	auto b = nd::make_darray(a);
	require(b(0, 0) == 1);
	require(b(0, 1) == 2);
	require(b(1, 0) == 3);
	require(b(1, 1) == 4);

	auto c = a;
	require(c(0, 0) == 1);
	require(c(0, 1) == 2);
	require(c(1, 0) == 3);
	require(c(1, 1) == 4);

	auto d = nd::make_darray<bool>(2_c, 2_c);
	d(0, 0) = true;
	d(0, 1) = false;
	d(1, 0) = true;
	d(1, 1) = false;

	auto e = nd::make_darray(d);
	require(e(0, 0));
	require(!e(0, 1));
	require(e(1, 0));
	require(!e(1, 1));

	auto f = d;
	require(f(0, 0));
	require(!f(0, 1));
	require(f(1, 0));
	require(!f(1, 1));
}

module("test copy construction static static")
{
	using namespace nd::tokens;

	auto a = nd::make_sarray<float>(2_c, 2_c);
	a(0, 0) = 1;
	a(0, 1) = 2;
	a(1, 0) = 3;
	a(1, 1) = 4;

	auto b = nd::make_sarray(a);
	require(b(0, 0) == 1);
	require(b(0, 1) == 2);
	require(b(1, 0) == 3);
	require(b(1, 1) == 4);

	auto c = a;
	require(c(0, 0) == 1);
	require(c(0, 1) == 2);
	require(c(1, 0) == 3);
	require(c(1, 1) == 4);

	auto d = nd::make_sarray<bool>(2_c, 2_c);
	d(0, 0) = true;
	d(0, 1) = false;
	d(1, 0) = true;
	d(1, 1) = false;

	auto e = nd::make_sarray(d);
	require(e(0, 0));
	require(!e(0, 1));
	require(e(1, 0));
	require(!e(1, 1));

	auto f = d;
	require(f(0, 0));
	require(!f(0, 1));
	require(f(1, 0));
	require(!f(1, 1));
}

module("test copy construction mixed")
{
	using namespace nd::tokens;

	auto a = nd::make_sarray<float>(2_c, 2_c);
	a(0, 0) = 1;
	a(0, 1) = 2;
	a(1, 0) = 3;
	a(1, 1) = 4;

	auto b = nd::make_darray(a);
	require(b(0, 0) == 1);
	require(b(0, 1) == 2);
	require(b(1, 0) == 3);
	require(b(1, 1) == 4);

	auto c = nd::make_darray<float>(2_c, 2_c);
	c(0, 0) = 1;
	c(0, 1) = 2;
	c(1, 0) = 3;
	c(1, 1) = 4;

	auto d = nd::make_sarray(c);
	require(d(0, 0) == 1);
	require(d(0, 1) == 2);
	require(d(1, 0) == 3);
	require(d(1, 1) == 4);

	decltype(b) e = a;
	require(e(0, 0) == 1);
	require(e(0, 1) == 2);
	require(e(1, 0) == 3);
	require(e(1, 1) == 4);

	decltype(a) f = b;
	require(f(0, 0) == 1);
	require(f(0, 1) == 2);
	require(f(1, 0) == 3);
	require(f(1, 1) == 4);
}

module("test move construction dynamic dynamic")
{
	using namespace nd::tokens;

	auto a = nd::make_darray<float>(2_c, 2_c);
	a(0, 0) = 1;
	a(0, 1) = 2;
	a(1, 0) = 3;
	a(1, 1) = 4;

	auto b = nd::make_darray(std::move(a));
	require(a.direct_view().begin() == nullptr);
	require(b(0, 0) == 1);
	require(b(0, 1) == 2);
	require(b(1, 0) == 3);
	require(b(1, 1) == 4);

	auto c = std::move(b);
	require(b.direct_view().begin() == nullptr);
	require(c(0, 0) == 1);
	require(c(0, 1) == 2);
	require(c(1, 0) == 3);
	require(c(1, 1) == 4);
	
	auto d = nd::make_darray<bool>(2_c, 2_c);
	d(0, 0) = true;
	d(0, 1) = false;
	d(1, 0) = true;
	d(1, 1) = false;

	auto e = nd::make_darray(std::move(d));
	require(d.direct_view().begin() == nullptr);
	require(e(0, 0));
	require(!e(0, 1));
	require(e(1, 0));
	require(!e(1, 1));

	auto f = std::move(e);
	require(e.direct_view().begin() == nullptr);
	require(f(0, 0));
	require(!f(0, 1));
	require(f(1, 0));
	require(!f(1, 1));
}

module("test move construction static static")
{
	using namespace nd::tokens;

	auto a = nd::make_sarray<float>(2_c, 2_c);
	a(0, 0) = 1;
	a(0, 1) = 2;
	a(1, 0) = 3;
	a(1, 1) = 4;

	auto b = nd::make_sarray(std::move(a));
	require(b(0, 0) == 1);
	require(b(0, 1) == 2);
	require(b(1, 0) == 3);
	require(b(1, 1) == 4);

	auto c = std::move(b);
	require(c(0, 0) == 1);
	require(c(0, 1) == 2);
	require(c(1, 0) == 3);
	require(c(1, 1) == 4);
	
	auto d = nd::make_darray<bool>(2_c, 2_c);
	d(0, 0) = true;
	d(0, 1) = false;
	d(1, 0) = true;
	d(1, 1) = false;

	auto e = nd::make_darray(std::move(d));
	require(e(0, 0));
	require(!e(0, 1));
	require(e(1, 0));
	require(!e(1, 1));

	auto f = std::move(e);
	require(f(0, 0));
	require(!f(0, 1));
	require(f(1, 0));
	require(!f(1, 1));
}

suite("dense storage test")
