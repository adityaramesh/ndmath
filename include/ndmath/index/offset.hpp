/*
** File Name: offset.hpp
** Author:    Aditya Ramesh
** Date:      01/12/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z657D1622_7A43_4C33_91E0_DC7828EA6FF6
#define Z657D1622_7A43_4C33_91E0_DC7828EA6FF6

namespace nd {

template <
	size_t Dims,
	bool IsConstexpr1,
	bool IsConstexpr2,
	class Result1,
	class Result2,
	class ConstResult1,
	class ConstResult2,
	class Derived1,
	class Derived2
>
CC_ALWAYS_INLINE auto
offset(
	const index_base<Dims, IsConstexpr1, Result1, ConstResult1, Derived1>& n,
	const index_base<Dims, IsConstexpr2, Result2, ConstResult2, Derived2>& e
) noexcept
{
	using namespace tokens;
	if (Dims == 1) return n(0);

	auto off = n(0);
	for (auto i = size_t{1}; i != Dims; ++i) {
		off = e(end - i) * 
		// TODO
	}
	return off;
}

template <
	size_t Dims,
	bool IsConstexpr1,
	bool IsConstexpr2,
	class Result1,
	class Result2,
	class ConstResult1,
	class ConstResult2,
	class Derived1,
	class Derived2
>
CC_ALWAYS_INLINE CC_CONST constexpr
auto offset(
	const index_base<Dims, IsConstexpr1, Result1, ConstResult1, Derived1>& n,
	const index_base<Dims, IsConstexpr2, Result2, ConstResult2, Derived2>& e
) noexcept
{
	using namespace tokens;
	if (Dims == 1) return n(0);

	auto off = e(end - 1) * n(0);
	for (auto i = size_t{1}; i != Dims; ++i) {
		off = e(end - i) * (off + n(i));
	}
	return off;
}

/*
** TODO: offset given storage order?
*/


}

#endif
