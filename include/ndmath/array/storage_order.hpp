/*
** File Name: storage_order.hpp
** Author:    Aditya Ramesh
** Date:      03/20/2015
** Contact:   _@adityaramesh.com
*/

#ifndef ZD3020EBC_43E7_48C3_8634_51FD184EA373
#define ZD3020EBC_43E7_48C3_8634_51FD184EA373

namespace nd {
namespace detail {

template <class Seq>
struct default_storage_order_helper;

template <class T, T... Ts>
struct default_storage_order_helper<std::integer_sequence<T, Ts...>>
{ static constexpr auto value = basic_sc_index<T, Ts...>; };

}

template <size_t Dims>
static constexpr auto default_storage_order =
detail::default_storage_order_helper<
	mpl::to_values<mpl::range_c<unsigned, 0, Dims - 1>>
>::value;

}

#endif
