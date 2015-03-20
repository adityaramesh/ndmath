/*
** File Name: coord_test.cpp
** Author:    Aditya Ramesh
** Date:      01/08/2015
** Contact:   _@adityaramesh.com
*/

#include <ccbase/unit_test.hpp>
#include <ndmath/coord.hpp>

module("test static constexpr arithmetic")
{
	using namespace nd::tokens;

	constexpr auto c1 = c<10>;
	constexpr auto c2 = end - c<1>;
	constexpr auto c3 = c<2> * (end - c<1>) / c<2>;
	
	static_assert(c1.value() == 10, "");
	static_assert(c2.value(10) == 9, "");
	static_assert(c3.value(10) == 9, "");
}

module("test non-static constexpr arithmetic")
{
	using namespace nd::tokens;

	constexpr auto c1 = nd::make_c_coord(10);
	constexpr auto c2 = end - 1;
	constexpr auto c3 = 2 * (end - 1) / 2;
	
	static_assert(c1.value() == 10, "");
	static_assert(c2.value(10) == 9, "");
	static_assert(c3.value(10) == 9, "");
}

module("test runtime arithmetic")
{
	using namespace nd::tokens;

	auto n = 10;
	auto c1 = nd::make_coord(10);
	auto c2 = end - 1;
	auto c3 = 2 * (end - 1) / 2;
	
	require(c1.value() == 10);
	require(c2.value(n) == 9);
	require(c3.value(n) == 9);
}

suite("location test")
