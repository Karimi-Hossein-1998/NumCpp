#pragma once
#include <cassert>
#include <cstddef>
#include <vector>
#include <iostream>
#include <algorithm>
#include <initializer_list>

template <typename T=double>
struct Vec
{
	std::vector<T> data;
	Vec() = default;
    Vec(const Vec&) = default;
    Vec(Vec&&) noexcept = default;
    explicit Vec (size_t n, T initVal = T{}) : data(n,initVal) {}
	Vec (std::initializer_list<T> list) : data(list) {}
    [[nodiscard]] size_t size() const noexcept {return data.size();}
    [[nodiscard]] bool empty() const noexcept {return data.empty();}
    [[nodiscard]] T* ptr() noexcept {return data.data();}
    [[nodiscard]] const T* ptr() const noexcept {return data.data();}
    inline T& operator[](size_t i) noexcept {assert(i<data.size() && "Vec index out of bounds"); return data[i];}
    inline const T& operator[](size_t i) const noexcept {assert(i<data.size() && "Vec index out of bounds"); return data[i];}
    inline T& operator()(size_t i) noexcept {return (*this)[i];}
    inline const T& operator()(size_t i) const noexcept {return (*this)[i];}

	// operator =
    inline Vec<T>& operator=(const Vec<T>&) = default;
    inline Vec<T>& operator=(Vec<T>&&) noexcept = default;
    template<typename RowProxy> requires requires(RowProxy r){r.rowPtr; r.nCols;}
	inline Vec<T>& operator=(const RowProxy& row) {data.assign(row.rowPtr,row.rowPtr+row.nCols);return *this;}
    inline Vec<T>& operator=(const std::vector<T>& v) {data.assign(v.begin(),v.end());return *this;}
    template<typename otherVec> requires requires(otherVec v){v.data;}
    inline Vec<T>& operator=(const otherVec& v) {data.assign(v.data.begin(),v.data.end());return *this;}
    inline Vec<T>& operator=(std::initializer_list<T> list) {data.assign(list.begin(),list.end()); return *this;}
    inline Vec<T>& operator=(T value) {std::fill(data.begin(),data.end(),value); return *this;}

    // HELPER
    inline void printv() const
    {
        if (data.empty()) {std::cout << "{}\n"; return;}
        std::cout << "{ ";
        for (size_t i=0; i<data.size()-1; ++i) std::cout << data[i] << "\n  ";
        std::cout << data[data.size()-1] << " }\n";
    }

    // EXTEND
    inline void push_back(const T& value) {data.push_back(value);}
    inline void push_back(T&& value) {data.push_back(std::move(value));}
    inline void extend(const std::vector<T>& v) {data.insert(data.end(),v.begin(),v.end());}
    inline void extend(std::vector<T>&& v) {data.insert(data.end(),std::make_move_iterator(v.begin()),std::make_move_iterator(v.end()));}
    inline void extend(const Vec<T>& v) {data.insert(data.end(),v.data.begin(),v.data.end());}
    inline void extend(Vec<T>&& v) {data.insert(data.end(),std::make_move_iterator(v.data.begin()),std::make_move_iterator(v.data.end()));}
    template<typename otherVec> requires requires(otherVec v){v.data;}
    inline void extend(const otherVec<T>& v) {data.insert(data.end(),v.data.begin(),v.data.end());}
    inline void extend(std::initializer_list<T> list) {data.insert(data.end(),list.begin(),list.end());}
    inline void extend(size_t n, T value) {data.insert(data.end(),n,value);}
    template<typename RowProxy> requires requires(RowProxy r){r.rowPtr; r.nCols;}
    inline void extend(const RowProxy& row) {data.insert(data.end(),row.rowPtr,row.rowPtr+row.nCols);}

    // operator+
    inline Vec<T>& operator+(T value){for(size_t i=0; i<data.size(); ++i) data[i]+=value; return *this;}
};

