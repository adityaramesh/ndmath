/*
** File Name: dense_storage_test.cpp
** Author:    Aditya Ramesh
** Date:      03/20/2015
** Contact:   _@adityaramesh.com
*/

#include <ccbase/unit_test.hpp>
#include <ndmath/array/dense_storage.hpp>

module("test construction")
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
	require(arr.flat_view()[0] == 1);
	require(arr.flat_view()[1] == 2);
	require(arr.flat_view()[2] == 3);
	require(arr.flat_view()[3] == 4);
}

module("test bool indexing")
{
	using namespace nd::tokens;
	auto arr = nd::make_darray<bool>(c<2>, c<2>);
	arr(0, 0) = true;
	arr(0, 1) = true;
	arr(1, 0) = true;
	arr(1, 1) = true;

	require(arr.memory_size() == sizeof(unsigned));
	require(arr.flat_view()[0]);
	require(arr.flat_view()[1]);
	require(arr.flat_view()[2]);
	require(arr.flat_view()[3]);
}

suite("dense storage test")
