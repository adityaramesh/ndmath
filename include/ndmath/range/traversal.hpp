/*
** File Name: traversal.hpp
** Author:    Aditya Ramesh
** Date:      01/11/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z778133CF_3F80_45F8_AC43_1BF687537BDE
#define Z778133CF_3F80_45F8_AC43_1BF687537BDE

namespace nd {
namespace detail {

namespace for_each {

// Forward declaration for the struct in `for_each.hpp`.
template <size_t Dim, size_t Dims, class Attribs, bool Noexcept>
struct evaluator;

}

namespace do_while {

// Forward declaration for the struct in `do_while.hpp`.
template <size_t Dim, size_t Dims, class Attribs, bool Noexcept>
struct evaluator;

}

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
	using detail::for_each::evaluator;
	static constexpr auto dims = Bases::dims();
	static constexpr auto is_noexcept = noexcept(f(sc_index_n<dims, 0>));
	using helper = evaluator<0, Bases::dims(), Attribs, is_noexcept>;
	helper::apply(r, f);
}

template <
	class Bases,
	class Extents,
	class Strides,
	class Attribs,
	class Func,
	nd_enable_if((std::is_same<
		std::decay_t<
			std::result_of_t<Func(decltype(sc_index_n<Bases::dims(), 0>))>
		>, bool
	>::value))
>
CC_ALWAYS_INLINE void
do_while(const range<Bases, Extents, Strides, Attribs>& r, const Func& f)
noexcept(noexcept(f(sc_index_n<Bases::dims(), 0>)))
{
	using detail::do_while::evaluator;
	static constexpr auto dims = Bases::dims();
	static constexpr auto is_noexcept = noexcept(f(sc_index_n<dims, 0>));
	using helper = evaluator<0, Bases::dims(), Attribs, is_noexcept>;
	helper::apply(r, f);
}

}

#endif
