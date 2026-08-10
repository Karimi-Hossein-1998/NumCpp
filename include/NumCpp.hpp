#pragma once
#include <cassert>
#include <cstddef>
#include <vector>
#include <cstdio>
#include <algorithm>
#include <initializer_list>

template <typename T=double>
struct Vec
{
	std::vector<T> data;
	Vec() = default;
	explicit Vec (size_t n, T initVal = T{}) : data(n,initVal) {}
    [[nodiscard]] size_t size() const noexcept {return data.size();}
    [[nodiscard]] bool empty() const noexcept {return data.empty();}
    [[nodiscard]] T* ptr() noexcept {return data.data();}
    [[nodiscard]] const T* ptr() const noexcept {return data.data();}
    inline T& operator[](size_t i) noexcept {assert(i<data.size() && "Vec index out of bounds"); return data[i];}
    inline const T& operator[](size_t i) const noexcept {assert(i<data.size() && "Vec index out of bounds"); return data[i];}
    inline T& operator()(size_t i) noexcept {return (*this)[i];}
    inline const T& operator()(size_t i) const noexcept {return (*this)[i];}

	// operator =
	template<typename RowProxy> requires requires(RowProxy r){r.rowPtr; r.nCols;}
	inline Vec<T>& operator=(const RowProxy& row) {data.assign(row.rowPtr,row.rowPtr+row.nCols);return *this;}
    inline Vec<T>& operator=(const std::vector<T>& v) {data.assign(v.begin(),v.end());return *this;}
    template<typename otherVec> requires requires(otherVec v){v.data;}
    inline Vec<T>& operator=(const otherVec& v) {data.assign(v.data.begin(),v.data.end());return *this;}
    inline Vec<T>& operator=(std::initializer_list<T> list) {data.assign(list.begin(),list.end()); return *this;}
    inline Vec<T>& operator=(T value) {std::fill(data.begin(),data.end(),value); return *this;}
};

template <typename T = double>
struct Matrix
{
    size_t nRows = 0;
    size_t nCols = 0;
    std::vector<T> data;
    Matrix() = default;
    explicit Matrix(size_t nrows, size_t ncols, T initVal = T{}) : nRows(nrows), nCols(ncols), data(nrows*ncols,initVal) {}
    [[nodiscard]] size_t rows() const noexcept {return nRows;}
    [[nodiscard]] size_t cols() const noexcept {return nCols;}
    [[nodiscard]] size_t size() const noexcept {return data.size();}
    [[nodiscard]] bool empty() const noexcept {return data.empty();}
    [[nodiscard]] T* ptr() noexcept {return data.data();}
    [[nodiscard]] const T* ptr() const noexcept {return data.data();}

