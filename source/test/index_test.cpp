/*
** File Name: index_test.cpp
** Author:    Aditya Ramesh
** Date:      01/08/2015
** Contact:   _@adityaramesh.com
*/

#include <ccbase/unit_test.hpp>
#include <ndmath/index/constant_index.hpp>
#include <ndmath/index/index.hpp>
#include <ndmath/index/subindex.hpp>
#include <ndmath/index/composite_index.hpp>
#include <ndmath/index/binary_index_expr.hpp>

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
	auto i1 = nd::make_index(1, 2, 3);

	static_assert(i1.dims() == 3, "");
	require(i1(0) == 1);
	require(i1(1) == 2);
	require(i1(2) == 3);
	require(i1.first() == 1);
	require(i1.last() == 3);
	require(i1 == i1);
	require(!(i1 != i1));
	require(off(i1) == 6);
}

module("test constexpr subindex")
{
	using namespace nd::tokens;

	constexpr auto i1 = nd::cindex<0, 1, 2, 3, 4>;
	static_assert(i1(c<1>, c<3>).dims() == 3, "");
	static_assert(i1(c<1>, c<3>)(0) == 1, "");
	static_assert(i1(c<1>, c<3>).first() == 1, "");
	static_assert(i1(c<1>, c<3>).last() == 3, "");
}

module("test dynamic subindex")
{
	using namespace nd::tokens;
	auto i1 = nd::make_index(0, 1, 2, 3, 4);

	static_assert(i1(c<1>, c<3>).dims() == 3, "");
	require(i1(c<1>, c<3>)(0) == 1);
	require(i1(c<1>, c<3>).first() == 1);
	require(i1(c<1>, c<3>).last() == 3);

	i1(c<1>, c<3>) = {7, 8, 9};
	assert(i1 == nd::make_index(0, 7, 8, 9, 4));
}

module("test constexpr composite index")
{
	using namespace nd::tokens;
	constexpr auto i1 = nd::cindex<0, 1, 2, 3>;
	constexpr auto i2 = nd::cindex<4, 5>;

	static_assert((i1, i2) == nd::cindex<0, 1, 2, 3, 4, 5>, "");
	static_assert((i1(c<0>, c<1>), i2(c<1>, c<1>)) == nd::cindex<0, 1, 5>, "");
}

module("test dynamic composite index")
{
	using namespace nd::tokens;
	auto i1 = nd::make_index(0, 1, 2, 3);
	auto i2 = nd::make_index(4, 5);

	require((i1, i2) == nd::make_index(0, 1, 2, 3, 4, 5));
	require((i1(c<0>, c<1>), i2(c<1>, c<1>)) == nd::make_index(0, 1, 5));

	(i1, i2)(c<2>, c<4>) = {7, 8, 9};
	require((i1, i2) == nd::make_index(0, 1, 7, 8, 9, 5));
}

module("test constexpr index arithmetic")
{
	constexpr auto i1 = nd::cindex<1, 2, 3>;

	static_assert(i1 + i1 == nd::cindex<2, 4, 6>, "");
	static_assert(i1 - i1 == nd::cindex<0, 0, 0>, "");
	static_assert(i1 * i1 == nd::cindex<1, 4, 9>, "");
	static_assert(i1 / i1 == nd::cindex<1, 1, 1>, "");
	static_assert(i1 * (i1 + i1) / i1 - i1 == i1, "");
}

module("test dynamic index arithmetic")
{
	auto i1 = nd::make_index(1, 2, 3);

	require(i1 + i1 == nd::make_index(2, 4, 6));
	require(i1 - i1 == nd::make_index(0, 0, 0));
	require(i1 * i1 == nd::make_index(1, 4, 9));
	require(i1 / i1 == nd::make_index(1, 1, 1));
	require(i1 * (i1 + i1) / i1 - i1 == i1);
}

suite("index test")
