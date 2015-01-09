/*
** File Name: index_test.cpp
** Author:    Aditya Ramesh
** Date:      01/08/2015
** Contact:   _@adityaramesh.com
*/

#include <ccbase/unit_test.hpp>
#include <ndmath/index/constant_index.hpp>
#include <ndmath/index/index.hpp>

module("test constant index")
{
	constexpr auto i1 = nd::cindex<1, 2, 3>;

	static_assert(i1.dims() == 3, "");
	static_assert(i1(0) == 1, "");
	static_assert(i1(1) == 2, "");
	static_assert(i1(2) == 3, "");
	static_assert(i1.first() == 1, "");
	static_assert(i1.last() == 3, "");
	static_assert(i1 == i1, "");
	static_assert(!(i1 != i1), "");
	static_assert(off(i1) == 6, "");
}

module("test index")
{
	auto i1 = nd::index<3>{1, 2, 3};

	static_assert(i1.dims() == 3, "");
	assert(i1(0) == 1);
	assert(i1(1) == 2);
	assert(i1(2) == 3);
	assert(i1.first() == 1);
	assert(i1.last() == 3);
	assert(i1 == i1);
	assert(!(i1 != i1));
	assert(off(i1) == 6);
}

suite("index test")
