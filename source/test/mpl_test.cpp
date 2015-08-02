/*
** File Name: mpl_test.cpp
** Author:    Aditya Ramesh
** Date:      07/27/2015
** Contact:   _@adityaramesh.com
*/

#include <cstdint>
#include <ccbase/unit_test.hpp>
#include <ndmath/mpl/parse_integer.hpp>
#include <ndmath/mpl/parse_decimal.hpp>
#include <ndmath/mpl/parse_bool.hpp>
#include <ndmath/mpl/parse_array.hpp>
#include <ndmath/mpl/flatten_list.hpp>
#include <ndmath/mpl/pack_bools.hpp>

namespace mpl = cc::mpl;

module("test parse integer")
{
	using s1 = mpl::to_types<std::integer_sequence<char, '1', '2', '3'>>;
	using s2 = mpl::to_types<std::integer_sequence<char, '-', '1', '2', '3'>>;
	using s3 = mpl::to_types<std::integer_sequence<char, '0', '1', '2', '3'>>;
	using s4 = mpl::to_types<std::integer_sequence<char, '-', '0', '1', '2', '3'>>;
	using s5 = mpl::to_types<std::integer_sequence<char, '1', '2', '3', '.'>>;
	using s6 = mpl::to_types<std::integer_sequence<char, '0'>>;
	using s7 = mpl::to_types<std::integer_sequence<char, '-', '0'>>;

	static_assert(nd::parse_integer_c<s1> == 123, "");
	static_assert(nd::parse_integer_c<s2> == -123, "");
	static_assert(nd::parse_integer_c<s3> == 123, "");
	static_assert(nd::parse_integer_c<s4> == -123, "");
	static_assert(nd::parse_integer_c<s5> == 123, "");
	static_assert(nd::parse_integer_c<s6> == 0, "");
	static_assert(nd::parse_integer_c<s7> == 0, "");
	static_assert(nd::parse_integer<s1>::tail::size() == 0, "");
	static_assert(nd::parse_integer<s2>::tail::size() == 0, "");
	static_assert(nd::parse_integer<s3>::tail::size() == 0, "");
	static_assert(nd::parse_integer<s4>::tail::size() == 0, "");
	static_assert(nd::parse_integer<s5>::tail::size() == 1, "");
	static_assert(nd::parse_integer<s6>::tail::size() == 0, "");
	static_assert(nd::parse_integer<s7>::tail::size() == 0, "");
}

template <class Scalar, class Ratio>
static constexpr auto cast = Scalar(Ratio::num) / Ratio::den;

module("test parse decimal")
{
	using s1 = mpl::to_types<std::integer_sequence<char, '1', '2', '3'>>;
	using s2 = mpl::to_types<std::integer_sequence<char, '-', '1', '2', '3'>>;
	using s3 = mpl::to_types<std::integer_sequence<char, '1', '2', '3', '.'>>;
	using s4 = mpl::to_types<std::integer_sequence<char, '1', '2', '3', '.', '0'>>;
	using s5 = mpl::to_types<std::integer_sequence<char, '1', '2', '3', '.', '1'>>;
	using s6 = mpl::to_types<std::integer_sequence<char, '1', '2', '3', '.', '0', '1'>>;
	using s7 = mpl::to_types<std::integer_sequence<char, '.', '0'>>;
	using s8 = mpl::to_types<std::integer_sequence<char, '.', '0', '1'>>;
	using s9 = mpl::to_types<std::integer_sequence<char, '-', '.', '0'>>;
	using s10 = mpl::to_types<std::integer_sequence<char, '0', 'e', '0'>>;
	using s11 = mpl::to_types<std::integer_sequence<char, '1', 'e', '0'>>;
	using s12 = mpl::to_types<std::integer_sequence<char, '1', 'e', '1'>>;
	using s13 = mpl::to_types<std::integer_sequence<char, '1', 'e', '-', '1'>>;
	using s14 = mpl::to_types<std::integer_sequence<char, '1', 'e', '0', '1'>>;
	using s15 = mpl::to_types<std::integer_sequence<char, '1', 'e', '-', '0', '1'>>;
	using s16 = mpl::to_types<std::integer_sequence<char, '1', '.', 'e', '1'>>;
	using s17 = mpl::to_types<std::integer_sequence<char, '.', '1', 'e', '1'>>;

	using t1 = typename nd::parse_decimal<s1>::type;
	using t2 = typename nd::parse_decimal<s2>::type;
	using t3 = typename nd::parse_decimal<s3>::type;
	using t4 = typename nd::parse_decimal<s4>::type;
	using t5 = typename nd::parse_decimal<s5>::type;
	using t6 = typename nd::parse_decimal<s6>::type;
	using t7 = typename nd::parse_decimal<s7>::type;
	using t8 = typename nd::parse_decimal<s8>::type;
	using t9 = typename nd::parse_decimal<s9>::type;
	using t10 = typename nd::parse_decimal<s10>::type;
	using t11 = typename nd::parse_decimal<s11>::type;
	using t12 = typename nd::parse_decimal<s12>::type;
	using t13 = typename nd::parse_decimal<s13>::type;
	using t14 = typename nd::parse_decimal<s14>::type;
	using t15 = typename nd::parse_decimal<s15>::type;
	using t16 = typename nd::parse_decimal<s16>::type;
	using t17 = typename nd::parse_decimal<s17>::type;

	static_assert(cast<double, t1> == 123, "");
	static_assert(cast<double, t2> == -123, "");
	static_assert(cast<double, t3> == 123, "");
	static_assert(cast<double, t4> == 123, "");
	static_assert(cast<double, t5> == 123.1, "");
	static_assert(cast<double, t6> == 123.01, "");
	static_assert(cast<double, t7> == 0, "");
	static_assert(cast<double, t8> == 0.01, "");
	static_assert(cast<double, t9> == 0, "");
	static_assert(cast<double, t10> == 0, "");
	static_assert(cast<double, t11> == 1, "");
	static_assert(cast<double, t12> == 10, "");
	static_assert(cast<double, t13> == 0.1, "");
	static_assert(cast<double, t14> == 10, "");
	static_assert(cast<double, t15> == 0.1, "");
	static_assert(cast<double, t16> == 10, "");
	static_assert(cast<double, t17> == 1, "");
}

