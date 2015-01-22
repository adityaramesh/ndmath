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
