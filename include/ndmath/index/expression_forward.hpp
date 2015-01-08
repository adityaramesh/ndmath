/*
** File Name: expression_forward.hpp
** Author:    Aditya Ramesh
** Date:      01/08/2015
** Contact:   _@adityaramesh.com
*/

#ifndef ZC9BB3DD3_7099_474A_B255_9C79D1009DE7
#define ZC9BB3DD3_7099_474A_B255_9C79D1009DE7

#include <cstddef>
#include <ccbase/platform.hpp>

namespace nd {

/*
** subindex
*/

template <class Index>
class subindex;

template <size_t A, class Index>
class cl_subindex;

template <size_t B, class Index>
class cr_subindex;

template <size_t A, size_t B, class Index>
class c_subindex;

template <class Index>
CC_ALWAYS_INLINE auto
make_subindex(Index& i) noexcept
{ return subindex<Index>{i}; }

template <size_t A, class Index>
CC_ALWAYS_INLINE auto
make_cl_subindex(Index& i) noexcept
{ return cl_subindex<A, Index>{i}; }

template <size_t B, class Index>
CC_ALWAYS_INLINE auto
make_cr_subindex(Index& i) noexcept
{ return cr_subindex<B, Index>{i}; }

template <size_t A, size_t B, class Index>
CC_ALWAYS_INLINE auto
make_c_subindex(Index& i) noexcept
{ return c_subindex<A, B, Index>{i}; }

/*
** composite_index
*/

template <class Index1, class Index2>
class composite_index;

template <class Index1, class Index2>
CC_ALWAYS_INLINE auto
make_composite_index(Index1& i1, Index2& i2)
{ return composite_index<Index1, Index2>{i1, i2}; }

}

#endif