module("test parse bool")
{
	using s1 = mpl::to_types<std::integer_sequence<char, 't', 'r', 'u', 'e'>>;
	using s2 = mpl::to_types<std::integer_sequence<char, 'f', 'a', 'l', 's', 'e'>>;

	using t1 = nd::parse_bool<s1>;
	using t2 = nd::parse_bool<s2>;

	static_assert(t1::type::value == true, "");
	static_assert(t2::type::value == false, "");

	static_assert(t1::tail::size() == 0, "");
	static_assert(t2::tail::size() == 0, "");
}

module("test parse array")
{
	using s1 = mpl::to_types<std::integer_sequence<char, '[', '1', ']'>>;
	using s2 = mpl::to_types<std::integer_sequence<char, '[', '1', ' ', '2', ']'>>;
	using s3 = mpl::to_types<std::integer_sequence<char, '[', '[', '1', ']', ']'>>;
	using s4 = mpl::to_types<std::integer_sequence<char, '[', '[', '1', ']', '[', '1', ']', ']'>>;
	using s5 = mpl::to_types<std::integer_sequence<char, '[', 't', 'r', 'u', 'e', ']'>>;

	using t1 = nd::parse_array<mpl::quote<nd::parse_decimal>, s1>;
	using t2 = nd::parse_array<mpl::quote<nd::parse_decimal>, s2>;
	using t3 = nd::parse_array<mpl::quote<nd::parse_decimal>, s3>;
	using t4 = nd::parse_array<mpl::quote<nd::parse_decimal>, s4>;
	using t5 = nd::parse_array<mpl::quote<nd::parse_bool>, s5>;
}

module("test flatten list")
{
	using l1 = mpl::list<mpl::int_<1>, mpl::int_<2>, mpl::int_<3>>;
	using l2 = mpl::list<
		mpl::list<mpl::int_<1>>,
		mpl::int_<2>,
		mpl::list<mpl::int_<3>>
	>;
	using l3 = mpl::list<
		mpl::list<mpl::list<>>,
		mpl::list<mpl::int_<1>, mpl::list<mpl::int_<2>>, mpl::int_<3>>
	>;

	using m1 = typename nd::flatten_list<l1>::type;
	using m2 = typename nd::flatten_list<l2>::type;
	using m3 = typename nd::flatten_list<l3>::type;
	using n  = mpl::list<mpl::int_<1>, mpl::int_<2>, mpl::int_<3>>;

	static_assert(mpl::lists_same<m1, n>::value, "");
	static_assert(mpl::lists_same<m2, n>::value, "");
	static_assert(mpl::lists_same<m3, n>::value, "");
}

module("test pack bools")
{
	using l1 = mpl::to_types<std::integer_sequence<bool, true, false, true, false>>;
	using m1 = nd::pack_bools<unsigned, l1>;

	static_assert(m1::value == 0b0101, "");
}

module("test pack bool lists")
{
	using l1 = mpl::to_types<std::integer_sequence<bool, true, false, true, false>>;
	using l2 = mpl::append<mpl::bool_<false>, mpl::repeat_nc<64, mpl::bool_<true>>>;

	using m1 = nd::pack_bool_lists<uint32_t, l1>;
	using m2 = nd::pack_bool_lists<uint32_t, l2>;

	static_assert(mpl::at_c<0, m1>::value == 5u, "");
	static_assert(mpl::at_c<0, m2>::value == 4294967295u, "");
	static_assert(mpl::at_c<1, m2>::value == 4294967295u, "");
	static_assert(mpl::at_c<2, m2>::value == 0, "");
}

suite("mpl test")
