/*
** File Name: element_from_offset.hpp
** Author:    Aditya Ramesh
** Date:      03/18/2015
** Contact:   _@adityaramesh.com
**
** A generic method to obtain the element of an array corresponding to a given
** offset. Faster approaches may be possible in various cases. If such an
** approach exists, define a specialized offset to element function instead of
** using this one.
*/

#ifndef Z19813ACB_83A9_4CE4_AAAB_E02079D412E8
#define Z19813ACB_83A9_4CE4_AAAB_E02079D412E8

namespace nd {
namespace detail {

/*
** Suppose we are given an array with extents $e_1, ..., e_n$ and an offset off.
** Assume that the storage order of the array is such that $e_n$ increases the
** fastest and $e_1$ the slowest. Then the coordinates $c_1, ..., c_n$
** corresponding to off are given by:
** - c_1 = floor(off / e_2 * ... * e_n)
** - c_2 = floor(off / e_3 * ... * e_n) % e_2
** - c_3 = floor(off / e_4 * ... * e_n) % e_3
** - c_n = off % e_n
*/

template <size_t CurDim, size_t LastDim, class SizeType>
struct element_from_offset_helper
{
	using next = element_from_offset_helper<CurDim + 1, LastDim, SizeType>;

	template <class Integer, class Array, class... Ts>
	CC_ALWAYS_INLINE constexpr
	static auto apply(
		const SizeType off,
		const SizeType prod,
		Array& arr,
		const Ts&... ts
	) noexcept(Array::is_noexcept_accessible)
	{
		using namespace tokens;

		return next::apply(
			off,
			prod * arr.extents().at(
				arr.storage_order().at_l(c<LastDim - CurDim>)),
			(off / prod) % arr.extents().at(
				arr.storage_order().at_l(c<LastDim - CurDim>)),
			ts...
		);
	}
};

template <size_t LastDim, class SizeType>
struct element_from_offset_helper<LastDim, LastDim, SizeType>
{
	template <class Integer, class Array, class... Ts>
	CC_ALWAYS_INLINE constexpr
	static auto apply(
		const SizeType off,
		const SizeType prod,
		Array& arr,
		const Ts&... ts
	) noexcept(Array::is_noexcept_accessible)
	{ return arr(off / prod, ts...); }
};

}

struct element_from_offset
{
	template <class Array>
	CC_ALWAYS_INLINE constexpr
	auto operator()(const typename Array::size_type off, Array& arr) const
	noexcept(Array::is_noexcept_accessible)
	{
		using size_type = typename Array::size_type;
		using helper = detail::element_from_offset_helper<
			0, Array::dims - 1, size_type>;
		return helper::apply(off, 1, arr);
	}
};

}

#endif
