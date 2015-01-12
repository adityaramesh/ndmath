/*
** File Name: sequence.hpp
** Author:    Aditya Ramesh
** Date:      01/12/2015
** Contact:   _@adityaramesh.com
*/

#ifndef Z42F95A8C_C536_473F_B0A5_C8ACE61A94FD
#define Z42F95A8C_C536_473F_B0A5_C8ACE61A94FD

namespace nd {

template <class... Ts>
struct sequence
{
	static constexpr auto size()
	noexcept { return sizeof...(Ts); }
};

template <class Seq1, class Seq2>
struct cat_impl;

template <class... Ts, class... Us>
struct cat_impl<sequence<Ts...>, sequence<Us...>>
{ using type = sequence<Ts..., Us...>; };

template <class Seq1, class Seq2>
using cat = typename cat_impl<Seq1, Seq2>::type;

template <class Seq1, class T>
struct append_impl;

template <class... Ts, class T>
struct append_impl<sequence<Ts...>, T>
{ using type = sequence<Ts..., T>; };

template <class Seq, class T>
using append = typename append_impl<Seq, T>::type;

}

#endif
