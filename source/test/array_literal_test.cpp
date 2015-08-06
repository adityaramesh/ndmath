/*
** File Name: array_literal_test.cpp
** Author:    Aditya Ramesh
** Date:      08/06/2015
** Contact:   _@adityaramesh.com
*/

#include <ccbase/unit_test.hpp>
#include <ndmath/array/dense_storage.hpp>
#include <ndmath/array/array_literal.hpp>

module("test implicit type deduction")
{
	auto a1 = nd_array([[1. 0 0] [0 1 0] [0 0 1]]);
	auto a2 = nd_array([true false true]);
	auto a3 = nd_array([0 1 2 3]);
	auto a4 = nd_array([-1 0 1 2 3]);

	using t1 = decltype(a1)::exterior_type;
	using t2 = decltype(a2)::exterior_type;
	using t3 = decltype(a3)::exterior_type;
	using t4 = decltype(a4)::exterior_type;

	static_assert(std::is_same<t1, float>::value, "");
	static_assert(std::is_same<t2, bool>::value, "");
	static_assert(std::is_same<t3, unsigned>::value, "");
	static_assert(std::is_same<t4, int>::value, "");
}

module("test explicit type deduction")
{
	//
}

suite("array literal test")
