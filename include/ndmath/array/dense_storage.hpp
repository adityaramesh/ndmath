/*
** File Name: dense_storage.hpp
** Author:    Aditya Ramesh
** Date:      03/16/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z48CDD07C_465F_4840_8116_DB760EF85D3B
#define Z48CDD07C_465F_4840_8116_DB760EF85D3B

#include <ndmath/array/array_wrapper.hpp>
#include <ndmath/array/layout_base.hpp>
#include <ndmath/array/coords_to_offset.hpp>
#include <ndmath/array/boolean_proxy.hpp>
#include <ndmath/array/construction_proxy.hpp>
#include <ndmath/array/storage_order.hpp>

namespace nd {

template <class T, class Extents, class StorageOrder, class Alloc>
class dense_storage;

namespace detail {

template <class T>
struct dense_storage_access
{
	using underlying_type = T;

	template <class SizeType, class Array>
	CC_ALWAYS_INLINE
	static auto& at(const SizeType off, Array& arr) noexcept
	{ return arr.data()[off]; }

	template <class SizeType, class Extents, class StorageOrder>
	CC_ALWAYS_INLINE
	static auto uninitialized_at(
		const SizeType off,
		dense_storage<T, Extents, StorageOrder, void>& arr
	) noexcept
	{
		using proxy_type = construction_proxy<underlying_type, void>;
		return proxy_type{at(off, arr)};
	}

	template <class SizeType, class Extents, class StorageOrder, class Alloc>
	CC_ALWAYS_INLINE
	static auto uninitialized_at(
		const SizeType off,
		dense_storage<T, Extents, StorageOrder, Alloc>& arr
	) noexcept
	{
		using array_type      = std::decay_t<decltype(arr)>;
		using allocator_type  = typename array_type::allocator_type;
		using proxy_type      = construction_proxy<underlying_type, allocator_type>;
		return proxy_type{at(off, arr), arr.allocator()};
	}

	template <class SizeType>
	CC_ALWAYS_INLINE constexpr
	static auto underlying_size(const SizeType n) noexcept
	{ return n; }
};

template <>
struct dense_storage_access<bool>
{
	using underlying_type = unsigned;

	template <class SizeType, class Array>
	CC_ALWAYS_INLINE
	static auto at(const SizeType off, Array& arr)
	noexcept
	{
		using value_type = std::conditional_t<
			std::is_const<Array>::value,
			const underlying_type, underlying_type>;
		using proxy_type = boolean_proxy<value_type, SizeType>;
		return proxy_type{arr.data()[underlying_offset(off)], off};
	}

	template <class SizeType, class Extents, class StorageOrder>
	CC_ALWAYS_INLINE
	static auto uninitialized_at(
		const SizeType off,
		dense_storage<bool, Extents, StorageOrder, void>& arr
	) noexcept
	{
		using proxy_type = construction_proxy<underlying_type, void>;
		return proxy_type{arr.data()[underlying_offset(off)]};
	}

	template <class SizeType, class Extents, class StorageOrder, class Alloc>
	CC_ALWAYS_INLINE
	static auto uninitialized_at(
		const SizeType off,
		dense_storage<bool, Extents, StorageOrder, Alloc>& arr
	) noexcept
	{
		using array_type     = std::decay_t<decltype(arr)>;
		using allocator_type = typename array_type::allocator_type;
		using proxy_type     = construction_proxy<underlying_type, allocator_type>;
		return proxy_type{arr.data()[underlying_offset(off)], arr.allocator()};
	}

	template <class SizeType>
	CC_ALWAYS_INLINE constexpr
	static auto underlying_offset(const SizeType n)
	noexcept { return n / SizeType(8 * sizeof(underlying_type)); }

	template <class SizeType>
	CC_ALWAYS_INLINE constexpr
	static auto underlying_size(const SizeType n)
	noexcept
	{
		nd_assert(n != 0, "array must have nonzero size");
		return SizeType{1} + underlying_offset(n - 1);
	}
};

struct construction_view_access
{
	template <class SizeType, class Array>
	CC_ALWAYS_INLINE constexpr
	auto operator()(const SizeType off, Array& arr)
	const noexcept
	{
		using helper = typename Array::helper;
		return helper::uninitialized_at(off, arr);
	}
};

}

template <class T, class Extents, class StorageOrder, class Alloc>
class dense_storage;

template <class T, class Extents, class StorageOrder>
class dense_storage<T, Extents, StorageOrder, void> final :
layout_base<Extents, StorageOrder>
{
public:
	CC_ALWAYS_INLINE constexpr
	static auto dims() noexcept
	{ return Extents::dims(); }
private:
	using base   = layout_base<Extents, StorageOrder>;
	using helper = detail::dense_storage_access<T>;

	friend struct detail::construction_view_access;
	friend struct detail::dense_storage_access<T>;

	using start = std::decay_t<decltype(std::declval<Extents>().start())>;
	using strides = std::decay_t<decltype(std::declval<Extents>().strides())>;

	static_assert(
		Extents::allows_static_access,
		"Extents of static dense storage must be statically accessible."
	);
	static_assert(
		start{} == sc_index_n<dims(), 0>,
		"Start of range of dense storage must be the zero index."
	);
	static_assert(
		strides{} == sc_index_n<dims(), 1>,
		"Range of dense storage must have unit stride."
	);
public:
	using size_type       = unsigned;
	using value_type      = std::decay_t<T>;
	using underlying_type = typename helper::underlying_type;
	static constexpr auto is_lazy = false;

	using base::extents;
	using base::storage_order;
private:
	using size_c = decltype(std::declval<Extents>().size_c());
	static constexpr auto m_size = std::decay_t<size_c>::value();

	std::array<underlying_type, m_size> m_data;
public:
	CC_ALWAYS_INLINE constexpr
	explicit dense_storage(uninitialized_t) noexcept {}

	CC_ALWAYS_INLINE constexpr
	explicit dense_storage()
	noexcept(noexcept(
		std::is_nothrow_default_constructible<underlying_type>::value))
	{
		/*
		** See comments regarding construction in the other
		** specialization of `dense_storage`.
		*/
		if (!std::is_trivial<underlying_type>::value) {
			for (auto i = size_t{0}; i != underlying_size(); ++i) {
				::new (&m_data[i]) underlying_type{};
			}
		}
	}

	template <class U>
	CC_ALWAYS_INLINE constexpr
	explicit dense_storage(const U& init)
	noexcept(noexcept(
		std::is_nothrow_constructible<underlying_type, const U&>::value))
	{
		for (auto i = size_t{0}; i != underlying_size(); ++i) {
			::new (&m_data[i]) underlying_type(init);
		}
	}

	CC_ALWAYS_INLINE
	~dense_storage()
	{
		for (auto i = size_t{0}; i != underlying_size(); ++i) {
			m_data[i].~underlying_type();
		}
	}

	/*
	** The generic copy and move constructors and assignment operators
	** implemented for us by array_wrapper are already efficient.
	*/
	dense_storage(const dense_storage&)   = delete;
	dense_storage(dense_storage&&)        = delete;
	auto& operator=(const dense_storage&) = delete;
	auto& operator=(dense_storage&& rhs)  = delete;

	CC_ALWAYS_INLINE constexpr
	auto memory_size() const noexcept
	{ return sizeof(underlying_type) * underlying_size(); }

	template <class... Ts>
	CC_ALWAYS_INLINE
	auto at(const Ts... ts) noexcept
	nd_deduce_return_type(helper::at(
		coords_to_offset::apply(*this, ts...), *this))

	template <class... Ts>
	CC_ALWAYS_INLINE constexpr
	auto at(const Ts... ts) const noexcept
	nd_deduce_return_type(helper::at(
		coords_to_offset::apply(*this, ts...), *this))

	template <class... Ts>
	CC_ALWAYS_INLINE
	auto uninitialized_at(const Ts... ts) noexcept
	nd_deduce_return_type(helper::uninitialized_at(
		coords_to_offset::apply(*this, ts...), *this))

	CC_ALWAYS_INLINE
	auto flat_view() noexcept
	{
		auto func = [&](const auto off, auto& arr) CC_ALWAYS_INLINE
		nd_deduce_noexcept_and_return_type(helper::at(off, arr));

		return make_flat_view<decltype(func)>(*this, size());
	}

	CC_ALWAYS_INLINE
	auto flat_view() const noexcept
	{
		auto func = [&](const auto off, auto& arr) CC_ALWAYS_INLINE
		nd_deduce_noexcept_and_return_type(helper::at(off, arr));

		return make_flat_view<decltype(func)>(*this, size());
	}

	CC_ALWAYS_INLINE
	auto construction_view() noexcept
	{
		/*
		** XXX: Using this results in a linker error. I feel like this
		** is a compiler bug.
		**
		** auto func = [&](const auto off, auto& arr) CC_ALWAYS_INLINE
		** nd_deduce_noexcept_and_return_type(
		** 	helper::uninitialized_at(off, arr));
		**
		** return make_construction_view<decltype(func)>(*this,
		** 	underlying_size());
		*/
		using access = detail::construction_view_access;
		return make_construction_view<access>(*this, underlying_size());
	}

	CC_ALWAYS_INLINE
	auto direct_view() noexcept
	{
		return boost::make_iterator_range(m_data.begin(),
			m_data.end());
	}

	CC_ALWAYS_INLINE
	auto direct_view() const noexcept
	{
		return boost::make_iterator_range(m_data.begin(),
			m_data.end());
	}
