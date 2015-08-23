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

module("test unary ops")
{
	using namespace nd::tokens;

	auto x1 = nd_array([1 0; 0 1]);

	require(+x1 == x1);
	// TODO cast<> function.
	require(-x1 == nd_array([-1 0; 0 -1]));
	require(~~x1 == x1);

	using t1 = nd::detail::move_assignment_traits<decltype(+x1), decltype(x1)>;
	using t2 = nd::detail::move_assignment_traits<decltype(-x1), decltype(x1)>;
	using t3 = nd::detail::move_assignment_traits<decltype(~x1), decltype(x1)>;

	static_assert(t1::can_use_underlying_view, "");
	static_assert(t2::can_use_underlying_view, "");
	static_assert(t3::can_use_underlying_view, "");

	auto x2 = nd_array([t f; f t]);

	require(!x2 == nd_array([f t; t f]));
	require(nd::fast_not(x2) == nd_array([f t; t f]));

	using u1 = nd::detail::move_assignment_traits<decltype(!x2), decltype(x2)>;
	using u2 = nd::detail::move_assignment_traits<decltype(nd::fast_not(x2)), decltype(x2)>;

	static_assert(!u1::can_use_underlying_view, "");
	static_assert(u2::can_use_underlying_view, "");
	static_assert(u1::can_use_flat_view, "");
}

module("test binary ops")
{
	using namespace nd::tokens;

	auto x1 = nd_array([1 0; 0 1]);
	auto y1 = nd_array(float, [2 0; 0 2]);

	require((x1() + x1()) == nd_array([2 0; 0 2]));
	require((x1() + y1()) == nd_array([3 0; 0 3]));

	using t1 = nd::detail::move_assignment_traits<decltype(x1 + x1), decltype(x1)>;
	using t2 = nd::detail::move_assignment_traits<decltype(x1 + y1), decltype(x1)>;

	static_assert(t1::can_use_underlying_view, "");
	static_assert(t2::can_use_underlying_view, "");

	auto x2 = nd_array([t f; f t]);
	auto y2 = nd_array([f t; t f]);

	require((x2 && x2) == nd_array([t f; f t]));
	require((x2 && y2) == nd_array([f f; f f]));

	using u1 = nd::detail::move_assignment_traits<decltype(x2 && x2), decltype(x2)>;
	using u2 = nd::detail::move_assignment_traits<decltype(x2 && y2), decltype(x2)>;

	static_assert(!u1::can_use_underlying_view, "");
	static_assert(!u2::can_use_underlying_view, "");
	static_assert(u1::can_use_flat_view, "");
	static_assert(u2::can_use_flat_view, "");

	require((x2() <nd::fast_and> x2()) == nd_array([t f; f t]));
	require((x2() <nd::fast_and> y2()) == nd_array([f f; f f]));

	using v1 = nd::detail::move_assignment_traits<
		decltype(x2() <nd::fast_eq> x2()), decltype(x2)>;
	using v2 = nd::detail::move_assignment_traits<
		decltype(x2() <nd::fast_neq> y2()), decltype(x2)>;

	static_assert(v1::can_use_underlying_view, "");
	static_assert(v2::can_use_underlying_view, "");
}

suite("elemwise view test")
