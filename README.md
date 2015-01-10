<!--
  ** File Name: README.md
  ** Author:    Aditya Ramesh
  ** Date:      11/23/2014
  ** Contact:   _@adityaramesh.com
-->

# Overview

High-performance, multidimensional arrays in modern C++.

# Immediate TODO

## Iterator Module

- Syntax for creating a range from an index/indices?

## Array Module

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
