/*
** File Name: elemwise_view_test.cpp
** Author:    Aditya Ramesh
** Date:      08/15/2015
** Contact:   _@adityaramesh.com
*/

#include <ccbase/unit_test.hpp>
#include <ndmath/array/dense_storage.hpp>
#include <ndmath/array/array_literal.hpp>

module("test scalar relational ops")
{
	using namespace nd::tokens;

	auto x1 = nd_array([1 0; 0 1]);
	auto y1 = nd_array([2 0; 0 2]);
	auto z1 = nd_array(float, [2 0; 0 2]);

	require(x1 == x1);
	require(x1 != y1);
	require(y1 == z1);

	using t1 = nd::detail::relational_operation_traits<
		decltype(x1), decltype(x1), nd::detail::equal_to>;
	using t2 = nd::detail::relational_operation_traits<
		decltype(x1), decltype(y1), nd::detail::equal_to>;
	using t3 = nd::detail::relational_operation_traits<
		decltype(y1), decltype(z1), nd::detail::equal_to>;

	static_assert(t1::can_use_underlying_view, "");
	static_assert(t2::can_use_underlying_view, "");
	static_assert(t3::can_use_underlying_view, "");

	auto x2 = nd_array([t f; f t]);
	auto y2 = nd_array([f t; t f]);

	require(x2 == x2);
	require(x2 != y2);

	using u1 = nd::detail::relational_operation_traits<
		decltype(x2), decltype(x2), nd::detail::equal_to>;
	using u2 = nd::detail::relational_operation_traits<
		decltype(x2), decltype(y2), nd::detail::equal_to>;

	static_assert(!u1::can_use_underlying_view, "");
	static_assert(!u2::can_use_underlying_view, "");
	static_assert(u1::can_use_flat_view, "");
	static_assert(u2::can_use_flat_view, "");

	require(x2 <nd::fast_eq> x2);
	require(x2 <nd::fast_neq> y2);

	using v1 = nd::detail::relational_operation_traits<
		decltype(x2), decltype(x2), nd::detail::fast_eq>;
	using v2 = nd::detail::relational_operation_traits<
		decltype(x2), decltype(y2), nd::detail::fast_neq>;

	static_assert(v1::can_use_underlying_view, "");
	static_assert(v2::can_use_underlying_view, "");
}

module("test elemwise relational ops")
{
	using namespace nd::tokens;

	auto x1 = nd_array([1 0; 0 1]);
	auto y1 = nd_array([2 0; 0 2]);
	auto z1 = nd_array(float, [2 0; 0 2]);

	require((x1() == x1()) == nd_array([t t; t t]));
	require((x1() != y1()) == nd_array([t f; f t]));
	require((y1() == z1()) == nd_array([t t; t t]));

	using b = decltype(nd::make_sarray<bool>(2_c, 2_c));

	using t1 = nd::detail::move_assignment_traits<decltype(x1() == x1()), b>;
	using t2 = nd::detail::move_assignment_traits<decltype(x1() != y1()), b>;
	using t3 = nd::detail::move_assignment_traits<decltype(y1() == z1()), b>;

	static_assert(t1::can_use_underlying_view, "");
	static_assert(t2::can_use_underlying_view, "");
	static_assert(t3::can_use_underlying_view, "");

	auto x2 = nd_array([t f; f t]);
	auto y2 = nd_array([f t; t f]);

	require((x2() == x2()) == nd_array([t t; t t]));
	require((x2() != y2()) == nd_array([t t; t t]));

	using u1 = nd::detail::move_assignment_traits<decltype(x2() == x2()), b>;
	using u2 = nd::detail::move_assignment_traits<decltype(x2() != y2()), b>;

	static_assert(!u1::can_use_underlying_view, "");
	static_assert(!u2::can_use_underlying_view, "");
	static_assert(u1::can_use_flat_view, "");
	static_assert(u2::can_use_flat_view, "");

	require((x2() <nd::fast_eq> x2()) == nd_array([t t; t t]));
	require((x2() <nd::fast_neq> y2()) == nd_array([t t; t t]));

	using v1 = nd::detail::move_assignment_traits<decltype(x2() <nd::fast_eq> x2()), b>;
	using v2 = nd::detail::move_assignment_traits<decltype(x2() <nd::fast_neq> y2()), b>;

	static_assert(v1::can_use_underlying_view, "");
	static_assert(v2::can_use_underlying_view, "");
}

module("test arithmetic ops")
{
	//using namespace nd::tokens;

	//auto x = nd_array([1 2; 3 4]);
	//auto y = nd_array([0 2; 3 0]);

	//require(x + y == nd_array([1 4; 6 4]));

	//auto f = [](auto x, auto y) { return x + y; };
	//auto a = nd_array([1 2; 3 4]);
	//auto b = nd_array(float, [1 2; 3 4]);
	//auto e = nd::make_elemwise_view(a, b, f);

	//require(e.dims() == 2);
	//require(e.extents() == nd::extents(2_c, 2_c));
	//require(e == nd_array([2 4; 6 8]));
	//require(nd_array([1 2]) + nd_array([3 4]) == nd_array([4 6]));

	//nd::zip(a, b)(0, 0) = nd::make_tuple(10, -2);
	//require(a(0, 0) == 10);
	//require(b(0, 0) == -2);

	//require(!nd_array([t f; f t]) == nd_array([f t; t f]));
	//auto z = !nd_array([t f; f t]);
	//cc::println("$ $; $ $", z(0, 0), z(0, 1), z(1, 0), z(1, 1));
	//for (const auto& x : z.flat_view()) {
	//	cc::println("${bin}", x);
	//}

	//auto x = nd_array([t f; f t]);
	//auto y = nd_array([t f; f t]);
	//x = x() <nd::fast_eq> y();
	//x = x() <nd::fast_eq> y();
	//x() == y();
	//auto v = x <nd::fast_eq> y;

	//auto a = nd::make_sarray(x <nd::fast_and> y);
	//cc::println("$ $; $ $", a(0, 0), a(0, 1), a(1, 0), a(1, 1));
}

module("test logical ops")
{
	// test normal and "fast" variants
	// use assertions on the 
}

module("test elementwise comparison")
{
	//
}

suite("elemwise view test")