template <typename T = double>
struct Matrix
{
    size_t nRows = 0;
    size_t nCols = 0;
    std::vector<T> data;
    Matrix() = default;
    Matrix(const Matrix&) = default;
    Matrix(Matrix&&) noexcept = default;
    explicit Matrix(size_t nrows, size_t ncols, T initVal = T{}) : nRows(nrows), nCols(ncols), data(nrows*ncols,initVal) {}
    Matrix (std::initializer_list<T> list) : nRows(1), nCols(list.size()), data(list) {}
    Matrix (std::initializer_list<std::initializer_list<T>> lists)
    {
        nRows = lists.size();
        nCols = (nRows>0)?lists.begin()->size():0;
        for (const auto& list : lists)
        {
            nCols = std::max(nCols,list.size());
        }
        data.reserve(nRows*nCols);
        for (const auto& list : lists)
        {
            data.insert(data.end(),list.begin(),list.end());
            if (list.size()<nCols) data.insert(data.end(),nCols-list.size(),static_cast<T>(0));
        }
    }
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
            assert(list.size()<=nCols && "Initializer list size mismatch");
            T* endCopy = std::copy(list.begin(),list.end(),rowPtr);
            std::fill(endCopy,rowPtr+nCols,T{});
            return *this;
        }
        inline RowSubMatrix& operator=(const std::vector<T>& v)
        {
            assert(v.size()<=nCols && "Vec size match matrix column count");
            T* endCopy = std::copy(v.begin(), v.end(), rowPtr);
            std::fill(endCopy,rowPtr+nCols,T{});
            return *this;
        }
        template<typename otherVec> requires requires(otherVec v){v.data;}
        inline RowSubMatrix& operator=(const otherVec& v)
        {
            assert(v.size()<=nCols && "Vec size match matrix column count");
            T* endCopy = std::copy(v.data.begin(), v.data.end(), rowPtr);
            std::fill(endCopy,rowPtr+nCols,T{});
            return *this;
        }
        template<typename RowProxy> requires requires(RowProxy r){r.rowPtr; r.nCols;}
        inline RowSubMatrix& operator=(const RowProxy& r)
        {
            assert(r.nCols<=nCols && "Matrix column count mismatch");
            T* endCopy = std::copy(r.rowPtr, r.rowPtr+r.nCols, rowPtr);
            std::fill(endCopy,rowPtr+nCols,T{});
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
    inline Matrix<T>& operator=(const Matrix<T>&) = default;
    inline Matrix<T>& operator=(Matrix<T>&&) noexcept = default;
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
    inline Matrix<T>& operator=(std::initializer_list<std::initializer_list<T>> lists)
    {
        nRows = lists.size();
        nCols = (nRows>0)?lists.begin()->size():0;
        for (const auto& list : lists)
        {
            nCols = std::max(nCols,list.size());
        }
        data.clear(); data.reserve(nRows*nCols);
        for (const auto& list : lists)
        {
            data.insert(data.end(),list.begin(),list.end());
            if (list.size()<nCols) data.insert(data.end(),nCols-list.size(),static_cast<T>(0));
        }
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

    // HELPER
    inline void printm() const
    {
        if (data.empty()) {std::cout << "{}\n"; return;}
        std::cout << "{ ";
    	for (size_t r=0; r<nRows; ++r)
        {
            r==0?std::cout << "{ " << data[r*nCols] << '\t':std::cout << "  { " << data[r*nCols] << '\t';
            for (size_t c=1; c<nCols-1; ++c)
            {
                std::cout << data[r*nCols+c] << '\t';
            }
            (r==(nRows-1))?std::cout << data[r*nCols+nCols-1] << " } }\n":std::cout << data[r*nCols+nCols-1] << " }\n";
        }
    }
    // EXTEND
    inline void extend_cols(const T& value) {data.insert(data.end(),nCols,value);nRows+=1;}
    inline void extend_cols(const std::vector<T>& v)
    {
        assert(v.size()<=nCols && "Vector size and column counts mismatch"); size_t copyCount = std::min(v.size(),nCols);
        data.insert(data.end(),v.begin(),v.begin()+copyCount);if (copyCount<nCols) data.insert(data.end(),nCols-v.size(),T{});nRows+=1;
    }
    inline void extend_cols(std::vector<T>&& v)
    {
        assert(v.size()<=nCols && "Vector size and column counts mismatch"); size_t copyCount = std::min(v.size(),nCols);
        data.insert(data.end(),std::make_move_iterator(v.begin()),std::make_move_iterator(v.begin()+copyCount));
        if (copyCount<nCols) data.insert(data.end(),nCols-v.size(),T{}); nRows+=1;
    }
    template<typename otherVec> requires requires(otherVec v){v.data;}
    inline void extend_cols(const otherVec& v)
    {
        assert(v.data.size()<=nCols && "Vector size and column counts mismatch"); size_t copyCount = std::min(v.data.size(),nCols);
        data.insert(data.end(),v.data.begin(),v.data.begin()+copyCount); if (copyCount<nCols) data.insert(data.end(),nCols-v.data.size(),T{}); nRows+=1;
    }
    inline void extend_cols(std::initializer_list<T> list)
    {
        assert(list.size()<=nCols && "List size and column count mismatch"); size_t copyCount = std::min(list.size(),nCols);
        data.insert(data.end(),list.begin(),list.begin()+copyCount);if (copyCount<nCols) data.insert(data.end(),nCols-list.size(),T{});nRows+=1;
    }
    inline void extend_cols(std::initializer_list<std::initializer_list<T>> lists) {for (const auto& list : lists) extend_cols(list);}
    inline void extend_cols(size_t n,const T& value) {data.insert(data.end(),n*nCols,value);nRows+=n;}
    template<typename RowProxy> requires requires(RowProxy r){r.rowPtr; r.nCols;}
    inline void extend_cols(const RowProxy& row)
    {
        assert(row.nCols<=nCols && "Column counts mismatch"); size_t copyCount = std::min(row.nCols,nCols);
        data.insert(data.end(),row.rowPtr,row.rowPtr+copyCount); if (copyCount<nCols) data.insert(data.end(),nCols-row.nCols,T{}); nRows+=1;
    }
    template<typename MatProxy> requires requires(MatProxy m){m.data; m.nRows; m.nCols;}
    inline void extend_cols(const MatProxy& mat)
    {
        for (size_t i=0; i<mat.nRows; ++i) extend_cols(m[i]);
    }
};

/******************************
 *  ||\ ||     /\    ||//\\   *
 *  ||\\||    //\\   ||\\//   *
 *  || \||   ///\\\  ||       *
 *          ////\\\\          *
 *         /////\\\\\         *
 *        /////  \\\\\        *
 *       /////    \\\\\       *
 *      /////      \\\\\      *
 *     /////        \\\\\     *
 *    /////          \\\\\    *
 *   ///////////\\\\\\\\\\\   *
 *  ///// NumPy Style. \\\\\  *
 * /////----------------\\\\\ *
 ******************************/
template <typename T=double>
struct NVec
{
    std::vector<T> data;
    NVec() = default;
    NVec(const NVec&) = default;
    NVec(NVec&&) noexcept = default;
    explicit NVec (size_t n, T initVal = T{}) : data(n,initVal) {}
    NVec (std::initializer_list<T> list) : data(list) {}
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
    inline NVec<T>& operator=(const NVec<T>&) = default;
    inline NVec<T>& operator=(NVec<T>&&) noexcept = default;
    template<typename RowProxy> requires requires(RowProxy r){r.rowPtr; r.nCols;}
    inline NVec<T>& operator=(const RowProxy& row) {data.assign(row.rowPtr,row.rowPtr+row.nCols);return *this;}
    inline NVec<T>& operator=(const std::vector<T>& v) {data.assign(v.begin(),v.end());return *this;}
    template<typename otherVec> requires requires(otherVec v){v.data;}
    inline NVec<T>& operator=(const otherVec& v) {data.assign(v.data.begin(),v.data.end());return *this;}
    inline NVec<T>& operator=(std::initializer_list<T> list) {data.assign(list.begin(),list.end()); return *this;}
    inline NVec<T>& operator=(T value) {std::fill(data.begin(),data.end(),value); return *this;}

    // HELPER
    inline void printv() const
    {
        if (data.empty()) {std::cout << "{}\n"; return;}
        std::cout << "{ ";
        for (size_t i=0; i<data.size()-1; ++i) std::cout << data[i] << "\n  ";
        std::cout << data[data.size()-1] << " }\n";
    }

    // EXTEND
    inline void push_back(const T& value) {data.push_back(value);}
    inline void push_back(T&& value) {data.push_back(std::move(value));}
    inline void extend(const std::vector<T>& v) {data.insert(data.end(),v.begin(),v.end());}
    inline void extend(std::vector<T>&& v) {data.insert(data.end(),std::make_move_iterator(v.begin()),std::make_move_iterator(v.end()));}
    inline void extend(const NVec<T>& v) {data.insert(data.end(),v.data.begin(),v.data.end());}
    inline void extend(NVec<T>&& v) {data.insert(data.end(),std::make_move_iterator(v.data.begin()),std::make_move_iterator(v.data.end()));}
    template<typename otherVec> requires requires(otherVec v){v.data;}
    inline void extend(const otherVec& v) {data.insert(data.end(),v.data.begin(),v.data.end());}
    inline void extend(std::initializer_list<T> list) {data.insert(data.end(),list.begin(),list.end());}
    inline void extend(size_t n, T value) {data.insert(data.end(),n,value);}
    template<typename RowProxy> requires requires(RowProxy r){r.rowPtr; r.nCols;}
    inline void extend(const RowProxy& row) {data.insert(data.end(),row.rowPtr,row.rowPtr+row.nCols);}

    // operator+
    inline NVec<T>& operator+(T value){for(size_t i=0; i<data.size(); ++i) data[i]+=value; return *this;}
};

template <typename T = double>
struct NMatrix
{
    size_t nRows = 0;
    size_t nCols = 0;
    std::vector<T> data;
    NMatrix() = default;
    NMatrix(const NMatrix&) = default;
    NMatrix(NMatrix&&) noexcept = default;
    explicit NMatrix(size_t nrows, size_t ncols, T initVal = T{}) : nRows(nrows), nCols(ncols), data(nrows*ncols,initVal) {}
    NMatrix (std::initializer_list<T> list) : nRows(1), nCols(list.size()), data(list) {}
    NMatrix (std::initializer_list<std::initializer_list<T>> lists)
    {
        nRows = lists.size();
        nCols = (nRows>0)?lists.begin()->size():0;
        for (const auto& list : lists)
        {
            nCols = std::max(nCols,list.size());
        }
        data.reserve(nRows*nCols);
        for (const auto& list : lists)
        {
            data.insert(data.end(),list.begin(),list.end());
            if (list.size()<nCols) data.insert(data.end(),nCols-list.size(),static_cast<T>(0));
        }
    }
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
            assert(list.size()<=nCols && "Initializer list size mismatch");
            T* endCopy = std::copy(list.begin(),list.end(),rowPtr);
            std::fill(endCopy,rowPtr+nCols,T{});
            return *this;
        }
        inline RowSubMatrix& operator=(const std::vector<T>& v)
        {
            assert(v.size()<=nCols && "Vec size match matrix column count");
            T* endCopy = std::copy(v.begin(), v.end(), rowPtr);
            std::fill(endCopy,rowPtr+nCols,T{});
            return *this;
        }
        template<typename otherVec> requires requires(otherVec v){v.data;}
        inline RowSubMatrix& operator=(const otherVec& v)
        {
            assert(v.size()<=nCols && "Vec size match matrix column count");
            T* endCopy = std::copy(v.data.begin(), v.data.end(), rowPtr);
            std::fill(endCopy,rowPtr+nCols,T{});
            return *this;
        }
        template<typename RowProxy> requires requires(RowProxy r){r.rowPtr; r.nCols;}
        inline RowSubMatrix& operator=(const RowProxy& r)
        {
            assert(r.nCols<=nCols && "Matrix column count mismatch");
            T* endCopy = std::copy(r.rowPtr, r.rowPtr+r.nCols, rowPtr);
            std::fill(endCopy,rowPtr+nCols,T{});
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
    inline NMatrix<T>& operator=(const NMatrix<T>&) = default;
    inline NMatrix<T>& operator=(NMatrix<T>&&) noexcept = default;
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
    inline NMatrix<T>& operator=(std::initializer_list<std::initializer_list<T>> lists)
    {
        nRows = lists.size();
        nCols = (nRows>0)?lists.begin()->size():0;
        for (const auto& list : lists)
        {
            nCols = std::max(nCols,list.size());
        }
        data.clear(); data.reserve(nRows*nCols);
        for (const auto& list : lists)
        {
            data.insert(data.end(),list.begin(),list.end());
            if (list.size()<nCols) data.insert(data.end(),nCols-list.size(),static_cast<T>(0));
        }
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
        else if (r<-1) r=std::abs(r);
        else if (c<-1) c=std::abs(c);
        assert(r*c==size && "Total size mismatch");
		nRows=static_cast<size_t>(r); nCols=static_cast<size_t>(c);
    }
    inline void resize(size_t r, size_t c)
    {
        if (r*c==data.size()) {nRows=r; nCols=c;}
        else if (r==0 || c==0) {nRows=0; nCols=0; data.clear();}
        else {nRows=r; nCols=c; data.resize(r*c);}
    }

    // HELPER
    inline void printm() const
    {
        if (data.empty()) {std::cout << "{}\n"; return;}
        std::cout << "{ ";
        for (size_t r=0; r<nRows; ++r)
        {
            r==0?std::cout << "{ " << data[r*nCols] << '\t':std::cout << "  { " << data[r*nCols] << '\t';
            for (size_t c=1; c<nCols-1; ++c)
            {
                std::cout << data[r*nCols+c] << '\t';
            }
            (r==(nRows-1))?std::cout << data[r*nCols+nCols-1] << " } }\n":std::cout << data[r*nCols+nCols-1] << " }\n";
        }
    }
    // EXTEND
    inline void extend_cols(const T& value) {data.insert(data.end(),nCols,value);nRows+=1;}
    inline void extend_cols(const std::vector<T>& v)
    {
        assert(v.size()<=nCols && "Vector size and column counts mismatch"); size_t copyCount = std::min(v.size(),nCols);
        data.insert(data.end(),v.begin(),v.begin()+copyCount);if (copyCount<nCols) data.insert(data.end(),nCols-v.size(),T{});nRows+=1;
    }
    inline void extend_cols(std::vector<T>&& v)
    {
        assert(v.size()<=nCols && "Vector size and column counts mismatch"); size_t copyCount = std::min(v.size(),nCols);
        data.insert(data.end(),std::make_move_iterator(v.begin()),std::make_move_iterator(v.begin()+copyCount));
        if (copyCount<nCols) data.insert(data.end(),nCols-v.size(),T{}); nRows+=1;
    }
    template<typename otherVec> requires requires(otherVec v){v.data;}
    inline void extend_cols(const otherVec& v)
    {
        assert(v.data.size()<=nCols && "Vector size and column counts mismatch"); size_t copyCount = std::min(v.data.size(),nCols);
        data.insert(data.end(),v.data.begin(),v.data.begin()+copyCount); if (copyCount<nCols) data.insert(data.end(),nCols-v.data.size(),T{}); nRows+=1;
    }
    inline void extend_cols(std::initializer_list<T> list)
    {
        assert(list.size()<=nCols && "List size and column count mismatch"); size_t copyCount = std::min(list.size(),nCols);
        data.insert(data.end(),list.begin(),list.begin()+copyCount);if (copyCount<nCols) data.insert(data.end(),nCols-list.size(),T{});nRows+=1;
    }
    inline void extend_cols(std::initializer_list<std::initializer_list<T>> lists) {for (const auto& list : lists) extend_cols(list);}
    inline void extend_cols(size_t n,const T& value) {data.insert(data.end(),n*nCols,value);nRows+=n;}
    template<typename RowProxy> requires requires(RowProxy r){r.rowPtr; r.nCols;}
    inline void extend_cols(const RowProxy& row)
    {
        assert(row.nCols<=nCols && "Column counts mismatch"); size_t copyCount = std::min(row.nCols,nCols);
        data.insert(data.end(),row.rowPtr,row.rowPtr+copyCount); if (copyCount<nCols) data.insert(data.end(),nCols-row.nCols,T{}); nRows+=1;
    }
    template<typename MatProxy> requires requires(MatProxy m){m.data; m.nRows; m.nCols;}
    inline void extend_cols(const MatProxy& mat)
    {
        for (size_t i=0; i<mat.nRows; ++i) extend_cols(m[i]);
    }
};
