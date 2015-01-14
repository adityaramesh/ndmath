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

## Constexpr Notes

- Expressions should store their arguments by value, but views (e.g. `subarray`)
should store their arguments by reference.
  - This is because expressions are usually created from prvalues of other
  expressions, and the terminal nodes contain references to the concrete types
  in the expression hierarchy. So expressions act as storage for these prvalues.
  - On the other hand, views are created from lvalues, so copying the object
  that is being viewed is wasteful.

- Don't use `CC_CONST` for any function that writes to memory at all, even if
teh function just writes to single int member variable.

- For both the location and index modules, we can get away with only having one
specialization of the wrapper class (I hope), because the constructor of the
wrapped type is always constexpr. But for `dynamic_array`, this is not the case:
it has a non-constexpr constructor, and a nontrivial destructor.
  - This is according to the information here:
  http://stackoverflow.com/questions/21787488/can-have-definition-variable-of-non-literal-type-in-constexpr-function-body-c1.
  - We can solve this problem using the same technique we used earlier: make two
  overloads for each operator: one for constexpr-only arguments, and another
  that accepts non-constexpr arguments.
  - For dynamic arrays, we have the additional problem that the wrapped type
  would implement operations that are unsuitable for the general `array_wrapper`
  interface (e.g. `reshape`, `resize`, etc.). It might make more sense to use
  traditional CRTP (e.g. base-to-derived casting) for dynamic_array and the new
  CRTP (wrapping) for other kinds of arrays.
    - If we do this, can we still use the same expressions for non-constexpr
    arrays that we use for constexpr arrays? E.g. will `elemwise_array_expr` be
    compatibile with `dynamic_array` since `operator()` must be `constexpr` for
    the former?

## Range Module

### Long-Term TODO

- Use expression templates to define an algebra of loop optimizations for
ranges (e.g. unrolling, tiling, parallelization, etc).
  - The `range_base` class should be responsible for evaluating the for loop
  corresponding to each dimension by calling the appropriate method of its
  derived class.
  - When `for_each` is called on a `range_base` class, a reference to an index
  is passed to the `range_base`, and the for loops are evaluated recursively.
  - How about range fusion (i.e. nested loop fusion)?

## Array Module

- Supported expressions:
  - `array_base` (const and non-const specializations)
  - `constant_array`
  - `array`
  - `subarray` (`const_subarray`, `constexpr_subarray`)
  - `binary_array_expr`
  - `functional_array` (is there a better name?)
  - `mapped_array`

- Auxiliary classes/functions:
  - `array_iterator`

- Support ranges in `operator()`, e.g. `a[i, end][c<3>, end - 1][c<2>, end, 2]`
- Use constexpr for arrays to implement compile-time arrays with the following
syntax:

  	using arr = "
		1 2 3
		4 5 6
		7 8 9
	"_array;

- Consider making the iterator for arrays derive from or encapsulate the
iterator for ranges.

### Immediate TODO

- Notes:
  - Make `extents()` a public member variable called `ext`. This makes the
  syntax much nicer.
  - Subblock syntax for arrays: operator()(idx, idx) specifies the top-left
  corner and extents.
  - The extents of an array should be `index_base`.
  - Shorten names where appropriate, e.g. `dimension => dim`.
  - Comparison operations.
  - Implement arithmetic assignment operators (e.g. `+=`).
  - Decide on a syntax for array slices. Possibly use underscores: `a(0, _, 0, _) = ...`.

### Long-Term TODO

- Come up with the syntax to write loops using generic packets instead of the
same scalar type of the array. This would allow the loop operation to be
expressed using SIMD operations. Combined with the syntax for unrolling loops,
this can become very powerful.
