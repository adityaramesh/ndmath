/*
** File Name: array_literal_test.cpp
** Author:    Aditya Ramesh
** Date:      08/06/2015
** Contact:   _@adityaramesh.com
*/

#include <ccbase/unit_test.hpp>
#include <ndmath/array/dense_storage.hpp>
#include <ndmath/array/array_literal.hpp>
#include <boost/preprocessor/variadic/size.hpp>

module("test implicit type deduction")
{
	auto a1 = nd_array([[1. 0 0] [0 1 0] [0 0 1]]);
	auto a2 = nd_array([true false true]);
	auto a3 = nd_array([0 1 2 3]);
	auto a4 = nd_array([-1 0 1 2 3]);
	auto a5 = nd_array([t f t]);

	using t1 = decltype(a1)::external_type;
	using t2 = decltype(a2)::external_type;
	using t3 = decltype(a3)::external_type;
	using t4 = decltype(a4)::external_type;
	using t5 = decltype(a5)::external_type;

	static_assert(std::is_same<t1, float>::value, "");
	static_assert(std::is_same<t2, bool>::value, "");
	static_assert(std::is_same<t3, unsigned>::value, "");
	static_assert(std::is_same<t4, int>::value, "");
	static_assert(std::is_same<t5, bool>::value, "");
}

module("test explicit type deduction")
{
	auto a1 = nd_array(unsigned, [-1 0 1 2 3]);
	auto a2 = nd_array(double, [-1 0 1 2 3]);
	auto a3 = nd_array(int, [-1 0 1 2 3]);

	using t1 = decltype(a1)::external_type;
	using t2 = decltype(a2)::external_type;
	using t3 = decltype(a3)::external_type;

	static_assert(std::is_same<t1, unsigned>::value, "");
	static_assert(std::is_same<t2, double>::value, "");
	static_assert(std::is_same<t3, int>::value, "");
}

suite("array literal test")
