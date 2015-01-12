/*
** File Name: for_each.hpp
** Author:    Aditya Ramesh
** Date:      01/11/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z778133CF_3F80_45F8_AC43_1BF687537BDE
#define Z778133CF_3F80_45F8_AC43_1BF687537BDE

#include <ndmath/utility.hpp>

namespace nd {

template <class Bases, class Extents, class Strides>
class range;

template <
	class Bases,
	class Extents,
	class Strides,
	class Func,
	nd_enable_if(Bases::dims() == 1)
>
CC_ALWAYS_INLINE void
for_each(const range<Bases, Extents, Strides>& r, const Func& f)
noexcept(noexcept(f(index_cn<Bases::dims(), 0>)))
{
	using index_type = typename Bases::index_type;
	for (auto i0 = r.base(0); i0 != r.extent(0); i0 += r.stride(0))
		f(make_index(i0));
}

template <
	class Bases,
	class Extents,
	class Strides,
	class Func,
	nd_enable_if(Bases::dims() == 2)
>
CC_ALWAYS_INLINE void
for_each(const range<Bases, Extents, Strides>& r, const Func& f)
noexcept(noexcept(f(index_cn<Bases::dims(), 0>)))
{
	using index_type = typename Bases::index_type;
	for (auto i0 = r.base(0); i0 != r.extent(0); i0 += r.stride(0))
	for (auto i1 = r.base(1); i1 != r.extent(1); i1 += r.stride(1))
		f(make_index(i0, i1));
}

template <
	class Bases,
	class Extents,
	class Strides,
	class Func,
	nd_enable_if(Bases::dims() == 3)
>
CC_ALWAYS_INLINE void
for_each(const range<Bases, Extents, Strides>& r, const Func& f)
noexcept(noexcept(f(index_cn<Bases::dims(), 0>)))
{
	using index_type = typename Bases::index_type;
	for (auto i0 = r.base(0); i0 != r.extent(0); i0 += r.stride(0))
	for (auto i1 = r.base(1); i1 != r.extent(1); i1 += r.stride(1))
	for (auto i2 = r.base(2); i2 != r.extent(2); i2 += r.stride(2))
		f(make_index(i0, i1, i2));
}

template <
	class Bases,
	class Extents,
	class Strides,
	class Func,
	nd_enable_if(Bases::dims() == 4)
>
CC_ALWAYS_INLINE void
for_each(const range<Bases, Extents, Strides>& r, const Func& f)
noexcept(noexcept(f(index_cn<Bases::dims(), 0>)))
{
	using index_type = typename Bases::index_type;
	for (auto i0 = r.base(0); i0 != r.extent(0); i0 += r.stride(0))
	for (auto i1 = r.base(1); i1 != r.extent(1); i1 += r.stride(1))
	for (auto i2 = r.base(2); i2 != r.extent(2); i2 += r.stride(2))
	for (auto i3 = r.base(3); i3 != r.extent(3); i3 += r.stride(3))
		f(make_index(i0, i1, i2, i3));
}

template <
	class Bases,
	class Extents,
	class Strides,
	class Func,
	nd_enable_if(Bases::dims() == 5)
>
CC_ALWAYS_INLINE void
for_each(const range<Bases, Extents, Strides>& r, const Func& f)
noexcept(noexcept(f(index_cn<Bases::dims(), 0>)))
{
	using index_type = typename Bases::index_type;
	for (auto i0 = r.base(0); i0 != r.extent(0); i0 += r.stride(0))
	for (auto i1 = r.base(1); i1 != r.extent(1); i1 += r.stride(1))
	for (auto i2 = r.base(2); i2 != r.extent(2); i2 += r.stride(2))
	for (auto i3 = r.base(3); i3 != r.extent(3); i3 += r.stride(3))
	for (auto i4 = r.base(4); i4 != r.extent(4); i4 += r.stride(4))
		f(make_index(i0, i1, i2, i3, i4));
}

template <
	class Bases,
	class Extents,
	class Strides,
	class Func,
	nd_enable_if(Bases::dims() == 6)
>
CC_ALWAYS_INLINE void
for_each(const range<Bases, Extents, Strides>& r, const Func& f)
noexcept(noexcept(f(index_cn<Bases::dims(), 0>)))
{
	using index_type = typename Bases::index_type;
	for (auto i0 = r.base(0); i0 != r.extent(0); i0 += r.stride(0))
	for (auto i1 = r.base(1); i1 != r.extent(1); i1 += r.stride(1))
	for (auto i2 = r.base(2); i2 != r.extent(2); i2 += r.stride(2))
	for (auto i3 = r.base(3); i3 != r.extent(3); i3 += r.stride(3))
	for (auto i4 = r.base(4); i4 != r.extent(4); i4 += r.stride(4))
	for (auto i5 = r.base(5); i5 != r.extent(5); i5 += r.stride(5))
		f(make_index(i0, i1, i2, i3, i4, i5));
}

template <
	class Bases,
	class Extents,
	class Strides,
	class Func,
	nd_enable_if(Bases::dims() == 7)
>
CC_ALWAYS_INLINE void
for_each(const range<Bases, Extents, Strides>& r, const Func& f)
noexcept(noexcept(f(index_cn<Bases::dims(), 0>)))
{
	using index_type = typename Bases::index_type;
	for (auto i0 = r.base(0); i0 != r.extent(0); i0 += r.stride(0))
	for (auto i1 = r.base(1); i1 != r.extent(1); i1 += r.stride(1))
	for (auto i2 = r.base(2); i2 != r.extent(2); i2 += r.stride(2))
	for (auto i3 = r.base(3); i3 != r.extent(3); i3 += r.stride(3))
	for (auto i4 = r.base(4); i4 != r.extent(4); i4 += r.stride(4))
	for (auto i5 = r.base(5); i5 != r.extent(5); i5 += r.stride(5))
	for (auto i6 = r.base(6); i6 != r.extent(6); i6 += r.stride(6))
		f(make_index(i0, i1, i2, i3, i4, i5, i6));
}

template <
	class Bases,
	class Extents,
	class Strides,
	class Func,
	nd_enable_if(Bases::dims() == 8)
>
CC_ALWAYS_INLINE void
for_each(const range<Bases, Extents, Strides>& r, const Func& f)
noexcept(noexcept(f(index_cn<Bases::dims(), 0>)))
{
	using index_type = typename Bases::index_type;
	for (auto i0 = r.base(0); i0 != r.extent(0); i0 += r.stride(0))
	for (auto i1 = r.base(1); i1 != r.extent(1); i1 += r.stride(1))
	for (auto i2 = r.base(2); i2 != r.extent(2); i2 += r.stride(2))
	for (auto i3 = r.base(3); i3 != r.extent(3); i3 += r.stride(3))
	for (auto i4 = r.base(4); i4 != r.extent(4); i4 += r.stride(4))
	for (auto i5 = r.base(5); i5 != r.extent(5); i5 += r.stride(5))
	for (auto i6 = r.base(6); i6 != r.extent(6); i6 += r.stride(6))
	for (auto i7 = r.base(7); i7 != r.extent(7); i7 += r.stride(7))
		f(make_index(i0, i1, i2, i3, i4, i5, i6, i7));
}

template <
	class Bases,
	class Extents,
	class Strides,
	class Func,
	nd_enable_if(Bases::dims() == 9)
>
CC_ALWAYS_INLINE void
for_each(const range<Bases, Extents, Strides>& r, const Func& f)
noexcept(noexcept(f(index_cn<Bases::dims(), 0>)))
{
	using index_type = typename Bases::index_type;
	for (auto i0 = r.base(0); i0 != r.extent(0); i0 += r.stride(0))
	for (auto i1 = r.base(1); i1 != r.extent(1); i1 += r.stride(1))
	for (auto i2 = r.base(2); i2 != r.extent(2); i2 += r.stride(2))
	for (auto i3 = r.base(3); i3 != r.extent(3); i3 += r.stride(3))
	for (auto i4 = r.base(4); i4 != r.extent(4); i4 += r.stride(4))
	for (auto i5 = r.base(5); i5 != r.extent(5); i5 += r.stride(5))
	for (auto i6 = r.base(6); i6 != r.extent(6); i6 += r.stride(6))
	for (auto i7 = r.base(7); i7 != r.extent(7); i7 += r.stride(7))
	for (auto i8 = r.base(8); i8 != r.extent(8); i8 += r.stride(8))
		f(make_index(i0, i1, i2, i3, i4, i5, i6, i7, i8));
}

template <
	class Bases,
	class Extents,
	class Strides,
	class Func,
	nd_enable_if(Bases::dims() == 10)
>
CC_ALWAYS_INLINE void
for_each(const range<Bases, Extents, Strides>& r, const Func& f)
noexcept(noexcept(f(index_cn<Bases::dims(), 0>)))
{
	using index_type = typename Bases::index_type;
	for (auto i0 = r.base(0); i0 != r.extent(0); i0 += r.stride(0))
	for (auto i1 = r.base(1); i1 != r.extent(1); i1 += r.stride(1))
	for (auto i2 = r.base(2); i2 != r.extent(2); i2 += r.stride(2))
	for (auto i3 = r.base(3); i3 != r.extent(3); i3 += r.stride(3))
	for (auto i4 = r.base(4); i4 != r.extent(4); i4 += r.stride(4))
	for (auto i5 = r.base(5); i5 != r.extent(5); i5 += r.stride(5))
	for (auto i6 = r.base(6); i6 != r.extent(6); i6 += r.stride(6))
	for (auto i7 = r.base(7); i7 != r.extent(7); i7 += r.stride(7))
	for (auto i8 = r.base(8); i8 != r.extent(8); i8 += r.stride(8))
	for (auto i9 = r.base(9); i9 != r.extent(9); i9 += r.stride(9))
		f(make_index(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9));

}

}

#endif
