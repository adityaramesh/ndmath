/*
** File Name: location_test.cpp
** Author:    Aditya Ramesh
** Date:      01/08/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z4C7D5762_9A12_4928_BDBC_2F58097EF579
#define Z4C7D5762_9A12_4928_BDBC_2F58097EF579

#include <ccbase/unit_test.hpp>
#include <ndmath/location.hpp>

module("test static constexpr arithmetic")
{
	using namespace nd::tokens;

	constexpr auto l1 = c<10>;
	constexpr auto l2 = end - c<1>;
	constexpr auto l3 = c<2> * (end - c<1>) / c<2>;
	
	static_assert(l1(10) == 10, "");
	static_assert(l2(10) == 9, "");
	static_assert(l3(10) == 9, "");
}

module("test non-static constexpr arithmetic")
{
	using namespace nd::tokens;

	constexpr auto l1 = nd::make_location(10);
	constexpr auto l2 = end - 1;
	constexpr auto l3 = 2 * (end - 1) / 2;
	
	static_assert(l1(10) == 10, "");
	static_assert(l2(10) == 9, "");
	static_assert(l3(10) == 9, "");
}

module("test runtime arithmetic")
{
	using namespace nd::tokens;

	auto n = 10;
	auto l1 = nd::make_location(10);
	auto l2 = end - 1;
	auto l3 = 2 * (end - 1) / 2;
	
	require(l1(n) == 10);
	require(l2(n) == 9);
	require(l3(n) == 9);
}

suite("location test")

#endif
