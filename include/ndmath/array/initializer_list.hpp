/*
** File Name: initializer_list.hpp
** Author:    Aditya Ramesh
** Date:      06/28/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z0C75DF68_FD1F_48DF_B7BC_8EB9215DA030
#define Z0C75DF68_FD1F_48DF_B7BC_8EB9215DA030

#include <ndmath/common.hpp>

namespace nd {
namespace detail {

template <class T, size_t CurLevel>
struct nested_initializer_list_helper
{
	using nested_type =
	typename nested_initializer_list_helper<T, CurLevel - 1>::type;

	using type = std::initializer_list<nested_type>;
};

template <class T>
struct nested_initializer_list_helper<T, 0>
{
	using type = std::initializer_list<T>;
};

}

template <class T, size_t Levels>
using nested_initializer_list =
typename detail::nested_initializer_list_helper<T, Levels - 1>::type;

namespace detail {

template <size_t CurCoord, size_t MaxCoord>
struct init_list_element_access
{
	using next = init_list_element_access<CurCoord + 1, MaxCoord>;

	template <class List, class Index>
	CC_ALWAYS_INLINE
	static auto apply(const List& list, const Index& i) noexcept
	{
		nd_assert(
			i(sc_coord<CurCoord>) < list.size(),
			"coordinate $ of index $ exceeds size of corresponding "
			"nested initializer list.\n▶ $ ≥ $",
			CurCoord + 1, i, i(sc_coord<CurCoord>), list.size()
		);
		return next::apply(*(list.begin() + i(sc_coord<CurCoord>)), i);
	}
};

template <size_t MaxCoord>
struct init_list_element_access<MaxCoord, MaxCoord>
{
	template <class List, class Index>
	CC_ALWAYS_INLINE
	static auto apply(const List& list, const Index& i) noexcept
	{
		nd_assert(
			i(sc_coord<MaxCoord>) < list.size(),
			"coordinate $ of index $ exceeds size of corresponding "
			"nested initializer list.\n▶ $ ≥ $",
			MaxCoord + 1, i, i(sc_coord<MaxCoord>), list.size()
		);
		return *(list.begin() + i(sc_coord<MaxCoord>));
	}
};

}

template <class T, size_t Levels, class Index>
CC_ALWAYS_INLINE auto
get_init_list_element(const nested_initializer_list<T, Levels>& list, const Index& i)
noexcept
{
	static_assert(Levels == Index::dims(), "");
	using helper = detail::init_list_element_access<0, Levels - 1>;
	return helper::apply(list, i);
}

}

#endif