private:
	CC_ALWAYS_INLINE
	auto data() noexcept
	{ return m_data.data(); }

	CC_ALWAYS_INLINE constexpr
	auto data() const noexcept
	{ return m_data.data(); }

	CC_ALWAYS_INLINE constexpr
	auto size() const noexcept
	{ return size_type{extents().size()}; }

	CC_ALWAYS_INLINE constexpr
	auto underlying_size() const noexcept
	{ return helper::underlying_size(size()); }
};

template <class T, class Extents, class StorageOrder, class Alloc>
class dense_storage final : layout_base<Extents, StorageOrder>
{
public:
	CC_ALWAYS_INLINE constexpr
	static auto dims() noexcept
	{ return Extents::dims(); }
private:
	using base   = layout_base<Extents, StorageOrder>;
	using helper = detail::dense_storage_access<T>;

	friend struct detail::construction_view_access;
	friend struct detail::dense_storage_access<T>;

	using start = std::decay_t<decltype(std::declval<Extents>().start())>;
	using strides = std::decay_t<decltype(std::declval<Extents>().strides())>;

	static_assert(
		start::allows_static_access,
		"Start of range of dense storage must be statically "
		"accessible."
	);
	static_assert(
		strides::allows_static_access,
		"Strides of range of dense storage must be statically "
		"accessible."
	);
	static_assert(
		start{} == sc_index_n<dims(), 0>,
		"Start of range of dense storage must be the zero index."
	);
	static_assert(
		strides{} == sc_index_n<dims(), 1>,
		"Range of dense storage must have unit stride."
	);
public:
	using size_type       = unsigned;
	using value_type      = std::decay_t<T>;
	using underlying_type = typename helper::underlying_type;
	using allocator_type  = mpl::apply<Alloc, underlying_type>;
	static constexpr auto is_lazy = false;

