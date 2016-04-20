<!--
  ** File Name: README.md
  ** Author:    Aditya Ramesh
  ** Date:      11/23/2014
  ** Contact:   _@adityaramesh.com
-->

# Overview

High-performance, multidimensional arrays in modern C++.

# Immediate TODO

- Read the codebase again and write an overview of the major concepts to make it easier to understand later.
- Revise the codebase:
  - Change the line width to 100; remove headers from files.
  - Think about any simplifications to the overall design (coord, index, array, etc.).
    - Do we really distinctions among all the different types of "constant"?
  - Examine the assembly dump for `range_iterator` again to see if there really is a degradation.

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
  - Delete the unused headers.
  - Update `range_literal.hpp`?
- Finish array module.

- GPU support.
  - Test compilation with Cuda 7.5.
  - Support for GPU allocation and transfer.
  - Support for GPU execution using heterogenerous lambdas.

- Simplifications in the future:
  - Constexpr lambdas (C++17) could be used to greatly simplify the code in many places (e.g. elementwise views).

## Array Module

- Consider using the new C++14 named operators (e.g. `<repeat>`) where
appropriate.

### Long-Term TODO

- Come up with the syntax to write loops using generic packets instead of the
same scalar type of the array. This would allow the loop operation to be
expressed using SIMD operations. Combined with the syntax for unrolling loops,
this can become very powerful.
