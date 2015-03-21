/*
** File Name: dense_storage_test.cpp
** Author:    Aditya Ramesh
** Date:      03/20/2015
** Contact:   _@adityaramesh.com
*/

#include <ccbase/unit_test.hpp>
#include <ndmath/array/dense_storage.hpp>

module("test construction")
{
	auto arr = nd::make_darray<float>(nd::cextents<20, 20>);
}

suite("dense storage test")
