/*
** File Name: index_test.cpp
** Author:    Aditya Ramesh
** Date:      01/08/2015
** Contact:   _@adityaramesh.com
*/

#include <ccbase/unit_test.hpp>
#include <ndmath/index/constant_index.hpp>

module("test constant index")
{
	constexpr auto i1 = nd::index<1, 2, 3>;

	static_assert(i1.dims() == 3, "");
	static_assert(i1(0) == 1, "");
	static_assert(i1(1) == 2, "");
	static_assert(i1(2) == 3, "");
	static_assert(i1.first() == 1, "");
	static_assert(i1.last() == 3, "");
	static_assert(i1 == i1, "");
	static_assert(!(i1 != i1), "");
}

suite("index test")
