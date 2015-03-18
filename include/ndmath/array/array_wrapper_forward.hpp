/*
** File Name: array_wrapper_forward.hpp
** Author:    Aditya Ramesh
** Date:      03/17/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z8C11C8FF_C2CA_4ED6_95BA_07725F1CDED9
#define Z8C11C8FF_C2CA_4ED6_95BA_07725F1CDED9

namespace nd {
namespace detail {

/*
** We need to make `IsLiteral` a template parameter, because we cannot
** conditionally make copy and move constructors constexpr. This forces us to
** use specializations.
*/
template <class T, bool IsLiteral>
class array_wrapper_impl;

}

template <class T>
using array_wrapper = detail::array_wrapper_impl<
	T, std::is_literal_type<T>::value
>;

}

#endif
