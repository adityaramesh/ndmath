/*
** File Name: for_each.hpp
** Author:    Aditya Ramesh
** Date:      01/11/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z778133CF_3F80_45F8_AC43_1BF687537BDE
#define Z778133CF_3F80_45F8_AC43_1BF687537BDE

namespace nd {
namespace detail {

// Forward declaration for the struct in `loop_optimization.hpp`.
template <size_t Dim, size_t Dims, class Attribs, bool Noexcept>
struct evaluate_helper;

}

template <class Bases, class Extents, class Strides, class Attribs>
class range;

template <
	class Bases,
	class Extents,
	class Strides,
	class Attribs,
	class Func
>
CC_ALWAYS_INLINE void
for_each(const range<Bases, Extents, Strides, Attribs>& r, const Func& f)
noexcept(noexcept(f(sc_index_n<Bases::dims(), 0>)))
{
	static constexpr auto dims = Bases::dims();
	static constexpr auto is_noexcept = noexcept(f(sc_index_n<dims, 0>));
	using helper = detail::evaluate_helper<0, Bases::dims(), Attribs, is_noexcept>;
	helper::apply(r, f);
}

}

#endif
