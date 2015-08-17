/*
** File Name: elemwise_expr_test.cpp
** Author:    Aditya Ramesh
** Date:      08/15/2015
** Contact:   _@adityaramesh.com
*/

#include <ccbase/unit_test.hpp>
#include <ndmath/array/dense_storage.hpp>
#include <ndmath/array/array_literal.hpp>
#include <ndmath/array/elemwise_expr.hpp>

module("test construction")
{
	using namespace nd::tokens;

	auto f = [](auto x, auto y) { return x + y; };
	auto a = nd_array([1 2; 3 4]);
	auto e = nd::make_elemwise_expr(a, a, f);

	require(e.dims() == 2);
	require(e.extents() == nd::extents(2_c, 2_c));
	require(e == nd_array([2 4; 6 8]));
}

suite("elemwise expr test")
