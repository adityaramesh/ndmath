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
	auto a2 = nd::make_darray<float>(c<20>, c<20>);

	// Extents with allocator and/or storage order:
	auto alloc = std::allocator<nd::underlying_type<float>>{};
	auto order = nd::default_storage_order<2>;
	auto a3 = nd::make_darray<float>(nd::extents(20, 20), alloc, order);
	auto a4 = nd::make_darray<float>(nd::cextents<20, 20>, alloc, order);

	// Extents with initial value:
	auto a5 = nd::make_darray<float>(0, nd::cextents<20, 20>);
	auto a6 = nd::make_darray<float>(0, nd::extents(20, 20), alloc, order);
}

module("test static construction")
{
	using namespace nd::tokens;

	// Extents only:
	auto a1 = nd::make_sarray<float>(c<20>, c<20>);

	// Extents with storage order:
	auto order = nd::default_storage_order<2>;
	auto a2 = nd::make_sarray<float>(nd::extents(c<20>, c<20>), order);

	// Extents with initial value:
	auto a3 = nd::make_sarray<float>(0, nd::cextents<20, 20>);
}

module("test accessors")
{
	using namespace nd::tokens;
	auto arr = nd::make_darray<float>(c<20>, c<20>);

	static_assert(arr.dims() == 2, "");
	require(arr.size() == 400);
}

module("test regular indexing")
{
	using namespace nd::tokens;
	auto arr = nd::make_darray<float>(c<2>, c<2>);
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
	auto arr = nd::make_darray<bool>(c<2>, c<2>);
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
	auto a = nd::make_darray<float>(nd::cextents<20, 20>);
	a(0, 0) = 1;
	a(0, 1) = 2;
	a(1, 0) = 3;
	a(1, 1) = 4;

	auto b = nd::make_darray<float>(nd::cextents<20, 20>);
	b = a;
	require(b(0, 0) == 1);
	require(b(0, 1) == 2);
	require(b(1, 0) == 3);
	require(b(1, 1) == 4);

	auto c = nd::make_darray<bool>(nd::cextents<20, 20>);
	c(0, 0) = true;
	c(0, 1) = false;
	c(1, 0) = true;
	c(1, 1) = false;

	auto d = nd::make_darray<bool>(nd::cextents<20, 20>);
	d = c;
	require(d(0, 0));
	require(!d(0, 1));
	require(d(1, 0));
	require(!d(1, 1));
}

module("test move assignment dynamic dynamic")
{
	auto a = nd::make_darray<float>(nd::cextents<20, 20>);
	a(0, 0) = 1;
	a(0, 1) = 2;
	a(1, 0) = 3;
	a(1, 1) = 4;

	auto b = nd::make_darray<float>(nd::cextents<20, 20>);
	b = std::move(a);
	require(a.direct_view().begin() == nullptr);
	require(b(0, 0) == 1);
	require(b(0, 1) == 2);
	require(b(1, 0) == 3);
	require(b(1, 1) == 4);

	auto c = nd::make_darray<bool>(nd::cextents<20, 20>);
	c(0, 0) = true;
	c(0, 1) = false;
	c(1, 0) = true;
	c(1, 1) = false;

	auto d = nd::make_darray<bool>(nd::cextents<20, 20>);
	d = std::move(c);
	require(c.direct_view().begin() == nullptr);
	require(d(0, 0));
	require(!d(0, 1));
	require(d(1, 0));
	require(!d(1, 1));
}

module("test copy construction dynamic dynamic")
{
	auto a = nd::make_darray<float>(nd::cextents<2, 2>);
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

	auto d = nd::make_darray<bool>(nd::cextents<2, 2>);
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

// TODO: copy construction static static
// TODO: copy construction dynamic static

/*
module("test move construction")
{
	auto a = nd::make_darray<float>(nd::cextents<2, 2>);
	a(0, 0) = 1;
	a(0, 1) = 2;
	a(1, 0) = 3;
	a(1, 1) = 4;

	// This won't work with our strategy because b will already be default
	// initialized before assignment to a.
	auto b = std::move(nd::make_darray(a));
	require(a.direct_view().begin() == nullptr);
	require(b(0, 0) == 1);
	require(b(0, 1) == 2);
	require(b(1, 0) == 3);
	require(b(1, 1) == 4);

	//auto c = a;
	//require(c(0, 0) == 1);
	//require(c(0, 1) == 2);
	//require(c(1, 0) == 3);
	//require(c(1, 1) == 4);
}
*/

// TODO copy assignment dynamic static

suite("dense storage test")
