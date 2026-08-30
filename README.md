# NumPy Style **Vec**tor and **Matrix**

This is an attempt to recreate numpy style vectors and matrices. I'll use a flat `std::vector` to keep the values of data. The `Vec` and `Matrix` proxies are merged into one now!

- [x] Added copy/move constructe and copy/move assignments (using `std::span` and `std::ranges` for data other than a flat--rvalue--`std::vector`).
- [x] Overloaded `operator[]` for usual indexing (`*this[i]` returns a `std::span` **and** `*this[i][j]` *or* `*this[i,j]` returns the *(i,j)*-th elements.)
- [x] Overloaded `operator()` for indexing, arbitrary index can be used `*this(-1,-1000)` it will wrap automatically.
- [x] You can use `(Slice(start,stop,[step]),Slice(start,stop,[step]))` for a strided submatrix (returns a `Matrix<T>`).
- [x] Added `.Transpose()` and `Transpose(Matrix<T>)` functions
- [x] Added utility/info functions (`.Rows`,`.Cols`,`.Shape`,`.size`,`.empty`,`.resize()`,`.reshape()`,`resize(Matrix<T>)`,`reshape(Matrix<T>)`).
- [x] Added arithmetics (+,-,*,/) with a *number* and other *Matrix*.
- [x] Added `AppendRows` and `AppendCols` to expand *Matrices*. Also added `JoinVertical` and `JoinHorizontal` to join to *Matrices*.
- [x] Added `ApplyFunc` to *apply* any function (lambda) to a *Matrix* in a vectorized way, element-wise.
- [x] Added `abs`, `sqrt`, and `power` and some *trig* functions for element-wise application.
- [x] Added *rvalue* overloads for some functionalities.
- [x] Added `Trace` method.
- [x] Added `MatMul` for *Matrix* multiplication (contraction operation).
- [x] Added `GaussJordan` and `GaussJordanInPlace` solvers for $\mathbb{A} \{\mathbb{x}_i\} = \mathbb{b}_i$.
- [ ] Need to add Matrix operations (Matrix multiplications, Trace, Determinant, Factorizations, etc.)

**NOTE:** Use `-std=c++23` and link with *TBB* library (`-ltbb`) to compile.

Feel free to contact me with any *improvement/feature addition* idea or *critiques*.