	using base::extents;
	using base::storage_order;
private:
	underlying_type* m_data{nullptr};
	allocator_type m_alloc{};
public:
	CC_ALWAYS_INLINE constexpr
	explicit dense_storage() noexcept {}

	CC_ALWAYS_INLINE
	explicit dense_storage(
		const Extents& e,
		allocator_type alloc = allocator_type{}
	) : base{e}, m_alloc{alloc}
	{
		nd_assert(e.size() > 0, "cannot allocate array of size zero");
		m_data = m_alloc.allocate(underlying_size());

		/*
		** XXX: Calling allocator::construct is technically required in
		** all cases, but in practice, I don't think that omitting it
		** when `T` is trivially constructible does any harm. I don't
		** want to default-construct elements unnecessarily.
		**
		** Note that we need to check `is_trivially_constructible` as
		** well as `is_trivially_copyable`; this is equivalent to
		** checking `is_trivial`.
		*/
		if (!std::is_trivial<underlying_type>::value) {
			for (auto i = size_t{0}; i != underlying_size(); ++i) {
				m_alloc.construct(&m_data[i]);
			}
		}
	}

	template <class U>
	CC_ALWAYS_INLINE
	explicit dense_storage(
		const U& init,
		const Extents& e,
		allocator_type alloc = allocator_type{}
	) : base{e}, m_alloc{alloc}
	{
		nd_assert(e.size() > 0, "cannot allocate array of size zero");

		m_data = m_alloc.allocate(underlying_size());
		for (auto i = size_t{0}; i != underlying_size(); ++i) {
			m_alloc.construct(&m_data[i], init);
		}
	}

