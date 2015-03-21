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

	// 1. Extents only:
	auto a1 = nd::make_darray<float>(20, 20);
	auto a2 = nd::make_darray<float>(c<20>, c<20>);

	// 2. Extents with allocator and/or storage order:
	auto alloc = std::allocator<nd::underlying_type<float>>{};
	auto order = nd::default_storage_order<2>;
	auto a3 = nd::make_darray<float>(nd::extents(20, 20), alloc, order);
	auto a4 = nd::make_darray<float>(nd::cextents<20, 20>, alloc, order);
}

suite("dense storage test")
