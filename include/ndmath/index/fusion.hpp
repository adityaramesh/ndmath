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

#define nd_define_relational_op(name, symbol)                                    \
                                                                                 \
namespace detail {                                                               \
                                                                                 \
template <uint_fast32_t Cur, uint_fast32_t Max>                                  \
struct name ## _range_helper                                                     \
{                                                                                \
	using next = name ## _range_helper<Cur + 1, Max>;                        \
                                                                                 \
	template <class W1, class W2>                                            \
	CC_ALWAYS_INLINE constexpr                                               \
	static auto apply(const W1& w1, const W2& w2) noexcept                   \
	{                                                                        \
		using tokens::c;                                                 \
		if (!(w1(c<Cur>) symbol w2(c<Cur>))) {                           \
			return false;                                            \
		}                                                                \
		return next::apply(w1, w2);                                      \
	}                                                                        \
};                                                                               \
                                                                                 \
template <uint_fast32_t Max>                                                     \
struct name ## _range_helper<Max, Max>                                           \
{                                                                                \
	template <class W1, class W2>                                            \
	CC_ALWAYS_INLINE constexpr                                               \
	static auto apply(W1, W2) noexcept                                       \
	{ return true; }                                                         \
};                                                                               \
                                                                                 \
}                                                                                \
                                                                                 \
template <class W1, class W2>                                                    \
CC_ALWAYS_INLINE constexpr                                                       \
auto name ## _range(const W1& w1, const W2& w2) noexcept                         \
{                                                                                \
	if (w1.dims() != w2.dims()) {                                            \
		return false;                                                    \
	}                                                                        \
                                                                                 \
	using coord = std::decay_t<W1>;                                          \
	using helper = detail:: name ## _range_helper<0, coord::dims().value()>; \
	return helper::apply(w1, w2);                                            \
}

nd_define_relational_op(equal, ==)
nd_define_relational_op(not_equal, !=)
nd_define_relational_op(greater_equal, >=)
nd_define_relational_op(less_equal, <=)
nd_define_relational_op(greater, >)
nd_define_relational_op(less, <)

#undef nd_define_relational_op

namespace detail {

template <uint_fast32_t Cur, uint_fast32_t Max>
struct for_each_helper
{
	using next = for_each_helper<Cur + 1, Max>;

	template <class W, class Func>
	CC_ALWAYS_INLINE
	static void apply(const W& w, const Func& f) noexcept
	{
		using tokens::c;
		f(w(c<Cur>));
		next::apply(w, f);
	}
};

template <uint_fast32_t Max>
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
	using helper = detail::for_each_helper<0, coord::dims().value()>;
	return helper::apply(w, f);
}

namespace detail {

template <uint_fast32_t Cur, uint_fast32_t Max>
struct iter_to_index_copy_helper
{
	using next = iter_to_index_copy_helper<Cur + 1, Max>;

	template <class Iterator, class W>
	CC_ALWAYS_INLINE
	static void apply(Iterator f, Iterator l, W& w) noexcept
	{
		using tokens::c;
		if (f == l) return;
		w(c<Cur>) = *f;
		next::apply(f + 1, l, w);
	}
};

template <uint_fast32_t Max>
struct iter_to_index_copy_helper<Max, Max>
{
	template <class Iterator, class W>
	CC_ALWAYS_INLINE
	static void apply(Iterator, Iterator, W) noexcept {}
};

template <uint_fast32_t Cur, uint_fast32_t Max>
struct index_to_index_copy_helper
{
	using next = index_to_index_copy_helper<Cur + 1, Max>;

	template <class W1, class W2>
	CC_ALWAYS_INLINE
	static void apply(W1& w1, W2& w2) noexcept
	{
		using tokens::c;
		w1(c<Cur>) = w2(c<Cur>);
		next::apply(w1, w2);
	}
};

template <uint_fast32_t Max>
struct index_to_index_copy_helper<Max, Max>
{
	template <class W1, class W2>
	CC_ALWAYS_INLINE
	static void apply(W1, W2) noexcept {}
};

}

template <class Iterator, class W>
CC_ALWAYS_INLINE
auto copy(Iterator f, Iterator l, W& w) noexcept
{
	using helper = detail::iter_to_index_copy_helper<0, W::dims().value()>;
	return helper::apply(f, l, w);
}

template <class W1, class W2>
CC_ALWAYS_INLINE
auto copy(W1& w1, W2& w2) noexcept
{
	using helper = detail::index_to_index_copy_helper<0, W1::dims().value()>;
	return helper::apply(w1, w2);
}

}}

#endif