	CC_ALWAYS_INLINE
	~dense_storage()
	{
		if (m_data == nullptr) return;

		for (auto i = size_t{0}; i != underlying_size(); ++i) {
			m_alloc.destroy(&m_data[i]);
		}
		m_alloc.deallocate(m_data, underlying_size());
	}

	/*
	** The generic versions of copy construction and assignment implemented
	** for us by array_wrapper are already efficient.
	*/
	dense_storage(const dense_storage&) = delete;
	auto& operator=(const dense_storage&) = delete;

	/*
	** Since we implement move assignment, array_wrapper uses it to
	** implement move construction for us. So there's no need for us to
	** implement move construction explicitly.
	*/
	dense_storage(dense_storage&&) = delete;

	CC_ALWAYS_INLINE
	auto& operator=(dense_storage&& rhs) noexcept
	{
		this->~dense_storage();
		extents(rhs.extents());
		m_data = rhs.m_data;
		rhs.m_data = nullptr;
		return *this;
	}

	template <class U, class Extents_, class StorageOrder_, class Alloc_,
	nd_enable_if((
		std::is_same<value_type, typename U::value_type>::value &&
		std::is_assignable<
			Extents, decltype(std::declval<U>().extents())
		>::value
	))>
	CC_ALWAYS_INLINE
	auto& operator=(dense_storage<U, Extents_, StorageOrder_, Alloc_>&& rhs)
	{
		this->~dense_storage();
		extents(rhs.extents());
		m_data = rhs.m_data;
		rhs.m_data = nullptr;
		return *this;
	}

	CC_ALWAYS_INLINE constexpr
	auto memory_size() const noexcept
	{ return sizeof(underlying_type) * underlying_size(); }

	template <class... Ts>
	CC_ALWAYS_INLINE
	auto at(const Ts... ts) noexcept
	nd_deduce_return_type(helper::at(
		coords_to_offset::apply(*this, ts...), *this))

	template <class... Ts>
	CC_ALWAYS_INLINE
	auto at(const Ts... ts) const noexcept
	nd_deduce_return_type(helper::at(
		coords_to_offset::apply(*this, ts...), *this))

	template <class... Ts>
	CC_ALWAYS_INLINE
	auto uninitialized_at(const Ts... ts) noexcept
	nd_deduce_return_type(helper::uninitialized_at(
		coords_to_offset::apply(*this, ts...), *this))

	CC_ALWAYS_INLINE
	auto flat_view() noexcept
	{
		auto func = [&](const auto off, auto& arr) CC_ALWAYS_INLINE
		nd_deduce_noexcept_and_return_type(helper::at(off, arr));

		return make_flat_view<decltype(func)>(*this, size());
	}

	CC_ALWAYS_INLINE
	auto flat_view() const noexcept
	{
		auto func = [&](const auto off, auto& arr) CC_ALWAYS_INLINE
		nd_deduce_noexcept_and_return_type(helper::at(off, arr));

		return make_flat_view<decltype(func)>(*this, size());
	}

