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
#include <ndmath/array/array_view.hpp>

namespace nd {
namespace detail {

template <class T>
struct dense_storage_helper
{
	using underlying_type = T;

	template <class SizeType>
	CC_ALWAYS_INLINE
	static auto& get(const SizeType n, T* arr) noexcept
	{ return arr[n]; }

	template <class SizeType>
	CC_ALWAYS_INLINE
	static auto& get(const SizeType n, const T* arr) noexcept
	{ return arr[n]; }

	template <class SizeType>
	CC_ALWAYS_INLINE constexpr
	static auto underlying_offset(const SizeType n) noexcept
	{ return n; }
};

template <>
struct dense_storage_helper<bool>
{
	using underlying_type = unsigned;

	template <class SizeType>
	CC_ALWAYS_INLINE
	static auto get(const SizeType n, underlying_type* arr)
	noexcept
	{
		using proxy_type = boolean_proxy<underlying_type, SizeType>;
		return proxy_type{arr[underlying_offset(n)], n};
	}

	template <class SizeType>
	CC_ALWAYS_INLINE
	static auto get(const SizeType n, const underlying_type* arr)
	noexcept
	{
		using proxy_type = boolean_proxy<const underlying_type, SizeType>;
		return proxy_type{arr[underlying_offset(n)], n};
	}

	template <class SizeType>
	CC_ALWAYS_INLINE constexpr
	static auto underlying_offset(const SizeType n)
	noexcept { return n / (8 * sizeof(underlying_type)); }
};

}

/*
** TODO specialization with Allocator = void for static storage
** NOTE: Alloc is a metafunction class. Example use:
** - mpl::quote<std::alloc>
** write note about assumption that allocator is stateless
*/

template <class T, class Extents, class StorageOrder, class Alloc>
class dense_storage final : layout_base<Extents, StorageOrder>
{
private:
	using base   = layout_base<Extents, StorageOrder>;
	using helper = detail::dense_storage_helper<T>;
public:
	using value_type      = std::decay_t<T>;
	using underlying_type = typename helper::underlying_type;
	using allocator_type  = mpl::apply<Alloc, underlying_type>;
private:
	underlying_type* m_data{nullptr};
	allocator_type m_alloc{};

	using base::extents;
	using base::storage_order;
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
		if (m_data == nullptr) return

		/*
		** XXX: See comment regarding allocator::construct above.
		*/
		if (!std::is_trivially_destructible<underlying_type>::value) {
			for (auto i = size_t{0}; i != underlying_size(); ++i) {
				m_alloc.destroy(&m_data[i]);
			}
		}
		alloc.deallocate(m_data, underlying_size());
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
		~dense_storage();
		extents(rhs.extents());
		m_data = rhs.m_data;
		rhs.m_data = nullptr;
		return *this;
	}

	template <class U, nd_enable_if((
		std::is_same<value_type, typename U::value_type>::value &&
		std::is_assignable<
			Extents, decltype(std::declval<U>().extents())
		>::value
	))>
	CC_ALWAYS_INLINE
	auto& operator=(dense_storage<U>&& rhs)
	{
		~dense_storage();
		extents(rhs.extents());
		m_data = rhs.m_data;
		rhs.m_data = nullptr;
		return *this;
	}

	template <class... Ts>
	CC_ALWAYS_INLINE
	auto& get(const Ts... ts) noexcept
	{
		return helper::get(
			coords_to_offset::apply(*this, ts...),
			m_data
		);
	}

	template <class... Ts>
	CC_ALWAYS_INLINE
	auto& get(const Ts... ts) const noexcept
	{
		return helper::get(
			coords_to_offset::apply(*this, ts...),
			m_data
		);
	}

	/*
	** TODO flat_view, direct_view
	**
	** For flat_view, just return a specialization of flat_iterator that
	** uses `coords_to_offset` with the offset. We should probably create a
	** private method that does this, and make `get` call it.
	**
	** For direct_view, use another method of the helper class that returns
	** the underlying element instead of the value_type.
	*/

	template <class Extents_, nd_enable_if((
		std::is_assignable<Extents, Extents_>::value))>
	CC_ALWAYS_INLINE
	void unsafe_resize(const Extents_& e)
	{
		if (e.size() < extents().size()) {
			/*
			** XXX: This code is technically invalid for reasons
			** discussed earlier.
			*/
			if (!std::is_trivially_destructible<underlying_type>::value) {
				auto off = helper::underlying_size(e.size());
				for (auto i = off; i != underlying_size(); ++i) {
					m_alloc.destroy(&m_data[i]);
				}
			}
		}
		else if (e.size() > extents().size()) {
			~dense_storage();

			auto new_size = helper::underlying_size(e.size());
			m_data = m_alloc.allocate(new_size, m_data);

			/*
			** XXX: This code is technically invalid for reasons
			** discussed earlier.
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
	CC_ALWAYS_INLINE constexpr
	auto size() const noexcept
	{ return extents().size(); }

	CC_ALWAYS_INLINE constexpr
	auto underlying_size() const noexcept
	{ return helper::underlying_size(size()); }
};

}

#endif
