/*
** File Name: range_test.cpp
** Author:    Aditya Ramesh
** Date:      01/10/2015
** Contact:   _@adityaramesh.com
*/

#include <ccbase/unit_test.hpp>
#include <ndmath/range/range.hpp>
//#include <ndmath/range/range_literal.hpp>
#include <ndmath/common.hpp>
#include <ndmath/range/attribute_traits.hpp>

module("test range construction")
{
	using nd::cindex;
	using nd::make_range;

	static constexpr auto b = cindex<0, 0, 0>;
	static constexpr auto e = cindex<50, 50, 50>;
	static constexpr auto s = cindex<1, 1, 1>;

	constexpr auto r1 = make_range(e);
	constexpr auto r2 = make_range(b, e);
	constexpr auto r3 = make_range(b, e, s);

	static_assert(r1.start() == b, "");
	static_assert(r1.finish() == e, "");
	static_assert(r1.strides() == s, "");
	static_assert(r2.start() == b, "");
	static_assert(r2.finish() == e, "");
	static_assert(r2.strides() == s, "");
	static_assert(r3.start() == b, "");
	static_assert(r3.finish() == e, "");
	static_assert(r3.strides() == s, "");
}

/*
module("test attribute")
{
	using namespace cc::mpl;
	using a = nd::default_attribs<3>;
	using r1 = nd::reverse_loops<list<int_<1>, int_<2>>, a>;
	using r2 = nd::apply_perm<list<int_<1>, int_<2>>, a>;
}

module("test range for_each")
{
	using nd::cindex;
	using nd::make_range;

	auto j = 0;
	constexpr auto r1 = make_range(cindex<50, 50, 50>);
	r1([&] (auto) { ++j; });
	require(j == 50 * 50 * 50);
}

module("test range iterator")
{
	using nd::cindex;
	using nd::make_range;

	auto j = 0;
	constexpr auto r1 = make_range(cindex<50, 50, 50>);
	for (const auto& i : r1) {
		(void)i;
		++j;
	}
	require(j == 50 * 50 * 50);
}
*/

/*
module("test range literal")
{
	constexpr auto r1 = "0 0 0; 50 50 50; 1 1 1"_range;
	constexpr auto r2 = "0 0 0; 50 50 50"_range;
	constexpr auto r3 = "50 50 50"_range;
	constexpr auto r4 = "; 50 50 50; "_range;
	constexpr auto r5 = "; 50 50 50; 1 1 1"_range;
	constexpr auto r6 = "; 50 50 50;"_range;

	static_assert(r1 == r2, "");
	static_assert(r1 == r3, "");
	static_assert(r1 == r4, "");
	static_assert(r1 == r5, "");
	static_assert(r1 == r6, "");
	static_assert(r1.bases() == nd::cindex<0, 0, 0>, "");
	static_assert(r1.extents() == nd::cindex<50, 50, 50>, "");
	static_assert(r1.strides() == nd::cindex<1, 1, 1>, "");
}
*/

suite("range test")
