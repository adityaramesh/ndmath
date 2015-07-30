/*
** File Name: mpl_test.cpp
** Author:    Aditya Ramesh
** Date:      07/27/2015
** Contact:   _@adityaramesh.com
*/

#include <ccbase/unit_test.hpp>
#include <ndmath/mpl/parse_integer.hpp>
#include <ndmath/mpl/parse_decimal.hpp>

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
}

suite("mpl test")