	CC_ALWAYS_INLINE
	auto construction_view() noexcept
	{
		/*
		** XXX: Using this results in a linker error. I feel like this
		** is a compiler bug.
		**
		** auto func = [&](const auto off, auto& arr) CC_ALWAYS_INLINE
		** nd_deduce_noexcept_and_return_type(
		** 	helper::uninitialized_at(off, arr));
		**
		** return make_construction_view<decltype(func)>(*this,
		** 	underlying_size());
		*/
		using access = detail::construction_view_access;
		return make_construction_view<access>(*this, underlying_size());
	}

	CC_ALWAYS_INLINE
	auto direct_view() noexcept
	{
		return boost::make_iterator_range(
			m_data, m_data + underlying_size());
	}

	CC_ALWAYS_INLINE
	auto direct_view() const noexcept
	{
		return boost::make_iterator_range(
			m_data, m_data + underlying_size());
	}

	template <class Extents_, nd_enable_if((
		std::is_assignable<Extents, Extents_>::value))>
	CC_ALWAYS_INLINE
	void destructive_resize(const Extents_& e)
	{
		nd_assert(e.size() > 0, "cannot change array size to zero");

		if (e.size() < extents().size()) {
			auto off = helper::underlying_size(e.size());
			for (auto i = off; i != underlying_size(); ++i) {
				m_alloc.destroy(&m_data[i]);
			}
		}
		else if (e.size() > extents().size()) {
			this->~dense_storage();
			auto new_size = helper::underlying_size(e.size());
			m_data = m_alloc.allocate(new_size, m_data);

			/*
			** XXX: Technically, this branch should always be
			** taken, for reasons discussed earlier.
			*/
			if (!std::is_trivial<underlying_type>::value) {
				for (auto i = size_t{0}; i != new_size; ++i) {
					m_alloc.construct(&m_data[i]);
				}
			}
		}
		extents(e);
	}

	/*
	** TODO: unsafe resize.
	*/
private:
	CC_ALWAYS_INLINE
	auto data() noexcept
	{ return m_data; }

	CC_ALWAYS_INLINE constexpr
	auto data() const noexcept
	{ return m_data; }
	//XXX
public:
	CC_ALWAYS_INLINE
	auto& allocator() noexcept
	{ return m_alloc; }

	CC_ALWAYS_INLINE constexpr
	auto& allocator() const noexcept
	{ return m_alloc; }

	CC_ALWAYS_INLINE constexpr
	auto size() const noexcept
	{ return size_type{extents().size()}; }

	CC_ALWAYS_INLINE constexpr
	auto underlying_size() const noexcept
	{ return helper::underlying_size(size()); }
};

template <class T>
using underlying_type =
typename detail::dense_storage_access<T>::underlying_type;

namespace detail {

template <class T>
struct is_integer_or_coord
{
	static constexpr auto value =
	std::is_integral<T>::value;
};

template <class Coord>
struct is_integer_or_coord<coord_wrapper<Coord>>
{
	static constexpr auto value = true;
};

}

template <class T, class... Ts,
nd_enable_if((mpl::all_true<detail::is_integer_or_coord<Ts>::value...>))>
CC_ALWAYS_INLINE constexpr
auto make_sarray(const Ts... ts)
noexcept(noexcept(
	std::is_nothrow_default_constructible<underlying_type<T>>::value))
{
	using extents       = decltype(extents(ts...));
	using storage_order = decltype(default_storage_order<sizeof...(Ts)>);
	using storage_type  = dense_storage<T, extents, storage_order, void>;
	using array_type    = array_wrapper<storage_type>;
	return array_type{};
}

template <
	class T,
	class Extents,
	class StorageOrder = decltype(default_storage_order<Extents::dims()>),
	// Prevents the wrong overload from being chosen.
	nd_enable_if((
		Extents::dims() == Extents::dims() &&
		StorageOrder::dims() == StorageOrder::dims()
	))
