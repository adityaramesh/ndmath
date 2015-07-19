/*
** File Name: loop_optimization.hpp
** Author:    Aditya Ramesh
** Date:      01/21/2015
** Contact:   _@adityaramesh.com
*/

#ifndef ZCD72012D_4FBC_4CC8_AC50_77209FC0C952
#define ZCD72012D_4FBC_4CC8_AC50_77209FC0C952

namespace nd {
namespace detail {

/*
** Helps us to avoid doubling the number of template specializations to address
** the direction of traversal along each coordinate. This class can be used to
** write loops in a direction-independent way, as long as the length of the
** range traversed by the loop can be described using a relatively simple
** expression.
*/
template <class Direction>
struct direction_helper;

template <>
struct direction_helper<forward>
{
	template <class Integer>
	CC_ALWAYS_INLINE constexpr
	static auto start(const Integer& start, Integer) noexcept
	{ return start; }

	template <class Integer>
	CC_ALWAYS_INLINE constexpr
	static auto finish(const Integer& start, Integer, const Integer& len) noexcept
	{ return start + len; }

	template <class Integer>
	CC_ALWAYS_INLINE
	static void step(Integer& counter, const Integer& n) noexcept
	{ counter += n; }
};

template <>
struct direction_helper<backward>
{
	template <class Integer>
	CC_ALWAYS_INLINE constexpr
	static auto start(Integer, const Integer& end) noexcept
	{ return end; }

	template <class Integer>
	CC_ALWAYS_INLINE constexpr
	static auto finish(Integer, const Integer& end, const Integer& len) noexcept
	{ return end - len; }

	template <class Integer>
	CC_ALWAYS_INLINE
	static void step(Integer& counter, const Integer& n) noexcept
	{ counter -= n; }
};

}}

#endif
