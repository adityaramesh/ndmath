/*
** File Name: location_test.cpp
** Author:    Aditya Ramesh
** Date:      01/08/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z4C7D5762_9A12_4928_BDBC_2F58097EF579
#define Z4C7D5762_9A12_4928_BDBC_2F58097EF579

#include <ccbase/unit_test.hpp>
#include <ndmath/location/arithmetic.hpp>
#include <ndmath/location/end_location.hpp>

module("test constexpr arithmetic")
{
	using namespace nd::tokens;

	constexpr auto l1 = c<10>;
	constexpr auto l2 = end - 1;
	constexpr auto l3 = 2 * (end - 1) / 2;
	
	static_assert(l1(10) == 10, "");
	static_assert(l2(10) == 9, "");
	static_assert(l3(10) == 9, "");
}

suite("location test")

#endif
