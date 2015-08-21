/*
** File Name: elemwise_view_test.cpp
** Author:    Aditya Ramesh
** Date:      08/15/2015
** Contact:   _@adityaramesh.com
*/

#include <ccbase/unit_test.hpp>
#include <ndmath/array/dense_storage.hpp>
#include <ndmath/array/array_literal.hpp>
#include <ndmath/array/elemwise_view.hpp>

module("test arithmetic ops")
{
	/*
	** TODO: actual tests
	*/

	//using namespace nd::tokens;

	//auto f = [](auto x, auto y) { return x + y; };
	//auto a = nd_array([1 2; 3 4]);
	//auto b = nd_array(float, [1 2; 3 4]);
	//auto e = nd::make_elemwise_view(a, b, f);

	//require(e.dims() == 2);
	//require(e.extents() == nd::extents(2_c, 2_c));
	//require(e == nd_array([2 4; 6 8]));
	//require(nd_array([1 2]) + nd_array([3 4]) == nd_array([4 6]));

	//nd::zip(a, b)(0, 0) = nd::make_tuple(10, -2);
	//require(a(0, 0) == 10);
	//require(b(0, 0) == -2);

	//require(!nd_array([t f; f t]) == nd_array([f t; t f]));
	//auto z = !nd_array([t f; f t]);
	//cc::println("$ $; $ $", z(0, 0), z(0, 1), z(1, 0), z(1, 1));
	//for (const auto& x : z.flat_view()) {
	//	cc::println("${bin}", x);
	//}

	//auto x = nd_array([t f; f t]);
	//auto y = nd_array([t f; f t]);
	//auto z = x <nd::fast_and> y;

	//auto a = nd::make_sarray(x <nd::fast_and> y);
	//cc::println("$ $; $ $", a(0, 0), a(0, 1), a(1, 0), a(1, 1));
}

module("test logical ops")
{
	// test normal and "fast" variants
	// use assertions on the 
}

suite("elemwise view test")
