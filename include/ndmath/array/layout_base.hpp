/*
** File Name: layout_base.hpp
** Author:    Aditya Ramesh
** Date:      03/16/2015
** Contact:   _@adityaramesh.com
**
** If the extents are statically accessible, then there is no need to waste a
** byte of memory by storing a dummy object. To avoid doing this, we delegate
** the responsibility of providing access to the extents and storage order to
** `layout_base`.
*/

#ifndef ZD22085D9_6F37_4A10_B685_9D0CCE06A44B
#define ZD22085D9_6F37_4A10_B685_9D0CCE06A44B

namespace nd {

template <
	class Extents,
	class StorageOrder,
	bool ExtentsStaticallyAccessible
>
class layout_base_impl;

template <class Extents, class StorageOrder>
class layout_base_impl<Extents, StorageOrder, true>
{
public:
	CC_ALWAYS_INLINE explicit
	layout_base_impl() noexcept {}

	CC_ALWAYS_INLINE explicit
	layout_base_impl(Extents) noexcept {}

	CC_ALWAYS_INLINE constexpr
	auto extents() const noexcept
	{ return Extents{}; }

	template <class Extents_, nd_enable_if((
		std::is_assignable<Extents, Extents_>::value	
	))>
	CC_ALWAYS_INLINE constexpr
	void extents(Extents_) const noexcept {}

	CC_ALWAYS_INLINE constexpr
	auto storage_order() const noexcept
	{ return StorageOrder{}; }
};

template <class Extents, class StorageOrder>
class layout_base_impl<Extents, StorageOrder, false>
{
private:
	Extents m_extents{};
public:
	CC_ALWAYS_INLINE explicit
	layout_base_impl() noexcept {}

	CC_ALWAYS_INLINE explicit
	layout_base_impl(const Extents& e)
	noexcept : m_extents{e} {}

	CC_ALWAYS_INLINE
	auto& extents() noexcept
	{ return m_extents; }

	CC_ALWAYS_INLINE constexpr
	auto& extents() const noexcept
	{ return m_extents; }

	template <class Extents_, nd_enable_if((
		std::is_assignable<Extents, Extents_>::value
	))>
	CC_ALWAYS_INLINE
	void extents(const Extents_& e) noexcept
	{ m_extents = e; }

	CC_ALWAYS_INLINE constexpr
	auto storage_order() const noexcept
	{ return StorageOrder{}; }
};

template <class Extents, class StorageOrder>
using layout_base = layout_base_impl<
	Extents, StorageOrder, Extents::allows_static_access
>;

}

#endif
