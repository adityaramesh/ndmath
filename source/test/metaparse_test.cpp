/*
** File Name: metaparse_test.cpp
** Author:    Aditya Ramesh
** Date:      01/11/2015
** Contact:   _@adityaramesh.com
*/

#include <ccbase/unit_test.hpp>
#include <ndmath/metaparse/parse_natural.hpp>
#include <ndmath/metaparse/skip_whitespace.hpp>
#include <ndmath/metaparse/parse_wsv.hpp>

module("test parse natural")
{
	using s1 = std::integer_sequence<char, '0', '0', '0'>;
	using s2 = std::integer_sequence<char, '1', '0', '0'>;
	using s3 = std::integer_sequence<char, '0', '0', '1'>;
	using s4 = std::integer_sequence<char, '0', '1', '0'>;
	using s5 = std::integer_sequence<char, '1', '2', '3'>;

	static_assert(nd::parse_natural<s1>::value == 0, "");
	static_assert(nd::parse_natural<s2>::value == 100, "");
	static_assert(nd::parse_natural<s3>::value == 1, "");
	static_assert(nd::parse_natural<s4>::value == 10, "");
	static_assert(nd::parse_natural<s5>::value == 123, "");
}

module("test skip whitespace")
{
	using s = std::integer_sequence<char, ' ', '\t', ' ', 'a', 'b', 'c', ' '>;
	using t = std::integer_sequence<char, 'a', 'b', 'c', ' '>;
	using u = nd::skip_whitespace<s>;
	static_assert(std::is_same<t, u>::value, "");
}

module("test parse wsv")
{
	using s = std::integer_sequence<char,
	      ' ', ' ', '0', ' ', '1', '\n',
	      ' ', ' ', '1', '2', '3', ' ', '4', '5', '6', ' ', ' ', '\n',
	      ' ', ' ', '\n',
	      '\n',
	      '0', ' ', '1', ' ', '2'
	>;
	using expected = nd::sequence<
		std::integer_sequence<uintmax_t, 0, 1>,
		std::integer_sequence<uintmax_t, 123, 456>,
		std::integer_sequence<uintmax_t>,
		std::integer_sequence<uintmax_t>,
		std::integer_sequence<uintmax_t, 0, 1, 2>
	>;
	using result = nd::parse_wsv<uintmax_t, nd::parse_natural, '\n', s>;
	static_assert(std::is_same<expected, result>::value, "");
}

suite("metaparse test");
