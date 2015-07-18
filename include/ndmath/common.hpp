/*
** File Name: common.hpp
** Author:    Aditya Ramesh
** Date:      01/09/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z389E692A_1EEC_45C1_96B9_6F99E9C26C49
#define Z389E692A_1EEC_45C1_96B9_6F99E9C26C49

#include <cstddef>
#include <cstdlib>
#include <type_traits>
#include <utility>
#include <ccbase/mpl.hpp>
#include <ccbase/format.hpp>
#include <ccbase/platform.hpp>
#include <boost/preprocessor/comparison/equal.hpp>
#include <boost/preprocessor/variadic/size.hpp>

#ifndef nd_no_debug
	#define nd_print_assert_2(cond, file, line) \
		cc::errln("Assertion `$` failed in `$`, line $.", \
			cond, file, line)

	#define nd_print_assert_0(cond, file, line, fmt, ...) \
		cc::errln("Assertion `$` failed in `$`, line $: " fmt ".", \
			cond, file, line, __VA_ARGS__)

	#define nd_print_assert_1(cond, file, line, msg) \
		cc::errln("Assertion `$` failed in `$`, line $: " msg ".", \
			cond, file, line)

	#define nd_print_assert_helper_2(x, ...) \
		nd_print_assert_ ## x(__VA_ARGS__)

	#define nd_print_assert_helper_1(x, ...) \
		nd_print_assert_helper_2(x, __VA_ARGS__)

	#define nd_assert_impl_0(cond, ...)                                          \
		do {                                                                 \
			if (!(cond)) {                                               \
				nd_print_assert_helper_1(                            \
					BOOST_PP_EQUAL(                              \
						BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), \
						1                                    \
					),                                           \
					#cond, __FILE__, __LINE__, __VA_ARGS__       \
				);                                                   \
				std::exit(EXIT_FAILURE);                             \
			}                                                            \
		}                                                                    \
		while (false)

	#define nd_assert_impl_1(cond)                                        \
		do {                                                          \
			if (!(cond)) {                                        \
				nd_print_assert_2(#cond, __FILE__, __LINE__); \
				std::exit(EXIT_FAILURE);                      \
			}                                                     \
		}                                                             \
		while (false)

	#define nd_assert_helper_2(x, ...) \
		nd_assert_impl_ ## x(__VA_ARGS__)

	#define nd_assert_helper_1(x, ...) nd_assert_helper_2(x, __VA_ARGS__)

	#define nd_assert(...) \
		nd_assert_helper_1(BOOST_PP_EQUAL( \
			BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), 1), __VA_ARGS__)
#else
	#define nd_assert(...)
#endif

#define nd_enable_if(x)                                                             \
	class EnableIfDummyType = void,                                             \
	typename std::enable_if_t<                                                  \
		std::is_same<EnableIfDummyType, EnableIfDummyType>::value && x, int \
	> = 0

#define nd_deduce_noexcept(x) \
	noexcept(noexcept(x)) { return x; }

#define nd_deduce_noexcept_and_return_type(x) \
	noexcept(noexcept(x))                 \
	-> decltype(x) { return x; }

namespace nd {
	
namespace mpl = cc::mpl;

struct in_place_t {};
static constexpr auto in_place = in_place_t{};

}

#endif
