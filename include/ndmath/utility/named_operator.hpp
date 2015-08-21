/*
** File Name: named_operator.hpp
** Author:    Aditya Ramesh
** Date:      08/21/2015
** Contact:   _@adityaramesh.com
**
** Adapted from Konrad Rudolph's implementation [here][named_operator].
**
** [named_operator]: https://github.com/klmr/named-operator
** "Named operators for C++"
**
** Current assumptions and limitations:
** - Does not respect move semantics.
** - RHS is always captured by const reference. The LHS is allowed to be
**   non-const in case the user is using the reflexive variant of the named
**   operator.
*/

#ifndef Z29389FC3_1557_4F1E_8266_0052E7F425A8
#define Z29389FC3_1557_4F1E_8266_0052E7F425A8

namespace nd {

template <class Func>
struct named_operator_wrapper final
{ Func func; };

template <class LHS, class Func>
struct named_operator_lhs
{
	LHS& value;
	Func func;
};

template <class LHS, class Func>
CC_ALWAYS_INLINE
auto operator<(LHS& lhs, const named_operator_wrapper<Func>& rhs)
noexcept { return named_operator_lhs<LHS, Func>{lhs, rhs.func}; }

template <class LHS, class Func>
CC_ALWAYS_INLINE constexpr
auto operator<(const LHS& lhs, const named_operator_wrapper<Func>& rhs)
noexcept { return named_operator_lhs<const LHS, Func>{lhs, rhs.func}; }

template <class LHS, class Func, class RHS>
CC_ALWAYS_INLINE constexpr
auto operator>(const named_operator_lhs<LHS, Func>& lhs, const RHS& rhs)
nd_deduce_noexcept_and_return_type(lhs.func(lhs.value, rhs))

template <class LHS, class Func, class RHS>
CC_ALWAYS_INLINE
auto operator>=(const named_operator_lhs<LHS, Func>& lhs, const RHS& rhs)
nd_deduce_noexcept_and_return_type(lhs.value = lhs.func(lhs.value, rhs))

template <class Func>
CC_ALWAYS_INLINE constexpr
auto make_named_operator(Func&& f) noexcept
{ return named_operator_wrapper<Func>{f}; }

}

#endif