    struct RowSubMatrix
    {
        T* rowPtr;
        size_t nCols;
        inline T& operator[](size_t col) noexcept {assert(col<nCols && "Matrix column out of bounds"); return rowPtr[col];};
        inline RowSubMatrix& operator=(std::initializer_list<T> list)
        {
            assert(list.size()==nCols && "Initializer list size mismatch");
            std::copy(list.begin(),list.end(),rowPtr);
            return *this;
        }
        inline RowSubMatrix& operator=(const std::vector<T>& v)
        {
            assert(v.size()==nCols && "Vec size match matrix column count");
            std::copy(v.begin(), v.end(), rowPtr);
            return *this;
        }
        template<typename otherVec> requires requires(otherVec v){v.data;}
        inline RowSubMatrix& operator=(const otherVec& v)
        {
            assert(v.size()==nCols && "Vec size match matrix column count");
            std::copy(v.data.begin(), v.data.end(), rowPtr);
            return *this;
        }
        template<typename RowProxy> requires requires(RowProxy r){r.rowPtr; r.nCols;}
        inline RowSubMatrix& operator=(const RowProxy& r)
        {
            assert(r.nCols==nCols && "Matrix column count mismatch");
            std::copy(r.rowPtr, r.rowPtr+r.nCols, rowPtr);
            return *this;
        }
        inline RowSubMatrix& operator=(T value) {std::fill(rowPtr,rowPtr+nCols,value); return *this;}
    };
    struct ConstRowSubMatrix
    {
        const T* rowPtr;
        size_t nCols;
        inline const T& operator[](size_t col) const noexcept {assert(col<nCols && "Matrix Column out of bounds"); return rowPtr[col];};
    };
    inline RowSubMatrix operator[](size_t row) noexcept {assert(row<nRows && "Matrix row out of bounds");return RowSubMatrix{&data[row*nCols],nCols};}
    inline ConstRowSubMatrix operator[](size_t row) const noexcept
    {
        assert(row<nRows && "Matrix row out of bounds");
        return ConstRowSubMatrix{&data[row*nCols],nCols};
	}
    inline T& operator[](size_t row, size_t col) noexcept
    {
        assert(row<nRows && "Matrix row out of bounds"); assert(col<nCols && "Matrix column out of bounds");
        return data[row*nCols+col];
    }
    inline const T& operator[](size_t row, size_t col) const noexcept
    {
        assert(row<nRows && "Matrix row out of bounds"); assert(col<nCols && "Matrix column out of bounds");
        return data[row*nCols+col];
    }
    inline T& operator()(size_t row, size_t col) noexcept {return (*this)[row,col];}
    inline const T& operator()(size_t row, size_t col) const noexcept {return (*this)[row,col];}
    // operator =
    template<typename otherMatrix> requires requires(otherMatrix m){m.nRows; m.nCols; m.data;}
    inline Matrix<T>& operator=(const otherMatrix& m) {nRows=m.nRows; nCols=m.nCols; data.assign(m.data.begin(),m.data.end());return *this;}
    inline Matrix<T>& operator=(T value) {std::fill(data.begin(),data.end(),value); return *this;}
    inline Matrix<T>& operator=(const std::vector<std::vector<T>>& vm)
    {
        if (vm.empty() || vm[0].empty()) {nRows=0;nCols=0;data.clear();return *this;}
        nRows=vm.size(); nCols=vm[0].size(); data.resize(nRows*nCols); T* start = data.data();
        for (size_t r=0; r<vm.size(); ++r) {assert(vm[r].size()==nCols && "Column size mismatch"); std::copy_n(vm[r].begin(),nCols,start+(r*nCols));}
        return *this;
    }

    // resize
    inline void reshape(size_t r, size_t c) {assert(r*c==data.size() && "Total size mismatch");nRows=r; nCols=c;}
    inline void resize(size_t r, size_t c)
    {
        if (r*c==data.size()) {nRows=r; nCols=c;}
        else if (r==0 || c==0) {nRows=0; nCols=0; data.clear();}
        else {nRows=r; nCols=c; data.resize(r*c);}
    }
};

template <typename T=double>
struct NVec
{
    std::vector<T> data;
    NVec() = default;
    explicit NVec (size_t n, T initVal = T{}) : data(n,initVal) {}
    [[nodiscard]] size_t size() const noexcept {return data.size();}
    [[nodiscard]] bool empty() const noexcept {return data.empty();}
    [[nodiscard]] T* ptr() noexcept {return data.data();}
    [[nodiscard]] const T* ptr() const noexcept {return data.data();}
    [[nodiscard]] inline size_t wrap_index(std::ptrdiff_t i) const noexcept
    {
        assert(!data.empty() && "Vector is empty");
        std::ptrdiff_t N = data.size();
        std::ptrdiff_t rem = i%N;
        return static_cast<size_t>(rem<0?rem+N:rem);
    }
    inline T& operator[](std::ptrdiff_t i) noexcept {return data[wrap_index(i)];}
    inline const T& operator[](std::ptrdiff_t i) const noexcept {return data[wrap_index(i)];}
    inline T& operator()(std::ptrdiff_t i) noexcept {return (*this)[i];}
    inline const T& operator()(std::ptrdiff_t i) const noexcept {return (*this)[i];}

