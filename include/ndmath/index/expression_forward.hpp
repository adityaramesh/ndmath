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

template <size_t Dims, bool IsConstexpr, class Derived>
class index_base;

template <size_t Dims, class Derived>
CC_ALWAYS_INLINE auto
make_subindex(index_base<Dims, false, Derived>& i)
noexcept { return subindex<index_base<Dims, false, Derived>>{i}; }

template <size_t Dims, class Derived>
CC_ALWAYS_INLINE auto
make_subindex(const index_base<Dims, false, Derived>& i)
noexcept { return subindex<index_base<Dims, false, Derived>>{i}; }

template <size_t A, size_t Dims, class Derived>
CC_ALWAYS_INLINE auto
make_cl_subindex(index_base<Dims, false, Derived>& i)
noexcept { return cl_subindex<A, index_base<Dims, false, Derived>>{i}; }

template <size_t A, size_t Dims, class Derived>
CC_ALWAYS_INLINE auto
make_cl_subindex(const index_base<Dims, false, Derived>& i)
noexcept { return cl_subindex<A, index_base<Dims, false, Derived>>{i}; }

template <size_t A, size_t Dims, class Derived>
CC_ALWAYS_INLINE CC_CONST constexpr
auto make_cl_subindex(const index_base<Dims, true, Derived>& i)
noexcept { return cl_subindex<A, index_base<Dims, true, Derived>>{i}; }

template <size_t B, size_t Dims, class Derived>
CC_ALWAYS_INLINE auto
make_cr_subindex(index_base<Dims, false, Derived>& i)
noexcept { return cr_subindex<B, index_base<Dims, false, Derived>>{i}; }

template <size_t B, size_t Dims, class Derived>
CC_ALWAYS_INLINE auto
make_cr_subindex(const index_base<Dims, false, Derived>& i)
noexcept { return cr_subindex<B, index_base<Dims, false, Derived>>{i}; }

template <size_t B, size_t Dims, class Derived>
CC_ALWAYS_INLINE CC_CONST constexpr
auto make_cr_subindex(const index_base<Dims, true, Derived>& i)
noexcept { return cr_subindex<B, index_base<Dims, true, Derived>>{i}; }

template <size_t A, size_t B, size_t Dims, class Derived>
CC_ALWAYS_INLINE auto
make_c_subindex(index_base<Dims, false, Derived>& i)
noexcept { return c_subindex<A, B, index_base<Dims, false, Derived>>{i}; }

template <size_t A, size_t B, size_t Dims, class Derived>
CC_ALWAYS_INLINE auto
make_c_subindex(const index_base<Dims, false, Derived>& i)
noexcept { return c_subindex<A, B, index_base<Dims, false, Derived>>{i}; }

template <size_t A, size_t B, size_t Dims, class Derived>
CC_ALWAYS_INLINE CC_CONST constexpr
auto make_c_subindex(const index_base<Dims, true, Derived>& i)
noexcept { return c_subindex<A, B, index_base<Dims, true, Derived>>{i}; }

/*
** composite_index
*/

template <class Index1, class Index2>
class composite_index;

template <
	size_t Dims1,
	size_t Dims2,
	bool IsConstexpr1,
	bool IsConstexpr2,
	class Derived1,
	class Derived2
>
CC_ALWAYS_INLINE auto
operator,(
	index_base<Dims1, IsConstexpr1, Derived1>& lhs,
	index_base<Dims2, IsConstexpr2, Derived2>& rhs
) noexcept
{
	return composite_index<
		index_base<Dims1, IsConstexpr1, Derived1>,
		index_base<Dims2, IsConstexpr2, Derived2>
	>{lhs, rhs};
}

template <
	size_t Dims1,
	size_t Dims2,
	bool IsConstexpr1,
	bool IsConstexpr2,
	class Derived1,
	class Derived2
>
CC_ALWAYS_INLINE auto
operator,(
	const index_base<Dims1, IsConstexpr1, Derived1>& lhs,
	const index_base<Dims2, IsConstexpr2, Derived2>& rhs
) noexcept
{
	return composite_index<
		index_base<Dims1, IsConstexpr1, Derived1>,
		index_base<Dims2, IsConstexpr2, Derived2>
	>{lhs, rhs};
}

template <
	size_t Dims1,
	size_t Dims2,
	class Derived1,
	class Derived2
>
CC_ALWAYS_INLINE CC_CONST constexpr
auto operator,(
	const index_base<Dims1, true, Derived1>& lhs,
	const index_base<Dims2, true, Derived2>& rhs
) noexcept
{
	return composite_index<
		index_base<Dims1, true, Derived1>,
		index_base<Dims2, true, Derived2>
	>{lhs, rhs};
}

}

#endif