>
CC_ALWAYS_INLINE constexpr
auto make_sarray(
	const Extents&,
	StorageOrder = default_storage_order<Extents::dims()>
)
noexcept(noexcept(
	std::is_nothrow_default_constructible<underlying_type<T>>::value))
{
	using storage_type = dense_storage<T, Extents, StorageOrder, void>;
	using array_type = array_wrapper<storage_type>;
	return array_type{};
}

template <
	class T,
	class U,
	class Extents,
	class StorageOrder = decltype(default_storage_order<Extents::dims()>),
	// Prevents the wrong overload from being chosen.
	nd_enable_if((
		std::is_constructible<underlying_type<T>, const U&>::value &&
		Extents::dims() == Extents::dims() &&
		StorageOrder::dims() == StorageOrder::dims()
	))
>
CC_ALWAYS_INLINE constexpr
auto make_sarray(
	const U& init,
	const Extents&,
	StorageOrder = default_storage_order<Extents::dims()>
)
noexcept(noexcept(
	std::is_nothrow_constructible<underlying_type<T>, const U&>::value))
{
	using storage_type = dense_storage<T, Extents, StorageOrder, void>;
	using array_type = array_wrapper<storage_type>;
	return array_type{init};
}

template <class T, class... Ts,
nd_enable_if((mpl::all_true<detail::is_integer_or_coord<Ts>::value...>))>
CC_ALWAYS_INLINE
auto make_darray(const Ts... ts)
{
	using extents       = decltype(extents(ts...));
	using storage_order = decltype(default_storage_order<sizeof...(Ts)>);
	using allocator     = mpl::quote<std::allocator>;
	using storage_type  = dense_storage<T, extents, storage_order, allocator>;
	using array_type    = array_wrapper<storage_type>;
	return array_type{nd::extents(ts...)};
}

template <
	class T,
	class Alloc = mpl::quote<std::allocator>,
	class Extents,
	class StorageOrder = decltype(default_storage_order<Extents::dims()>),
	// Prevents the wrong overload from being chosen.
	nd_enable_if((
		Extents::dims() == Extents::dims() &&
		StorageOrder::dims() == StorageOrder::dims()
	))
>
CC_ALWAYS_INLINE
auto make_darray(
	const Extents& e,
	const mpl::apply<Alloc, underlying_type<T>>& alloc =
		mpl::apply<Alloc, underlying_type<T>>{},
	StorageOrder = default_storage_order<Extents::dims()>
)
{
	using storage_type = dense_storage<T, Extents, StorageOrder, Alloc>;
	using array_type = array_wrapper<storage_type>;
	return array_type{e, alloc};
}

template <
	class T,
	class U,
	class Alloc = mpl::quote<std::allocator>,
	class Extents,
	class StorageOrder = decltype(default_storage_order<Extents::dims()>),
	// Prevents the wrong overload from being chosen.
	nd_enable_if((
		std::is_constructible<underlying_type<T>, const U&>::value &&
		Extents::dims() == Extents::dims() &&
		StorageOrder::dims() == StorageOrder::dims()
	))
>
CC_ALWAYS_INLINE
auto make_darray(
	const U& init,
	const Extents& e,
	const mpl::apply<Alloc, underlying_type<T>>& alloc =
		mpl::apply<Alloc, underlying_type<T>>{},
	StorageOrder = default_storage_order<Extents::dims()>
)
{
	using storage_type = dense_storage<T, Extents, StorageOrder, Alloc>;
	using array_type = array_wrapper<storage_type>;
	return array_type{init, e, alloc};
}

template <
	class W,
	class T,
	class Extents,
	class StorageOrder = decltype(std::declval<W>().storage_order())
>
CC_ALWAYS_INLINE
auto make_darray(
	const array_wrapper<W>& arr,
	const Extents& e,
	StorageOrder = decltype(arr.storage_order()){}
)
{
	/*
	using array_type = array_wrapper<T>;
	using value_type = typename array_type::value_type;
	using extents = decltype(arr.extents());
	using storage_order = decltype(arr.storage_order());

	using storage_type = dense_storage<
	*/
}

}

#endif
