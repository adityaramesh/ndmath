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
#include <ndmath/array/storage_order.hpp>

namespace nd {
namespace detail {

template <class T>
struct dense_storage_helper
{
	using underlying_type = T;

	template <class SizeType>
	CC_ALWAYS_INLINE
	static auto& at(const SizeType off, T* data) noexcept
	{ return data[off]; }

	template <class SizeType>
	CC_ALWAYS_INLINE
	static auto& at(const SizeType off, const T* data) noexcept
	{ return data[off]; }

	template <class SizeType>
	CC_ALWAYS_INLINE constexpr
	static auto underlying_size(const SizeType n) noexcept
	{ return n; }

	template <class Array>
	CC_ALWAYS_INLINE
	auto& operator()(const typename Array::size_type off, Array& arr)
	const noexcept { return at(off, arr.data()); }
};

template <>
struct dense_storage_helper<bool>
{
	using underlying_type = unsigned;

	template <class SizeType>
	CC_ALWAYS_INLINE
	static auto at(const SizeType off, underlying_type* data)
	noexcept
	{
		using proxy_type = boolean_proxy<underlying_type, SizeType>;
		return proxy_type{data[underlying_offset(off)], off};
	}

	template <class SizeType>
	CC_ALWAYS_INLINE
	static auto at(const SizeType off, const underlying_type* data)
	noexcept
	{
		using proxy_type = boolean_proxy<const underlying_type, SizeType>;
		return proxy_type{data[underlying_offset(off)], off};
	}

	template <class SizeType>
	CC_ALWAYS_INLINE constexpr
	static auto underlying_offset(const SizeType n)
	noexcept { return n / (8 * sizeof(underlying_type)); }

	template <class SizeType>
	CC_ALWAYS_INLINE constexpr
	static auto underlying_size(const SizeType n)
	noexcept
	{
		nd_assert(n != 0, "Array must have nonzero size.");
		return 1 + underlying_offset(n - 1);
		/*
		** This could overflow.
		**
		** return (n + (8 * sizeof(underlying_type)) - 1) /
		** 	(8 * sizeof(underlying_type));
		*/
	}

	template <class Array>
	CC_ALWAYS_INLINE
	auto operator()(const typename Array::size_type off, Array& arr)
	const noexcept { return at(off, arr.data()); }
};

}

/*
** TODO specialization with Allocator = void for static storage
**
** Technical note: Alloc is a metafunction class. Example use:
** `mpl::quote<std::allocator>`.
*/

template <class T, class Extents, class StorageOrder, class Alloc>
class dense_storage final : layout_base<Extents, StorageOrder>
{
public:
	CC_ALWAYS_INLINE constexpr
	static auto dims() noexcept
	{ return Extents::dims(); }
private:
	using base   = layout_base<Extents, StorageOrder>;
	using helper = detail::dense_storage_helper<T>;

	friend struct detail::dense_storage_helper<T>;

	using start = std::decay_t<decltype(std::declval<Extents>().start())>;
	using strides = std::decay_t<decltype(std::declval<Extents>().strides())>;

	static_assert(
		start::allows_static_access,
		"Start of range must be statically accessible."
	);
	static_assert(
		strides::allows_static_access,
		"Strides of range must be statically accessible."
	);
	static_assert(
		start{} == sc_index_n<dims(), 0>,
		"Start of range must be the zero index."
	);
	static_assert(
		strides{} == sc_index_n<dims(), 1>,
		"Range must have unit stride."
	);
public:
	using size_type       = unsigned;
	using value_type      = std::decay_t<T>;
	using underlying_type = typename helper::underlying_type;
	using allocator_type  = mpl::apply<Alloc, underlying_type>;

	static constexpr auto is_view = false;

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
		m_data = m_alloc.allocate(underlying_size());

		/*
		** XXX: Calling allocator::construct is technically required in
		** all cases, but in practice, I don't think that omitting it
		** when T is trivially constructible does any harm. I don't want
		** to default-construct elements unnecessarily.
		*/
		if (!std::is_trivially_constructible<underlying_type>::value) {
			for (auto i = size_t{0}; i != underlying_size(); ++i) {
				m_alloc.construct(&m_data[i]);
			}
		}
	}

	CC_ALWAYS_INLINE
	~dense_storage()
	{
		if (m_data == nullptr) return;

		/*
		** XXX: See comment regarding allocator::construct above.
		*/
		if (!std::is_trivially_destructible<underlying_type>::value) {
			for (auto i = size_t{0}; i != underlying_size(); ++i) {
				m_alloc.destroy(&m_data[i]);
			}
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
		coords_to_offset::apply(*this, ts...), m_data))

	template <class... Ts>
	CC_ALWAYS_INLINE
	auto at(const Ts... ts) const noexcept
	nd_deduce_return_type(helper::at(
		coords_to_offset::apply(*this, ts...), m_data))

	CC_ALWAYS_INLINE
	auto flat_view() noexcept
	{ return make_flat_view<helper>(*this); }

	CC_ALWAYS_INLINE
	auto flat_view() const noexcept
	{ return make_flat_view<helper>(*this); }

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
	void unsafe_resize(const Extents_& e)
	{
		if (e.size() < extents().size()) {
			/*
			** XXX: Technically, this branch should always be
			** taken, for reasons discussed earlier.
			*/
			if (!std::is_trivially_destructible<underlying_type>::value) {
				auto off = helper::underlying_size(e.size());
				for (auto i = off; i != underlying_size(); ++i) {
					m_alloc.destroy(&m_data[i]);
				}
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
			if (!std::is_trivially_constructible<underlying_type>::value) {
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

	CC_ALWAYS_INLINE constexpr
	auto size() const noexcept
	{ return extents().size(); }

	CC_ALWAYS_INLINE constexpr
	auto underlying_size() const noexcept
	{ return helper::underlying_size(size()); }
};

template <class T>
using underlying_type =
typename detail::dense_storage_helper<T>::underlying_type;

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
nd_enable_if((
	mpl::apply<
		mpl::uncurry<mpl::make_nary<mpl::quote<mpl::logical_and>>>,
		mpl::to_types<std::integer_sequence<bool,
			detail::is_integer_or_coord<Ts>::value...
		>>
	>::value
))>
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
	class StorageOrder = decltype(default_storage_order<Extents::dims()>)
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
	return array_type{e};
}

}

#endif
