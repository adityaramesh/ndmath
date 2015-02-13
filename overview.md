<!--
  ** File Name: overview.md
  ** Author:    Aditya Ramesh
  ** Date:      02/13/2015
  ** Contact:   _@adityaramesh.com
-->

# Overview of ndmath

## Motivation

Implementing numerical algorithms using high-performance C++ linear algebra
libraries (e.g. Eigen and Blaze) isn't as convenient and enjoyable as it is
using numpy's terse and legible syntax. Neither Eigen nor Blaze has support for
n-dimensional arrays, and workarounds can be frustrating and clumsy. Moreover,
it has now become commonplace for high-performance libraries to implement
the same algorithms twice, once for CPUs in C++ and once for GPUs in CUDA (Torch
for deep learning, OpenCV for computer vision, etc.).

It is now possible to solve many of the problems mentioned in the previous
paragraph. Here are ndmath's goals:
  - Generalize the abstraction patterns used by high-performance C++ linear
  algebra libraries (e.g. Blaze and Eigen) to an arbitrary number of dimensions.
  - Incorporate the syntax of numpy using C++14 features.
  - Integrate a framework for loop nest optimization into the library,
  including:
	- Loop unrolling, tiling, permutation, and parallelizations (allow the
	programmer to write an algorithm once, and select the best loop
	optimization parameters for each architecture).
	- GPU gridification (this allows the same expression to be evaluated
	either on the CPU or the GPU, so the programmer doesn't have to rewrite
	the same algorithm twice).

## Overview

ndmath is organized into four modules:
  - The `coord` module. Implements a powerful system to ensure constant
  propagation of compile-time indices occurs, even throughout very complex
  expressions. Allows the programmer to write things like `arr(end - 5) = 3`.
  - The `index` module. Extends the `coord` module to multiple dimensions.
  - The `range` module. A range is composed of three indices: the start, finish,
  and stride. Ranges encode the information necessary to represented nested
  loops, and allows loop optimizations to be applied to them.
  - The `array` module. Implements numpy's syntax for n-dimensional arrays.

## Examples

### Initialization

	/*
	** This array only exists during compile-time; it occupies no memory!
	** Any computations using this array will cause the corresponding
	** coefficients to be folded directly into the resulting expressions.
	**
	** How this syntax is valid C++14 is a more complicated story...
	*/
	auto m1 = nd_array([[1 2 3] [4 5 6] [7 8 9]])

	// Creates a static array.
	auto m2 = nd::make_array({{1, 2, 3}, {4, 5, 6}, {7, 8, 9}})

### Indexing

	// Select the first column.
	m1(_, 0)
	// Result: [1 4 7]

	// Select the top-left 2x2 submatrix.
	m1(cr<0, 2>, cr<0, 2>)
	// Result: [[1 2] [4 5]]

	// Create a submatrix from the corner elements.

	// Method 1: using strides.
	m1(cr<0, 2, 2>, cr<0, 2, 2>)
	// Result: [[1 3] [7 9]]

	// Method 2: using numpy's "advanced indexing" concept.
	m1({{0}, {2}}, {{0, 2}})
	// Result: [[1 3] [7 9]]

	// Reductions along rows.

	m1.max(1)
	// Result: [3, 6, 9]
	m1.sum(1)
	// Result: [6, 15, 24]

	// Boolean indexing:

	m1(m1.max(1) <= 6)
	// Result: [[1 2 3], [4 5 6]]

### Performance Optimization

	/*
	** Create a 10000x10000 matrix whose element (i, j) is a
	** uniformly-distributed double in [0, 1] computed on the fly. Note that
	** none of the following three statements actually allocates memory.
	*/
	auto gen  = std::default_random_engine{};
	auto dist = std::uniform_real_distribution<double>{0.0, 1.0};
	auto rand = nd::make_array{[&] (...) { return dist(gen); }, 10000, 10000};

	// Initialize three dynamically-allocated matrices using `rand`.
	auto a = nd::make_darray(rand);
	auto b = nd::make_darray(rand);
	auto c = nd::make_darray(rand);
	auto d = nd::make_darray(10000, 10000);

	// "Unoptimized" computation:
	d = a * b + c;
	// Unroll and tile the resulting loop nest:
	d = (a * b + c).unroll<1, 4>().tile<0, 8>();
	// GPU gridification using 128 threads per block (once CUDA supports
	// host-side C++14).
	d = (a * b + c).gridify<128, 128>();

	/*
	** Note: arbitrary expressions written using this library can be
	** gridified, including those that perform reductions, broadcasting, or
	** use advanced indexing techniques. This allows the user to generate
	** specialized CUDA kernels optimized for performing the tasks at hand.
	** Unrolling, tiling, and permutation can be applied to loops _within_
	** the CUDA kernel, and grid searches can be performed automatically
	** over these parameters to select the kernel with the best speedup.
	*/
