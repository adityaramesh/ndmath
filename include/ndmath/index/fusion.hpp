/*
** File Name: fusion.hpp
** Author:    Aditya Ramesh
** Date:      01/31/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z1DF79912_DA06_4F77_80FA_DFF3B061329F
#define Z1DF79912_DA06_4F77_80FA_DFF3B061329F

namespace nd {
namespace fusion {

template <class T>
class index_wrapper;

#define nd_define_relational_op(name, symbol)                            \
                                                                         \
namespace detail {                                                       \
                                                                         \
template <unsigned Cur, unsigned Max>                                    \
struct name ## _range_helper                                             \
{                                                                        \
	using next = name ## _range_helper<Cur + 1, Max>;                \
                                                                         \
	template <class W1, class W2>                                    \
	CC_ALWAYS_INLINE constexpr                                       \
	static auto apply(const W1& w1, const W2& w2) noexcept           \
	{                                                                \
		if (!(w1(sc_coord<Cur>) symbol w2(sc_coord<Cur>))) {     \
			return false;                                    \
		}                                                        \
		return next::apply(w1, w2);                              \
	}                                                                \
};                                                                       \
                                                                         \
template <unsigned Max>                                                  \
struct name ## _range_helper<Max, Max>                                   \
{                                                                        \
	template <class W1, class W2>                                    \
	CC_ALWAYS_INLINE constexpr                                       \
	static auto apply(W1, W2) noexcept                               \
	{ return true; }                                                 \
};                                                                       \
                                                                         \
}                                                                        \
                                                                         \
template <class W1, class W2>                                            \
CC_ALWAYS_INLINE constexpr                                               \
auto name ## _range(const W1& w1, const W2& w2) noexcept                 \
{                                                                        \
	if (w1.dims() != w2.dims()) {                                    \
		return false;                                            \
	}                                                                \
                                                                         \
	using coord = std::decay_t<W1>;                                  \
	using helper = detail:: name ## _range_helper<0, coord::dims()>; \
	return helper::apply(w1, w2);                                    \
}

nd_define_relational_op(equal, ==)
nd_define_relational_op(not_equal, !=)
nd_define_relational_op(greater_equal, >=)
nd_define_relational_op(less_equal, <=)
nd_define_relational_op(greater, >)
nd_define_relational_op(less, <)

#undef nd_define_relational_op

namespace detail {

template <unsigned Cur, unsigned Max>
struct prod_helper
{
	using next = prod_helper<Cur + 1, Max>;

	template <class W>
	CC_ALWAYS_INLINE constexpr
	static auto apply(const W& w) noexcept
	{ return w.at_c(sc_coord<Cur>) * next::apply(w); }
};

template <unsigned Max>
struct prod_helper<Max, Max>
{
	template <class W>
	CC_ALWAYS_INLINE constexpr
	static auto apply(const W& w) noexcept
	{ return w.at_c(sc_coord<Max>); }
};

}

template <class W>
CC_ALWAYS_INLINE constexpr
auto prod(const W& w) noexcept
{
	using helper = detail::prod_helper<0, W::dims() - 1>;
	return helper::apply(w);
}

namespace detail {

template <unsigned Cur, unsigned Max>
struct for_each_helper
{
	using next = for_each_helper<Cur + 1, Max>;

	template <class W, class Func>
	CC_ALWAYS_INLINE
	static void apply(const W& w, const Func& f) noexcept
	{
		f(w(sc_coord<Cur>));
		next::apply(w, f);
	}
};

template <unsigned Max>
struct for_each_helper<Max, Max>
{
	template <class W, class Func>
	CC_ALWAYS_INLINE
	static void apply(W, Func) noexcept {}
};

}

template <class W, class Func>
CC_ALWAYS_INLINE
auto for_each(const W& w, const Func& f) noexcept
{
	using coord = std::decay_t<W>;
	using helper = detail::for_each_helper<0, coord::dims()>;
	return helper::apply(w, f);
}

namespace detail {

template <unsigned Cur, unsigned Max>
struct iter_to_index_copy_helper
{
	using next = iter_to_index_copy_helper<Cur + 1, Max>;

	template <class Iterator, class W>
	CC_ALWAYS_INLINE
	static void apply(Iterator f, Iterator l, W& w) noexcept
	{
		if (f == l) return;
		w.at_c(sc_coord<Cur>) = *f;
		next::apply(f + 1, l, w);
	}
};

template <unsigned Max>
struct iter_to_index_copy_helper<Max, Max>
{
	template <class Iterator, class W>
	CC_ALWAYS_INLINE
	static void apply(Iterator, Iterator, W) noexcept {}
};

template <
	unsigned Cur,
	unsigned Last,
	bool SrcStaticallyAccessible,
	bool DstStaticallyAccessible
>
struct index_to_index_copy_helper;

template <unsigned Cur, unsigned Last>
struct index_to_index_copy_helper<Cur, Last, true, true>
{
	template <class W1, class W2>
	CC_ALWAYS_INLINE
	static void apply(const W1& w1, W2& w2) noexcept
	{
		using c1 = std::decay_t<decltype(w1.at_c(sc_coord<Cur>))>;
		using c2 = std::decay_t<decltype(w2.at_c(sc_coord<Cur>))>;
		static_assert(
			c1::value() == c2::value(),
			"Attempt to change a constant extent of an index."
		);

		using n1 = std::decay_t<decltype(w1.at_c(sc_coord<Cur + 1>))>;
		using n2 = std::decay_t<decltype(w2.at_c(sc_coord<Cur + 1>))>;
		using next = index_to_index_copy_helper<
			Cur + 1, Last,
			n1::allows_static_access,
			n2::allows_static_access
		>;
		next::apply(w1, w2);
	}
};

template <unsigned Cur, unsigned Last, bool SrcStaticallyAccessible>
struct index_to_index_copy_helper<Cur, Last, SrcStaticallyAccessible, false>
{
	template <class W1, class W2>
	CC_ALWAYS_INLINE
	static void apply(const W1& w1, W2& w2) noexcept
	{
		w2.at_c(sc_coord<Cur>) = w1.at_c(sc_coord<Cur>);

		using n1 = std::decay_t<decltype(w1.at_c(sc_coord<Cur + 1>))>;
		using n2 = std::decay_t<decltype(w2.at_c(sc_coord<Cur + 1>))>;
		using next = index_to_index_copy_helper<
			Cur + 1, Last,
			n1::allows_static_access,
			n2::allows_static_access
		>;
		next::apply(w1, w2);
	}
};

template <
	unsigned Cur,
	unsigned Last,
	bool SrcStaticallyAccessible,
	bool DstStaticallyAccessible
>
struct index_to_index_copy_helper
{
	static_assert(
		Cur == 0 && false,
		"Attempt to change a constant extent of an index."
	);
};

template <unsigned Last>
struct index_to_index_copy_helper<Last, Last, true, true>
{
	template <class W1, class W2>
	CC_ALWAYS_INLINE
	static void apply(const W1& w1, const W2& w2) noexcept
	{
		using c1 = std::decay_t<decltype(w1.at_c(sc_coord<Last>))>;
		using c2 = std::decay_t<decltype(w2.at_c(sc_coord<Last>))>;
		static_assert(
			c1::value() == c2::value(),
			"Attempt to change a constant extent of an index."
		);
	}
};

template <unsigned Last, bool SrcStaticallyAccessible>
struct index_to_index_copy_helper<Last, Last, SrcStaticallyAccessible, false>
{
	template <class W1, class W2>
	CC_ALWAYS_INLINE
	static void apply(const W1& w1, W2& w2) noexcept
	{ w2.at_c(sc_coord<Last>) = w1.at_c(sc_coord<Last>); }
};

}

template <class Iterator, class W>
CC_ALWAYS_INLINE
auto copy(Iterator f, Iterator l, W& w) noexcept
{
	using helper = detail::iter_to_index_copy_helper<0, W::dims()>;
	return helper::apply(f, l, w);
}

template <class W1, class W2>
CC_ALWAYS_INLINE
auto copy(const W1& w1, W2& w2) noexcept
{
	using t1 = std::decay_t<decltype(w1.at_c(sc_coord<0>))>;
	using t2 = std::decay_t<decltype(w2.at_c(sc_coord<0>))>;
	using helper = detail::index_to_index_copy_helper<
		0, W1::dims() - 1,
		t1::allows_static_access,
		t2::allows_static_access
	>;
	return helper::apply(w1, w2);
}

}}

#endif
