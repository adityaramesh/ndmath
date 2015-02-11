<!--
  ** File Name: README.md
  ** Author:    Aditya Ramesh
  ** Date:      11/23/2014
  ** Contact:   _@adityaramesh.com
-->

# Overview

High-performance, multidimensional arrays in modern C++.

# Immediate TODO

- Finish range module.
  - For 1D ranges, disable the usual `reverse`, `tile`, and other loop
  optimization functions, and set the value of the loop to zero. Disable
  `permute` entirely, since it's useless for 1D ranges.
  - Update `range_iterator.hpp` so that the direction attribute is taken into
  account.
- Replace ccbase mpl parsing with metaparse library; just include metaparse in
ccbase.
  - Replace `parse_natural, parse_ratio, parse_wsv`, etc.
  - Update `ratio_literal.hpp`.
  - Deleted the unused headers.
  - Update `range_literal.hpp`?
- Finish array module.

## Constexpr Notes

- Don't use `CC_CONST` for any function that writes to memory at all, even if
the function just writes to single int member variable.

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

## Array Module

- Consider using the new C++14 named operators (e.g. `<repeat>`) where
appropriate.

- Use the comma operator for doing elementwise operations like Matlab/Numpy.
  - E.g. `a ,< b` should return a functional_array whose element at index i
  indicates whether `a(i) < b(i)`.

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

  	constexpr auto arr = carray(
		1.0 0 0
		0 -1.0 0
		0 0 1.0
	);

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

- arr() switches to elementwise view
- exp(arr).normalize();
  - Make normalize a generic function that applies a reduction operation and
  then a binary operation to each element. By default, it uses + and /.
- syntax for slicing?

- Profile the code thoroughly and apply restrict where appropriate. See here for
notes on restrict: https://gcc.gnu.org/onlinedocs/gcc/Restricted-Pointers.html.

- Come up with the syntax to write loops using generic packets instead of the
same scalar type of the array. This would allow the loop operation to be
expressed using SIMD operations. Combined with the syntax for unrolling loops,
this can become very powerful.

- Support for tensor notation. See this page for more details:
http://eigen.tuxfamily.org/index.php?title=Working_notes_-_Tensor_module.
- Support for reduction and broadcasting operations:
http://eigen.tuxfamily.org/dox/group__TutorialReductionsVisitorsBroadcasting.html.
  - See this SO question for a nice example illustrating how to calculate sample
  covariance:
  http://stackoverflow.com/questions/15138634/eigen-is-there-an-inbuilt-way-to-calculate-sample-covariance/15142446#15142446.
