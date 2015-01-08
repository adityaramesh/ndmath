/*
** File Name: location_test.cpp
** Author:    Aditya Ramesh
** Date:      01/08/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z4C7D5762_9A12_4928_BDBC_2F58097EF579
#define Z4C7D5762_9A12_4928_BDBC_2F58097EF579

#include <ccbase/unit_test.hpp>
#include <ndmath/location/constant_location.hpp>
#include <ndmath/location/binary_location_expr.hpp>
#include <ndmath/location/end_location.hpp>

module("test construction")
{
	using namespace nd::tokens;

	auto l1 = c<123>;
	auto l2 = (c<2> * (c<100> + c<123>)) / c<2>;
	auto l3 = c<2> * (end - c<1>) / c<2>;

	static_assert(l1(0) == 123, "");
	static_assert(l2(0) == 223, "");
	static_assert(l3(10) == 9, "");
}

suite("location test")

#endif
