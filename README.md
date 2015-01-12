<!--
  ** File Name: README.md
  ** Author:    Aditya Ramesh
  ** Date:      11/23/2014
  ** Contact:   _@adityaramesh.com
-->

# Overview

High-performance, multidimensional arrays in modern C++.

# Immediate TODO

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

- Consider making the iterator for arrays derive from the iterator for ranges.

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

- Support ranges in `operator()`, e.g. `a[i, end][c<3>, end - 1][c<2>, end, 2]`
- Use constexpr for arrays to implement compile-time arrays with the following
syntax:

  	using arr = "
		1 2 3
		4 5 6
		7 8 9
	"_array;

- To implement the constexpr versions of the functions for multidimensional
arrays, we can use essentially the same functions we use for non-constexpr
arrays: the bodies of the functions will all use non-constexpr statements
anyway.
- This makes the effort to implement compile-time multidimensional arrays
minimal.
