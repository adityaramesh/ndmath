<!--
  ** File Name: README.md
  ** Author:    Aditya Ramesh
  ** Date:      11/23/2014
  ** Contact:   _@adityaramesh.com
-->

# Overview

High-performance, multidimensional arrays in modern C++.

# Immediate TODO

- Update range module.
  - Update `range_iterator.hpp` so that the direction attribute is taken into
  account.
  - Update `index_wrapper` and `array_wrapper` so that we use the macro decltype
  deducting trick instead of traits classes.
  - Add `range.size()`
- Replace ccbase mpl parsing with metaparse library; just include metaparse in
ccbase.
  - Replace `parse_natural, parse_ratio, parse_wsv`, etc.
  - Update `ratio_literal.hpp`.
  - Deleted the unused headers.
  - Update `range_literal.hpp`?
- Finish array module.

## Array Module

- Consider using the new C++14 named operators (e.g. `<repeat>`) where
appropriate.

### Long-Term TODO

- Come up with the syntax to write loops using generic packets instead of the
same scalar type of the array. This would allow the loop operation to be
expressed using SIMD operations. Combined with the syntax for unrolling loops,
this can become very powerful.
