/*
** File Name: tuple_test.cpp
** Author:    Aditya Ramesh
** Date:      08/20/2015
** Contact:   _@adityaramesh.com
*/

#include <ccbase/unit_test.hpp>
#include <ndmath/utility/tuple.hpp>

module("test construction")
{
	auto f = nd::make_tuple(1, "abc");
	using t1 = decltype(nd::get<0>(f));
	using t2 = decltype(nd::get<0>(std::move(f)));
	++nd::get<0>(f);
}

suite("tuple test")