    // operator =
    template<typename RowProxy> requires requires(RowProxy r){r.rowPtr; r.nCols;}
    inline NVec<T>& operator=(const RowProxy& row) {data.assign(row.rowPtr,row.rowPtr+row.nCols);return *this;}
    inline NVec<T>& operator=(const std::vector<T>& v) {data.assign(v.begin(),v.end());return *this;}
    template<typename otherVec> requires requires(otherVec v){v.data;}
    inline NVec<T>& operator=(const otherVec& v) {data.assign(v.data.begin(),v.data.end());return *this;}
    inline NVec<T>& operator=(std::initializer_list<T> list) {data.assign(list.begin(),list.end()); return *this;}
    inline NVec<T>& operator=(T value) {std::fill(data.begin(),data.end(),value); return *this;}
};

template <typename T = double>
struct NMatrix
{
    size_t nRows = 0;
    size_t nCols = 0;
    std::vector<T> data;
    NMatrix() = default;
    explicit NMatrix(size_t nrows, size_t ncols, T initVal = T{}) : nRows(nrows), nCols(ncols), data(nrows*ncols,initVal) {}
    [[nodiscard]] size_t rows() const noexcept {return nRows;}
    [[nodiscard]] size_t cols() const noexcept {return nCols;}
    [[nodiscard]] size_t size() const noexcept {return data.size();}
    [[nodiscard]] bool empty() const noexcept {return data.empty();}
    [[nodiscard]] T* ptr() noexcept {return data.data();}
    [[nodiscard]] const T* ptr() const noexcept {return data.data();}
    [[nodiscard]] inline size_t wrap_row(std::ptrdiff_t i) const noexcept
    {
        assert(nRows!=0 && "Matrix must not be empty");
        // assert(nCols!=0 && "Matrix rows must not be empty");
        std::ptrdiff_t rem = i%static_cast<std::ptrdiff_t>(nRows);
        return static_cast<size_t>(rem<0?rem+nRows:rem);
    }
    [[nodiscard]] inline size_t wrap_col(std::ptrdiff_t i) const noexcept
    {
        // assert(nRows!=0 && "Matrix must not be empty");
        assert(nCols!=0 && "Matrix rows must not be empty");
        std::ptrdiff_t rem = i%static_cast<std::ptrdiff_t>(nCols);
        return static_cast<size_t>(rem<0?rem+nCols:rem);
    }

