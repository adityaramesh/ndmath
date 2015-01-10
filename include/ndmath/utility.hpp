/*
** File Name: utility.hpp
** Author:    Aditya Ramesh
** Date:      01/09/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z389E692A_1EEC_45C1_96B9_6F99E9C26C49
#define Z389E692A_1EEC_45C1_96B9_6F99E9C26C49

#include <type_traits>

#define nd_enable_if(x)                                                             \
	class EnableIfDummyType = void,                                             \
	typename std::enable_if_t<                                                  \
		std::is_same<EnableIfDummyType, EnableIfDummyType>::value && x, int \
	> = 0

#endif
