/*
** File Name: elemwise_expr.hpp
** Author:    Aditya Ramesh
** Date:      08/15/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z485491EA_9715_4B7F_973C_58E0EA5942C8
#define Z485491EA_9715_4B7F_973C_58E0EA5942C8

namespace nd {

template <class T, class U, class Func>
struct elemwise_expr final
{
private:
	using const_ref_1 = typename T::const_reference;
	using const_ref_2 = typename U::const_reference;
	using size_type_1 = typename T::size_type;
	using size_type_2 = typename U::size_type;
	using extents_1   = std::decay_t<decltype(std::declval<T>().extents())>;
	using extents_2   = std::decay_t<decltype(std::declval<U>().extents())>;

	static constexpr auto sa_1 = extents_1::allows_static_access;
	static constexpr auto sa_2 = extents_2::allows_static_access;
public:
	using exterior_type = std::result_of_t<Func(const_ref_1, const_ref_2)>;
	using size_type     = std::common_type_t<size_type_1, size_type_2>;
	using value_type    = std::decay_t<exterior_type>;
	static constexpr auto is_lazy = true;
private:
	const T& m_left;
	const U& m_right;
	const Func& m_func;
public:
	CC_ALWAYS_INLINE constexpr
	explicit elemwise_expr(const T& t, const U& u, const Func& f)
	noexcept : m_left{t}, m_right{u}, m_func{f} {}

	CC_ALWAYS_INLINE constexpr
	auto memory_size() const noexcept
	{ return size_type{}; }

	template <class... Ts>
	CC_ALWAYS_INLINE constexpr
	decltype(auto) at(const Ts... ts) const noexcept
	{ return m_func(m_left(ts...), m_right(ts...)); }

	/*
	** We should implement flat_view in case both the LHS and the RHS
	** have the same storage order, and support fast flat views.
	*/

	/*
	** TODO: if both LHS and RHS have the same storage order, then we should
	** return that storage order.
	*/
	CC_ALWAYS_INLINE constexpr
	decltype(auto) storage_order() const noexcept
	{ return default_storage_order<std::decay_t<T>::dims()>; }

	template <nd_enable_if(sa_1)>
	CC_ALWAYS_INLINE constexpr
	decltype(auto) extents() const noexcept
	{ return m_left.extents(); }

	template <nd_enable_if((!sa_1 && sa_2))>
	CC_ALWAYS_INLINE constexpr
	decltype(auto) extents() const noexcept
	{ return m_right.extents(); }

	template <nd_enable_if((!sa_1 && !sa_2))>
	CC_ALWAYS_INLINE
	decltype(auto) extents() const noexcept
	{ return m_left.extents(); }
};

template <class T, class U, class Func>
CC_ALWAYS_INLINE constexpr
auto make_elemwise_expr(
	const array_wrapper<T>& t,
	const array_wrapper<U>& u,
	const Func& f
) noexcept
{
	using expr = elemwise_expr<array_wrapper<T>, array_wrapper<U>, Func>;
	using array_type = array_wrapper<expr>;
	return array_type{expr{t, u, f}};
}

}

#endif
