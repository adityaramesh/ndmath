/*
** File Name: range_test.cpp
** Author:    Aditya Ramesh
** Date:      01/10/2015
** Contact:   _@adityaramesh.com
*/

#include <ccbase/unit_test.hpp>
#include <ndmath/range/range_iterator.hpp>

module("test iteration")
{
	auto f = nd::make_range_iterator(nd::cindex<5, 5, 5>, nd::cindex<1, 1, 1>);
	auto l = nd::make_range_end_token(nd::cindex<5, 5, 5>);

	while (f != l) {
		cc::println(*f);
		++f;
	}

	// crange<args...>: short for make_range with appropriate constant indices
	// range(args...): short for make_range with appropriate index
	//for (const auto& i : nd::crange<50, 50, 50>) {
	//	//
	//}
}

suite("range test")
