/*
** File Name: coords_to_offset.hpp
** Author:    Aditya Ramesh
** Date:      03/18/2015
** Contact:   _@adityaramesh.com
**
** Used to convert a variadic sequence of coordinates into an offset.
*/

#ifndef Z01F92DF2_B6C6_46AE_9E8A_8FD28E108598
#define Z01F92DF2_B6C6_46AE_9E8A_8FD28E108598

namespace nd {
namespace detail {

/*
** Suppose we are given an array with extents $e_1, ..., e_n$ and coordinates
** $c_1, ..., c_n$. Assume that the storage order of the array is such that
** $e_n$ increases the fastest and $e_1$ the slowest. Then the offset is given
** by
** 	off = e_2 * ... * e_n * c_1 +
** 	      e_3 * ... * e_n * c_2 +
** 	      e_4 * ... * e_n * c_3 +
** 	      ...                   +
** 	      e_n * c_{n - 1} + c_n.
**
** This product can be computed recursively in an obvious way; this is what is
** done below.
*/

template <size_t CurDim, size_t End, class SizeType>
struct coords_to_offset_helper
{
	using next = coords_to_offset_helper<CurDim + 1, End, SizeType>;

	template <class Array, class T, class... Ts>
	CC_ALWAYS_INLINE constexpr
	static auto apply(
		const Array& arr,
		const SizeType prod,
		const T t,
		const Ts... ts
	) noexcept
	{
		using tokens::c;
		return next::apply(arr, arr.extents().length(
			arr.storage_order().at_c(c<CurDim>)) * prod +
			t - arr.extents().start(c<CurDim>), ts...);
	}

	template <class Array>
	CC_ALWAYS_INLINE constexpr
	static auto apply(const Array&, const SizeType prod)
	noexcept { return prod; }
};

template <size_t End, class SizeType>
struct coords_to_offset_helper<End, End, SizeType>
{
	template <class Array>
	CC_ALWAYS_INLINE constexpr
	static auto apply(const Array&, const SizeType prod)
	noexcept { return prod; }
};

}

struct coords_to_offset
{
	template <class Array, class... Ts>
	CC_ALWAYS_INLINE constexpr
	static auto apply(const Array& arr, const Ts... ts) noexcept
	{
		using size_type = typename Array::size_type;
		using helper = detail::coords_to_offset_helper<
			0, Array::dims() - 1, size_type>;
		return helper::apply(arr, ts...);
	}
};

}

#endif
