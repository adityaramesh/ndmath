<!--
  ** File Name: design_decisions.md
  ** Author:    Aditya Ramesh
  ** Date:      01/20/2015
  ** Contact:   _@adityaramesh.com
-->

# Design Decisions

## $[a, b]$ vs $[a, b + 1)$

- This decision has appeared in many forms in many areas of computer science for
a very long time.
- I want to make the syntax for this library as natural as possible.
- These are my opinions based on experiences with using other software:
  - When describing the _shape_ of something, using the form $[start, length)$
  is usually more intuitive, where start and length can be multidimensional
  indices. Examples:
    - Constant range of integers (normally specified using (length, value)).
    - Dimensions of matrix (the count is based on the number of entities in each
    dimension, not the range of the virtual coordinate system imposed on each
    dimension).
  - When describing a _selection_ or _grid_, using the form $[start, end]$ is
  usually more intuitive. Examples:
    - Range of consecutive integers: it is much more natural to say that I want
    the range $[a, b]$ than it is to say that I want the range $[a, b + 1)$ or
    $[a, b + s)$, where $s$ is the step (usually one).
    - Describing a subarray: when we are given an array and wish to select a
    subarray from it, it is usually easier to specify the top-left corner and
    bottom-right corner than it is to specify the top-left corner and describe
    the lengths of the subarray by performing subtraction.
    - Think about making a selection in Word or Photoshop. The important
    information is where we initially clicked down to start the selection and
    where we unclicked to end the selection.
    - Of course, there can be exceptions to this rule, depending on the
    application. But I still feel that this is a good heuristic on which to base
    the interface.

## Loop Optimization

- Four kinds of optimizations that are feasible to implement using standard C++
techniques:
  - Reversal.
  - Permutation.
  - Unrolling.
  - Tiling.
- Techniques like fusion and fission are not possible without using a
preprocessor, since we can't change code that has already been written.
- When all four optimziations are allowed, there is a combinatorial explosion in
the number of template specializations we need to address all possibilities.
- Need to organize helper structs carefully to prevent combinatorial explosion
while still making the code easy for the compiler to inline so that it is
equivalent to hand-optimized loops.
- Some simplifying assumptions:
  - The inner loop over each tile is always unrolled.
  - Remainder loops are always unrolled when possible.
- Both of these assumptions seem reasonable, because unrolling and tiling are
usually only effective when the factors used are relatively small (e.g. 2, 3, 4,
8, 16, etc.). So it seems unlikely that not unrolling the remainder loops would
provide any benefit, and allowing for this possibility would complicate the
interface.
- These were my main concerns when adding support for loop optimization. The
technical details involved are too numerous to explain concisely; look at the
source code if you're interested.

## Three Kinds of Constants

- With the introduction of `constexpr`, there are effectively three kinds of
constants in C++.
  - Objects whose values are accessible statically. I call this `static
  constexpr`. Constexpr alone is not sufficient to get us `static constexpr`,
  because we don't know whether the parameters to a `constexpr` function are
  themselves `constexpr`. `static constexpr` can only be achieved by storing the
  values as template parameters and providing static functions to retrieve them.
  - Objects whose values are `constexpr`. I call this `non-static constexpr`.
  This means that the object is a literal type. The values of the object can't
  be used in TMP (and hence can't be used to perform specialized codegen), but
  we can still perform `constexpr` computations using the object. This is done
  by making a `constexpr` instance of the object.
  - Objects whose values are only known at runtime.
- When performing full unroll for loop optimization, we need to be able to
obtain the coordinates of the range during compile-time.
- The first time I wrote the index interface, I used `enable_if` to make the
member functions static in case the values of the index were accessible
statically, or non-static if the values were not accessible statically.
- I thought this was a good way to deal with the `static constexpr`/`non-static
constexpr` dichotomy.
- But if we do things this way, we would need to use different syntax when
enforce `static constexpr`ness vs `non-static constexpr`ness. The static member
functions would have to be called using scope resolution syntax in contexts that
require the return value to be `constexpr`, whereas the non-static member
functions can only be called using the dot syntax.
- This forces us to double the number of template specializations in the loop
optimization header, which is not acceptable.
- To work around this, I provide two sets of accessor functions for indices and
ranges: one set only grants `non-constexpr access`, while the other set grants
`static constexpr` access (when possible).
- The second set of functions take the coordinates as template parameters rather
than as runtime parameters. To avoid having to write the `template` keyword as a
disambiguator, I made the second set of functions global. Examples:
  - `r.start(coord)` is `non-static constexpr`
  - `start<Coord>(r)` is `static constexpr`
- This still doesn't solve the problem: how do we know whether the return type
is `constexpr` or not?
- Instead of making the global accessor functions return integers, I made them
return `locations` instead. A location has a static variable that indicates
whether or not it is constant.
- With this framework in place, we can avoid having to double the number of
template specializations.
- Upshot: when designing a `constexpr` interface and `static constexpr` access
is a requirement, keep the scenario above in mind to make sure that you don't
run into similar problems due to non-uniform syntax.
