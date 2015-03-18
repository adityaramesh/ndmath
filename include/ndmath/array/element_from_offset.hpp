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

		const auto cur_extent =
		arr.extents().at(c<arr.storage_order.at_l(c<CurDim>).value()>);

		return next::apply(
			off, prod * cur_extent,
			(off / prod) % cur_extent, ts...
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
			1, Array::dims, size_type>;
		return helper::apply(off, 1, arr);
	}
};

}

#endif
