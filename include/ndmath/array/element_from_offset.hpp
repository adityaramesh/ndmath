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
**
** Technical note on return value: we have to deduce the return value beforehand
** and pass it as a template parameter, because using auto would convert
** references to values, which is not what we want.
*/

template <
	size_t CurDim,
	size_t LastDim,
	class SizeType,
	class ReturnValue,
	bool IsNoexceptAccessible
>
struct element_from_offset_helper
{
	using next = element_from_offset_helper<CurDim + 1, LastDim, SizeType,
		ReturnValue, IsNoexceptAccessible>;

	template <class Array, class... Ts>
	CC_ALWAYS_INLINE constexpr
	static ReturnValue apply(
		const SizeType off,
		const SizeType prod,
		Array& arr,
		const Ts&... ts
	) noexcept(IsNoexceptAccessible)
	{
		using namespace tokens;
		return next::apply(
			off,
			prod * arr.extents().length(
				arr.storage_order().at_l(c<LastDim - CurDim>)),
			arr.extents().start(c<LastDim - CurDim>) +
			(off / prod) % arr.extents().length(
				arr.storage_order().at_l(c<LastDim - CurDim>)),
			ts...
		);
	}
};

template <
	size_t LastDim,
	class SizeType,
	class ReturnValue,
	bool IsNoexceptAccessible
>
struct element_from_offset_helper<LastDim, LastDim, SizeType, ReturnValue,
	IsNoexceptAccessible>
{
	template <class Array, class... Ts>
	CC_ALWAYS_INLINE constexpr
	static ReturnValue apply(
		const SizeType off,
		const SizeType prod,
		Array& arr,
		const Ts&... ts
	) noexcept(IsNoexceptAccessible)
	{
		using namespace tokens;
		return arr.at(arr.extents().start(c<0>) + off / prod, ts...);
	}
};

}

struct element_from_offset
{
	template <class Array>
	CC_ALWAYS_INLINE constexpr
	auto operator()(const typename Array::size_type off, Array& arr) const
	noexcept(array_traits<Array>::is_noexcept_accessible) ->
	std::conditional_t<
		std::is_const<Array>::value,
		typename array_traits<Array>::const_reference,
		typename array_traits<Array>::reference
	>
	{
		using traits = array_traits<Array>;
		using size_type = typename traits::size_type;
		using return_value = std::conditional_t<
			std::is_const<Array>::value,
			typename traits::const_reference,
			typename traits::reference
		>;
		using helper = detail::element_from_offset_helper<
			0, traits::dims - 1, size_type, return_value,
			traits::is_noexcept_accessible>;
		return helper::apply(off, 1, arr);
	}
};

}

#endif