    struct RowSubMatrix
    {
        T* rowPtr;
        size_t nCols;
        [[nodiscard]] inline size_t wrap_col(std::ptrdiff_t i) const noexcept
        {
            // assert(nRows!=0 && "Matrix must not be empty");
            assert(nCols!=0 && "Matrix rows must not be empty");
            std::ptrdiff_t rem = i%static_cast<std::ptrdiff_t>(nCols);
            return static_cast<size_t>(rem<0?rem+nCols:rem);
        }
        inline T& operator[](std::ptrdiff_t col) noexcept {return rowPtr[wrap_col(col)];};
        inline RowSubMatrix& operator=(std::initializer_list<T> list)
        {
            assert(list.size()==nCols && "Initializer list size mismatch");
            std::copy(list.begin(),list.end(),rowPtr);
            return *this;
        }
        inline RowSubMatrix& operator=(const std::vector<T>& v)
        {
            assert(v.size()==nCols && "Vec size match matrix column count");
            std::copy(v.begin(), v.end(), rowPtr);
            return *this;
        }
        template<typename otherVec> requires requires(otherVec v){v.data;}
        inline RowSubMatrix& operator=(const otherVec& v)
        {
            assert(v.size()==nCols && "Vec size match matrix column count");
            std::copy(v.data.begin(), v.data.end(), rowPtr);
            return *this;
        }
        template<typename RowProxy> requires requires(RowProxy r){r.rowPtr; r.nCols;}
        inline RowSubMatrix& operator=(const RowProxy& r)
        {
            assert(r.nCols==nCols && "Matrix column count mismatch");
            std::copy(r.rowPtr, r.rowPtr+r.nCols, rowPtr);
            return *this;
        }
        inline RowSubMatrix& operator=(T value) {std::fill(rowPtr,rowPtr+nCols,value); return *this;}
    };
    struct ConstRowSubMatrix
    {
        const T* rowPtr;
        size_t nCols;
        [[nodiscard]] inline size_t wrap_col(std::ptrdiff_t i) const noexcept
        {
            // assert(nRows!=0 && "Matrix must not be empty");
            assert(nCols!=0 && "Matrix rows must not be empty");
            std::ptrdiff_t rem = i%static_cast<std::ptrdiff_t>(nCols);
            return static_cast<size_t>(rem<0?rem+nCols:rem);
        }
        inline const T& operator[](std::ptrdiff_t col) const noexcept {return rowPtr[wrap_col(col)];};
    };
    inline RowSubMatrix operator[](std::ptrdiff_t row) noexcept {return RowSubMatrix{&data[wrap_row(row)*nCols],nCols};}
    inline ConstRowSubMatrix operator[](std::ptrdiff_t row) const noexcept  {return ConstRowSubMatrix{&data[wrap_row(row)*nCols],nCols};}
    inline T& operator [](std::ptrdiff_t row, std::ptrdiff_t col) noexcept {return data[wrap_row(row)*nCols+wrap_col(col)];}
    inline const T& operator [](std::ptrdiff_t row, std::ptrdiff_t col) const noexcept {return data[wrap_row(row)*nCols+wrap_col(col)];}
    inline T& operator()(std::ptrdiff_t row, std::ptrdiff_t col) noexcept {return (*this)[row,col];}
    inline const T& operator()(std::ptrdiff_t row, std::ptrdiff_t col) const noexcept {return (*this)[row,col];}
	// operator =
	template<typename otherMatrix> requires requires(otherMatrix m){m.nRows; m.nCols; m.data;}
	inline NMatrix<T>& operator=(const otherMatrix& m) {nRows=m.nRows; nCols=m.nCols; data.assign(m.data.begin(),m.data.end());return *this;}
	inline NMatrix<T>& operator=(T value) {std::fill(data.begin(),data.end(),value); return *this;}
	inline NMatrix<T>& operator=(const std::vector<std::vector<T>>& vm)
    {
        if (vm.empty() || vm[0].empty()) {nRows=0;nCols=0;data.clear();return *this;}
        nRows=vm.size(); nCols=vm[0].size(); data.resize(nRows*nCols); T* start = data.data();
        for (size_t r=0; r<vm.size(); ++r) {assert(vm[r].size()==nCols && "Column size mismatch"); std::copy_n(vm[r].begin(),nCols,start+(r*nCols));}
        return *this;
    }

    // resize
    inline void reshape(std::ptrdiff_t r, std::ptrdiff_t c)
    {
        std::ptrdiff_t size = static_cast<std::ptrdiff_t>(data.size());
        if (r==-1 && c==-1) assert(false && "Cannot infer both dimensions with -1");
        else if (r==-1)
        {
            assert(c>0 && "Column count must be positive for row count is -1");
            assert(size%c==0 && "Total element count is NOT divisible by requested column count");
            r = size / c;
        }
        else if (c==-1)
        {
            assert(r>0 && "Row count must be positive for column count is -1");
            assert(size%r==0 && "Total element count is NOT divisible by requested row count");
            c = size / r;
        }
        assert(r*c==size && "Total size mismatch");
		nRows=static_cast<size_t>(r); nCols=static_cast<size_t>(c);
    }
    inline void resize(size_t r, size_t c)
    {
        if (r*c==data.size()) {nRows=r; nCols=c;}
        else if (r==0 || c==0) {nRows=0; nCols=0; data.clear();}
        else {nRows=r; nCols=c; data.resize(r*c);}
    }
};
