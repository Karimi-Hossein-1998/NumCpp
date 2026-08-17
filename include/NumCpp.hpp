
/**********************************************************************************************************************
 *                                                                                                                    *
 *                                                                                                                    *
 *                           \                             /\                             /                           *
 *                            \\                          //\\                          //                            *
 *                             \\\                       ///\\\                       ///                             *
 *                              \\\\                    ////\\\\                    ////                              *
 *                               \\\\\                 /////\\\\\                 /////                               *
 *                                \\\\\\              //////\\\\\\              //////                                *
 *                                 \\\\\\\           ///////\\\\\\\           ///////                                 *
 *                                  \\\\\\\\        ////////\\\\\\\\        ////////                                  *
 *                                   \\\\\\\\\     /////////\\\\\\\\\     /////////                                   *
 *                                    \\\\\\\\\\  //////////\\\\\\\\\\  //////////                                    *
 *                                     \\\\\\\\\\X/////////  \\\\\\\\\X//////////                                     *
 *                                      \\\\\\\\XXX///////    \\\\\\\XXX////////                                      *
 *                                       \\\\\\XXXXX/////      \\\\\XXXXX//////                                       *
 *                                        \\\\XXXXXXX///        \\\XXXXXXX////                                        *
 *                                         \\XXXXXXXXX/          \XXXXXXXXX//                                         *
 *                                          XXXXXXXXXX\          /XXXXXXXXXX                                          *
 *                                         //XXXXXXXX\\\        ///XXXXXXXX\\                                         *
 *                                        ////XXXXXX\\\\\      /////XXXXXX\\\\                                        *
 *                                       //////XXXX\\\\\\\    ///////XXXX\\\\\\                                       *
 *                                      ////////XX\\\\\\\\\  /////////XX\\\\\\\\                                      *
 *                                     //////////\\\\\\\\\\\///////////\\\\\\\\\\                                     *
 *                                    //////////  \\\\\\\\\XX/////////  \\\\\\\\\\                                    *
 *                                   //////////    \\\\\\\XXXX///////    \\\\\\\\\\                                   *
 *                                  //////////      \\\\\XXXXXX/////      \\\\\\\\\\                                  *
 *                                 //////////        \\\XXXXXXXX///        \\\\\\\\\\                                 *
 *                                //////////          \XXXXXXXXXX/          \\\\\\\\\\                                *
 *                               //////////           /XXXXXXXXXX\           \\\\\\\\\\                               *
 *                              //////////           ///XXXXXXXX\\\           \\\\\\\\\\                              *
 *                             //////////           /////XXXXXX\\\\\           \\\\\\\\\\                             *
 *                            //////////           ///////XXXX\\\\\\\           \\\\\\\\\\                            *
 *                           //////////           /////////XX\\\\\\\\\           \\\\\\\\\\                           *
 *                          //////////x          ///////////\\\\\\\\\\\          x\\\\\\\\\\                          *
 *                         //////////x\\        ///////////  \\\\\\\\\\\        //x\\\\\\\\\\                         *
 *                        //////////x\\\\      ///////////    \\\\\\\\\\\      ////x\\\\\\\\\\                        *
 *                       //////////x\\\\\\    ///////////      \\\\\\\\\\\    //////x\\\\\\\\\\                       *
 *                      //////////x\\\\\\\\  ///////////        \\\\\\\\\\\  ////////x\\\\\\\\\\                      *
 *                     //////////x\\\\\\\\\\///////////          \\\\\\\\\\\//////////x\\\\\\\\\\                     *
 *                                                                                                                    *
 *                                                                                                                    *
 **********************************************************************************************************************/

#pragma once
#include <cassert>
#include <cstddef>
#include <stdexcept>
#include <iterator>
#include <utility>
#include <vector>
#include <iostream>
#include <algorithm>
#include <initializer_list>
#include <limits>
#include <stdio.h>

template <typename T>
struct Ranges
{
    T start=0;
    T stop=0;
    T step=1;
    size_t size=0;
    Ranges() = default;
    Ranges(const Ranges&) = default;
    Ranges(Ranges&&) noexcept = default;
    explicit Ranges(T start_, T stop_, T step_) : start(start_), stop(stop_), step(step_)
    {
        if (step==0 || (step>0 && stop<=start) || (step<0 && stop>=start)) size = 0;
        else
        {
            double diff = static_cast<double>(stop-start); double dStep = static_cast<double>(step); double divided = diff/dStep;
            size_t newSize = static_cast<size_t>(divided); double eps=std::max(1e-14,divided*std::numeric_limits<double>::epsilon()*10.0);
            size = (divided-static_cast<double>(newSize))>eps?newSize+1:newSize;
        }
    }
    std::vector<T> range() {std::vector<T> v={}; for (size_t i=0; i<size; ++i) v.push_back(start+i*step); return v;};
    [[nodiscard]] inline T operator[](size_t index) const noexcept {return start+index*step;}
    Ranges<T>& operator=(const Ranges&) = default;
    Ranges<T>& operator=(Ranges&&) noexcept = default;
};

using Slice = Ranges<std::ptrdiff_t>;

template <typename T=double>
struct Vec
{
	std::vector<T> data;
	Vec() = default;
    Vec(const Vec&) = default;
    Vec(Vec&&) noexcept = default;
    template <typename otherVec> requires requires(otherVec v){v.data;} && (!std::same_as<std::decay_t<otherVec>, Vec<T>>)
    Vec(const otherVec& v) : data(v.data.begin(),v.data.end()) {}
    template <typename otherVec> requires requires(otherVec v){v.data;}
    && (!std::same_as<std::decay_t<otherVec>, Vec<T>>) && (!std::is_lvalue_reference_v<otherVec>)
    Vec(otherVec&& v) {data.assign(std::make_move_iterator(v.data.begin()),std::make_move_iterator(v.data.end()));}
    template<typename RowProxy> requires requires(RowProxy r){r.rowPtr; r.nCols;}
    Vec(const RowProxy& r) : data(r.rowPtr,r.rowPtr+r.nCols) {}
    explicit Vec (size_t n, T initVal = T{}) : data(n,initVal) {}
	Vec(std::initializer_list<T> list) : data(list) {}
	Vec (const std::vector<T>& v) : data(v) {}
	Vec (std::vector<T>&& v) : data(std::move(v)) {}
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
    template<typename otherVec> requires requires(otherVec v){v.data;} && (!std::same_as<std::decay_t<otherVec>, Vec>)
    inline Vec<T>& operator=(const otherVec& v) {data.assign(v.data.begin(),v.data.end());return *this;}
    template<typename otherVec> requires requires(otherVec v){v.data;} && (!std::same_as<std::decay_t<otherVec>, Vec>)
    && (!std::same_as<std::decay_t<otherVec>, Vec<T>>) && (!std::is_lvalue_reference_v<otherVec>)
    inline Vec<T>& operator=(otherVec&& v)
    {data.assign(std::make_move_iterator(v.data.begin()),std::make_move_iterator(v.data.end()));return *this;}
    inline Vec<T>& operator=(std::initializer_list<T> list) {data.assign(list.begin(),list.end()); return *this;}
    inline Vec<T>& operator=(T value) {std::fill(data.begin(),data.end(),value); return *this;}

    // HELPER
    inline void printv() const
    {
        if (data.empty()) {std::cout << "{}\n"; return;}
        std::cout << "Vec, Size: " << data.size() << "\n";
        std::cout << "{ ";
        for (size_t i=0; i<data.size()-1; ++i) std::cout << data[i] << ", ";
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
    inline void extend(const otherVec& v) {data.insert(data.end(),v.data.begin(),v.data.end());}
    template<typename otherVec> requires requires(otherVec v){v.data;} && (!std::same_as<std::decay_t<otherVec>, Vec>)
    && (!std::same_as<std::decay_t<otherVec>, Vec<T>>) && (!std::is_lvalue_reference_v<otherVec>)
    inline void extend(otherVec&& v) {data.insert(data.end(),std::make_move_iterator(v.data.begin()),std::make_move_iterator(v.data.end()));}
    inline void extend(std::initializer_list<T> list) {data.insert(data.end(),list.begin(),list.end());}
    inline void extend(size_t n, T value) {data.insert(data.end(),n,value);}
    template<typename RowProxy> requires requires(RowProxy r){r.rowPtr; r.nCols;}
    inline void extend(const RowProxy& row) {data.insert(data.end(),row.rowPtr,row.rowPtr+row.nCols);}
    // operator + - * / += -= *= /= dot
    inline Vec<T>& operator+=(T value){for(size_t i=0; i<data.size(); ++i) data[i]=data[i]+value; return *this;}
    inline Vec<T>& operator-=(T value){for(size_t i=0; i<data.size(); ++i) data[i]=data[i]-value; return *this;}
    inline Vec<T>& operator*=(T value){for(size_t i=0; i<data.size(); ++i) data[i]=data[i]*value; return *this;}
    inline Vec<T>& operator/=(T value){for(size_t i=0; i<data.size(); ++i) data[i]=data[i]/value; return *this;}
    inline Vec<T> operator+(T value) const {Vec v=*this; return v+=value;}
    inline Vec<T> operator-(T value) const {Vec v=*this; return v-=value;}
    inline Vec<T> operator*(T value) const {Vec v=*this; return v*=value;}
    inline Vec<T> operator/(T value) const {Vec v=*this; return v/=value;}
	// unary
    inline Vec<T> operator-() const {Vec<T> result=*this; for (auto& val : result.data) val=-val; return result;}
    inline Vec<T> operator-() && {for (auto& val : data) val=-val; return *this;}
	// binary
	template<typename U>
	inline Vec<T>& operator+=(const Vec<U>& v)
    {
        if (data.size()!=v.data.size()) throw std::runtime_error("Vector size mismatch");
        size_t n=data.size(); for (size_t i=0; i<n; ++i) data[i]=data[i]+static_cast<T>(v.data[i]); return *this;
    }
    template<typename U>
    inline Vec<T>& operator-=(const Vec<U>& v)
    {
        if (data.size()!=v.data.size()) throw std::runtime_error("Vector size mismatch");
        size_t n=data.size(); for (size_t i=0; i<n; ++i) data[i]=data[i]-static_cast<T>(v.data[i]); return *this;
    }
    template<typename U>
    inline Vec<T>& operator*=(const Vec<U>& v)
    {
        if (data.size()!=v.data.size()) throw std::runtime_error("Vector size mismatch");
        size_t n=data.size(); for (size_t i=0; i<n; ++i) data[i]=data[i]*static_cast<T>(v.data[i]); return *this;
    }
    template<typename U>
    inline Vec<T>& operator/=(const Vec<U>& v)
    {
        if (data.size()!=v.data.size()) throw std::runtime_error("Vector size mismatch");
        size_t n=data.size(); for (size_t i=0; i<n; ++i) data[i]=data[i]/static_cast<T>(v.data[i]); return *this;
    }
    // DOT
    template<typename U>
    inline T dot(const Vec<U>& v) const
    {
        if (v.data.size()!=data.size()) throw std::runtime_error("Vector sizes mismatch"); T result=T{};
        for (size_t i=0; i<data.size(); ++i) result+=data[i]*static_cast<T>(v.data[i]); return result;
    }
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
    template<typename otherMatrix> requires requires(otherMatrix m){m.nRows; m.nCols; m.data;}
    Matrix(const otherMatrix& m) : data(m.data.begin(),m.data.end()), nRows(m.nRows), nCols(m.nCols) {}
    template<typename otherMatrix> requires requires(otherMatrix m){m.nRows; m.nCols; m.data;}
    && (!std::same_as<std::decay_t<otherMatrix>, Matrix<T>>) && (!std::is_lvalue_reference_v<otherMatrix>)
    Matrix(otherMatrix&& m) : nRows(m.nRows), nCols(m.nCols)
    {data.assign(std::make_move_iterator(m.data.begin()),std::make_move_iterator(m.data.end()));}
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
            if (list.size()<nCols) data.insert(data.end(),nCols-list.size(),T{});
        }
    }
    Matrix (const std::vector<std::vector<T>>& m)
    {
        nRows = m.size();
        nCols = (nRows>0)?m[0].size():0;
        for (const auto& row : m)
        {
            nCols = std::max(nCols,row.size());
        }
        data.reserve(nRows*nCols);
        for (const auto& row : m)
        {
            data.insert(data.end(),row.begin(),row.end());
            if (row.size()<nCols) data.insert(data.end(),nCols-row.size(),T{});
        }
    }
    Matrix (std::vector<std::vector<T>>&& m)
    {
        nRows = m.size();
        nCols = (nRows>0)?m[0].size():0;
        for (const auto& row : m)
        {
            nCols = std::max(nCols,row.size());
        }
        data.reserve(nRows*nCols);
        for (const auto& row : m)
        {
            data.insert(data.end(),std::make_move_iterator(row.begin()),std::make_move_iterator(row.end()));
            if (row.size()<nCols) data.insert(data.end(),nCols-row.size(),T{});
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
    template<typename otherMatrix> requires requires(otherMatrix m){m.nRows; m.nCols; m.data;}
    && (!std::same_as<std::decay_t<otherMatrix>, Matrix<T>>) && (!std::is_lvalue_reference_v<otherMatrix>)
    inline Matrix<T>& operator=(otherMatrix&& m)
    {nRows=m.nRows;nCols=m.nCols;data.assign(std::make_move_iterator(m.data.begin()),std::make_move_iterator(m.data.end()));return *this;}
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
            if (list.size()<nCols) data.insert(data.end(),nCols-list.size(),T{});
        }
        return *this;
    }

    // resize
    inline void reshape(size_t r, size_t c) {assert(r*c==data.size() && "Total size mismatch"); if (r*c==data.size()){nRows=r; nCols=c;}}
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
        std::cout << "Matrix, Shape: (" << nRows << "," << nCols << ")\n";
        std::cout << "{ ";
    	for (size_t r=0; r<nRows; ++r)
        {
            r==0?std::cout << "{ ":std::cout << "  { ";
            for (size_t c=0; c<nCols-1; ++c)
            {
                std::cout << data[r*nCols+c] << ", ";
            }
            (r==(nRows-1))?std::cout << data[r*nCols+nCols-1] << " } }\n":std::cout << data[r*nCols+nCols-1] << " },\n";
        }
    }
    [[nodiscard]] static Matrix<T> Zeros(size_t row, size_t col=0)
    {
        if (col==0) return Matrix(row,row,T{});
        else return Matrix(row,col,T{});
    }
    [[nodiscard]] static Matrix<T> Ones(size_t row, size_t col=0)
    {
        if (col==0) return Matrix(row,row,static_cast<T>(1));
        else return Matrix(row,col,static_cast<T>(1));
    }
    [[nodiscard]] static Matrix<T> Eye(size_t row, size_t col=0)
    {
        Matrix<T> result = Matrix<T>::Ones(row,col);
        size_t count = std::min(row,result.nCols);
        for (size_t i=0; i<count; ++i) result.data[i*result.nCols+i]=T{};
        return result;
    }
    [[nodiscard]] static Matrix<T> Identity(size_t row, size_t col=0)
    {
        Matrix<T> result = Matrix<T>::Zeros(row,col);
        size_t count = std::min(row,result.nCols);
        for (size_t i=0; i<count; ++i) result.data[i*result.nCols+i]=static_cast<T>(1);
        return result;
    }
    // APPEND/EXTEND
    inline void append_rows(const T& value) {if (nCols==0) return; data.insert(data.end(),nCols,value);nRows+=1;}
    inline void append_cols(const T& value)
    {
        if (nRows==0) return; std::vector<T> newData; newData.reserve(nRows*(nCols+1));
        for (size_t r=0; r<nRows; ++r) {newData.insert(newData.end(),data.begin()+r*nCols,data.begin()+(r+1)*nCols); newData.push_back(value);}
        data=std::move(newData); nCols+=1;
    }
    inline void append_rows(const std::vector<T>& v)
    {
        if (nCols==0) return; assert(v.size()<=nCols && "Vector size and column counts mismatch"); size_t copyCount = std::min(v.size(),nCols);
        data.insert(data.end(),v.begin(),v.begin()+copyCount);if (copyCount<nCols) data.insert(data.end(),nCols-v.size(),T{});nRows+=1;
    }
    inline void append_cols(const std::vector<T>& v)
    {
        if (nRows==0) return; assert(v.size()<=nRows && "Vector size and column counts mismatch");
        size_t copyCount = std::min(v.size(),nRows); std::vector<T> newData; newData.reserve(nRows*(nCols+1));
        for (size_t r=0;r<nRows;++r){newData.insert(newData.end(),data.begin()+r*nCols,data.begin()+(r+1)*nCols);newData.push_back(r<copyCount?v[r]:T{});}
        data=std::move(newData); nCols+=1;
    }
    inline void append_rows(std::vector<T>&& v)
    {
        if (nCols==0) return; assert(v.size()<=nCols && "Vector size and column counts mismatch"); size_t copyCount = std::min(v.size(),nCols);
        data.insert(data.end(),std::make_move_iterator(v.begin()),std::make_move_iterator(v.begin()+copyCount));
        if (copyCount<nCols) data.insert(data.end(),nCols-v.size(),T{}); nRows+=1;
    }
    inline void append_cols(std::vector<T>&& v)
    {
        if (nRows==0) return; assert(v.size()<=nRows && "Vector size and column counts mismatch");
        size_t copyCount = std::min(v.size(),nRows); std::vector<T> newData; newData.reserve(nRows*(nCols+1));
        for (size_t r=0;r<nRows;++r)
        {newData.insert(newData.end(),data.begin()+r*nCols,data.begin()+(r+1)*nCols);newData.push_back(r<copyCount?std::move(v[r]):T{});}
        data=std::move(newData); nCols+=1;
    }
    template<typename otherVec> requires requires(otherVec v){v.data;}
    inline void append_rows(const otherVec& v)
    {
        assert(v.data.size()<=nCols && "Vector size and column counts mismatch"); size_t copyCount = std::min(v.data.size(),nCols);
        data.insert(data.end(),v.data.begin(),v.data.begin()+copyCount); if (copyCount<nCols) data.insert(data.end(),nCols-v.data.size(),T{}); nRows+=1;
    }
    template<typename otherVec> requires requires(otherVec v){v.data;}
    inline void append_cols(const otherVec& v)
    {
        if (nRows==0) return; assert(v.data.size()<=nRows && "Vector size and column counts mismatch");
        size_t copyCount = std::min(v.data.size(),nRows); std::vector<T> newData; newData.reserve(nRows*(nCols+1));
        for (size_t r=0;r<nRows;++r)
        {newData.insert(newData.end(),data.begin()+r*nCols,data.begin()+(r+1)*nCols);newData.push_back(r<copyCount?v.data[r]:T{});}
        data=std::move(newData); nCols+=1;
    }
    template<typename otherVec> requires requires(otherVec v){v.data;}
    inline void append_rows(otherVec&& v)
    {
        assert(v.data.size()<=nCols && "Vector size and column counts mismatch"); size_t copyCount = std::min(v.data.size(),nCols);
        data.insert(data.end(),std::make_move_iterator(v.data.begin()),std::make_move_iterator(v.data.begin()+copyCount));
        if (copyCount<nCols) data.insert(data.end(),nCols-v.data.size(),T{}); nRows+=1;
    }
    template<typename otherVec> requires requires(otherVec v){v.data;}
    inline void append_cols(otherVec&& v)
    {
        if (nRows==0) return; assert(v.data.size()<=nRows && "Vector size and column counts mismatch");
        size_t copyCount = std::min(v.data.size(),nRows); std::vector<T> newData; newData.reserve(nRows*(nCols+1));
        for (size_t r=0;r<nRows;++r)
        {newData.insert(newData.end(),data.begin()+r*nCols,data.begin()+(r+1)*nCols);newData.push_back(r<copyCount?std::move(v.data[r]):T{});}
        data=std::move(newData); nCols+=1;
    }
    inline void append_rows(std::initializer_list<T> list)
    {
        if (nCols==0) return; assert(list.size()<=nCols && "List size and column count mismatch"); size_t copyCount = std::min(list.size(),nCols);
        data.insert(data.end(),list.begin(),list.begin()+copyCount);if (copyCount<nCols) data.insert(data.end(),nCols-list.size(),T{});nRows+=1;
    }
    inline void append_cols(std::initializer_list<T> list)
    {
        if (nRows==0) return; assert(list.size()<=nRows && "Vector size and column counts mismatch");
        size_t copyCount = std::min(list.size(),nRows); std::vector<T> newData; newData.reserve(nRows*(nCols+1));
        for (size_t r=0;r<nRows;++r)
        {newData.insert(newData.end(),data.begin()+r*nCols,data.begin()+(r+1)*nCols);newData.push_back(r<copyCount?*(list.begin()+r):T{});}
        data=std::move(newData); nCols+=1;
    }
    inline void append_rows(std::initializer_list<std::initializer_list<T>> lists)
    {if (nCols==0) return; for (const auto& list : lists) append_rows(list);}
    inline void append_cols(std::initializer_list<std::initializer_list<T>> lists)
    {
        if (nRows==0) return;
        assert(lists.size()<=nRows && "List and row count mismatch"); size_t maxCol=0; size_t copyCount = std::min(nRows,lists.size());
        for (const auto& list : lists) {maxCol=std::max(maxCol,list.size());} std::vector<T> newData; newData.reserve(nRows*(nCols+maxCol)); size_t r=0;
        for (const auto& list : lists)
        {
            if (r>=copyCount) continue;
            newData.insert(newData.end(),data.begin()+r*nCols,data.begin()+(r+1)*nCols);
            newData.insert(newData.end(),list.begin(),list.end());newData.insert(newData.end(),(maxCol-list.size()),T{});
            r+=1;
        }
        if (copyCount<nRows)
        {
            for (size_t r=copyCount; r<nRows; ++r)
            {
                newData.insert(newData.end(),data.begin()+r*nCols,data.begin()+(r+1)*nCols);
                newData.insert(newData.end(),maxCol,T{});
            }
        }
        data=std::move(newData);nCols+=maxCol;
    }
    inline void append_rows(size_t n,const T& value) {data.insert(data.end(),n*nCols,value);nRows+=n;}
    inline void append_cols(size_t n,const T& value)
    {
        if (nRows==0) return; std::vector<T> newData; newData.reserve(nRows*(nCols+n));
        for (size_t r=0; r<nRows; ++r)
        {newData.insert(newData.end(),data.begin()+r*nCols,data.begin()+(r+1)*nCols);newData.insert(newData.end(),n,value);}
        data=std::move(newData); nCols+=n;
    }
    template<typename RowProxy> requires requires(RowProxy r){r.rowPtr; r.nCols;}
    inline void append_rows(const RowProxy& row)
    {
        assert(row.nCols<=nCols && "Column counts mismatch"); size_t copyCount = std::min(row.nCols,nCols);
        data.insert(data.end(),row.rowPtr,row.rowPtr+copyCount); if (copyCount<nCols) data.insert(data.end(),nCols-row.nCols,T{}); nRows+=1;
    }
    template<typename MatProxy> requires requires(MatProxy m){m.data; m.nRows; m.nCols;}
    inline void append_rows(const MatProxy& mat)
    {
        if (nCols==0) return; assert(mat.nCols<=nCols && "Matrix column count mismatch");
        size_t colCount = std::min(mat.nCols,nCols);
        for (size_t r=0; r<mat.nRows; ++r)
        {
            data.insert(data.end(),mat.data.begin()+r*mat.nCols,mat.data.begin()+r*mat.nCols+colCount);
            if (colCount<nCols) data.insert(data.end(),nCols-colCount,T{});
        }
        nRows+=mat.nRows;
    }
    template<typename MatProxy> requires requires(MatProxy m){m.data; m.nRows; m.nCols;}
    inline void append_cols(const MatProxy& mat)
    {
        if (nRows==0) return; assert(mat.nRows<=nRows && "Matrix row count mismatch"); std::vector<T> newData; newData.reserve(nRows*(nCols+mat.nCols));
        size_t copyCount = std::min(nRows,mat.nRows);
        for (size_t r=0; r<nRows; ++r)
        {
            newData.insert(newData.end(),data.begin()+r*nCols,data.begin()+(r+1)*nCols);
            (r>=copyCount)?newData.insert(newData.end(),mat.nCols,T{}):newData.insert(newData.end(),mat.data.begin()+r*mat.nCols,mat.data.begin()+(r+1)*mat.nCols);
        }
        data=std::move(newData);nCols+=mat.nCols;
    }
    // operator + - * / += -= *= /=
    inline Matrix<T>& operator+=(T value){for(size_t i=0; i<data.size(); ++i) data[i]=data[i]+value; return *this;}
    inline Matrix<T>& operator-=(T value){for(size_t i=0; i<data.size(); ++i) data[i]=data[i]-value; return *this;}
    inline Matrix<T>& operator*=(T value){for(size_t i=0; i<data.size(); ++i) data[i]=data[i]*value; return *this;}
    inline Matrix<T>& operator/=(T value){for(size_t i=0; i<data.size(); ++i) data[i]=data[i]/value; return *this;}
    inline Matrix<T> operator+(T value) const {Matrix v=*this; return v+=value;}
    inline Matrix<T> operator-(T value) const {Matrix v=*this; return v-=value;}
    inline Matrix<T> operator*(T value) const {Matrix v=*this; return v*=value;}
    inline Matrix<T> operator/(T value) const {Matrix v=*this; return v/=value;}
    // unary
    inline Matrix<T> operator-() const {Matrix<T> result=*this; for (auto& val : result.data) val=-val; return result;}
    inline Matrix<T> operator-() && {for (auto& val : data) val=-val; return *this;}
    // binary
    template<typename U>
    inline Matrix<T>& operator+=(const Matrix<U>& m)
    {
        if (m.nRows!=nRows || m.nCols!=nCols) throw std::runtime_error("Matrix dimension mismatch");
        for (size_t r=0; r<nRows; ++r) for (size_t c=0; c<nCols; ++c) data[r*nCols+c]=data[r*nCols+c]+static_cast<T>(m.data[r*m.nCols+c]); return *this;
    }
    template<typename U>
    inline Matrix<T>& operator-=(const Matrix<U>& m)
    {
        if (m.nRows!=nRows || m.nCols!=nCols) throw std::runtime_error("Matrix dimension mismatch");
        for (size_t r=0; r<nRows; ++r) for (size_t c=0; c<nCols; ++c) data[r*nCols+c]=data[r*nCols+c]-static_cast<T>(m.data[r*m.nCols+c]); return *this;
    }
    template<typename U>
    inline Matrix<T>& operator*=(const Matrix<U>& m)
    {
        if (m.nRows!=nRows || m.nCols!=nCols) throw std::runtime_error("Matrix dimension mismatch");
        for (size_t r=0; r<nRows; ++r) for (size_t c=0; c<nCols; ++c) data[r*nCols+c]=data[r*nCols+c]*static_cast<T>(m.data[r*m.nCols+c]); return *this;
    }
    template<typename U>
    inline Matrix<T>& operator/=(const Matrix<U>& m)
    {
        if (m.nRows!=nRows || m.nCols!=nCols) throw std::runtime_error("Matrix dimension mismatch");
        for (size_t r=0; r<nRows; ++r) for (size_t c=0; c<nCols; ++c) data[r*nCols+c]=data[r*nCols+c]/static_cast<T>(m.data[r*m.nCols+c]); return *this;
    }
    // Transpose
    inline Matrix<T>& Transpose()
    {
        if (nRows==0 || nCols==0) return *this;
        else if (nRows==1 || nCols==1) {std::swap(nRows,nCols); return *this;}
        else if (nCols==nRows)
        {
            for (size_t r=0; r<nRows; ++r)
                for (size_t c=r+1; c<nCols; ++c)
                    std::swap(data[r*nCols+c],data[c*nRows+r]);
            return *this;
        }
        else
        {
            std::vector<T> tdata(nRows*nCols);
            constexpr size_t BLOCKSIZE = 32;
            for (size_t r0=0; r0<nRows; r0+=BLOCKSIZE)
            {
                size_t rMax = std::min(r0+BLOCKSIZE,nRows);
                for (size_t c0=0; c0<nCols; c0+=BLOCKSIZE)
                {
                    size_t cMax = std::min(c0+BLOCKSIZE,nCols);
                    for (size_t r=r0; r<rMax; ++r)
                        for (size_t c=c0; c<cMax; ++c)
                            tdata[c*nRows+r] = data[r*nCols+c];
                }
            }
            data = std::move(tdata); std::swap(nRows,nCols); return *this;
        }
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
    template<typename otherVec> requires requires(otherVec v){v.data;} && (!std::same_as<std::decay_t<otherVec>,NVec<T>>)
    NVec (const otherVec& v) : data(v.data.begin(),v.data.end()) {}
    template<typename otherVec> requires requires(otherVec v){v.data;}
    && (!std::same_as<std::decay_t<otherVec>,NVec<T>>) && (!std::is_lvalue_reference_v<otherVec>)
    NVec (otherVec&& v) {data.insert(data.end(),std::make_move_iterator(v.data.begin()),std::make_move_iterator(v.data.end()));}
    template<typename RowProxy> requires requires(RowProxy r){r.rowPtr; r.nCols;}
    NVec (const RowProxy& r) : data(r.rowPtr,r.rowPtr+r.nCols) {}
    explicit NVec (size_t n, T initVal = T{}) : data(n,initVal) {}
    NVec (std::initializer_list<T> list) : data(list) {}
    NVec (const std::vector<T>& v) : data(v) {}
    NVec (std::vector<T>&& v) : data(std::move(v)) {}
    [[nodiscard]] size_t size() const noexcept {return data.size();}
    [[nodiscard]] bool empty() const noexcept {return data.empty();}
    [[nodiscard]] T* ptr() noexcept {return data.data();}
    [[nodiscard]] const T* ptr() const noexcept {return data.data();}
    [[nodiscard]] inline size_t wrap_index(std::ptrdiff_t i) const noexcept
    {
        assert(!data.empty() && "Vector is empty");
        std::ptrdiff_t N = data.size();
        if (N>0)
        {
            std::ptrdiff_t rem = i%N;
            return static_cast<size_t>(rem<0?rem+N:rem);
        }
        else {return 0;}
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
    template<typename otherVec> requires requires(otherVec v){v.data;}
    && (!std::same_as<std::decay_t<otherVec>,NVec<T>>) && (!std::is_lvalue_reference_v<otherVec>)
    inline NVec<T>& operator=(otherVec&& v) {data.assign(std::make_move_iterator(v.data.begin()),std::make_move_iterator(v.data.end())); return *this;}
    inline NVec<T>& operator=(std::initializer_list<T> list) {data.assign(list.begin(),list.end()); return *this;}
    inline NVec<T>& operator=(T value) {std::fill(data.begin(),data.end(),value); return *this;}

    // HELPER
    inline void printv() const
    {
        if (data.empty()) {std::cout << "{}\n"; return;}
        std::cout << "NVec, Size: " << data.size() << "\n";
        std::cout << "{ ";
        for (size_t i=0; i<data.size()-1; ++i) std::cout << data[i] << ", ";
        std::cout << data[data.size()-1] << " }\n";
    }
    // SLICE
    inline NVec<T> operator[](Slice index) const noexcept
    {
        NVec<T> result={};
        for (size_t i=0; i<index.size; ++i) result.push_back((*this)[index[i]]);
        return result;
    }
    inline NVec<T> operator()(Slice index) const noexcept {return (*this)[index];}
    // EXTEND
    inline void push_back(const T& value) {data.push_back(value);}
    inline void push_back(T&& value) {data.push_back(std::move(value));}
    inline void extend(const std::vector<T>& v) {data.insert(data.end(),v.begin(),v.end());}
    inline void extend(std::vector<T>&& v) {data.insert(data.end(),std::make_move_iterator(v.begin()),std::make_move_iterator(v.end()));}
    inline void extend(const NVec<T>& v) {data.insert(data.end(),v.data.begin(),v.data.end());}
    inline void extend(NVec<T>&& v) {data.insert(data.end(),std::make_move_iterator(v.data.begin()),std::make_move_iterator(v.data.end()));}
    template<typename otherVec> requires requires(otherVec v){v.data;}
    inline void extend(const otherVec& v) {data.insert(data.end(),v.data.begin(),v.data.end());}
    template<typename otherVec> requires requires(otherVec v){v.data;}
    && (!std::same_as<std::decay_t<otherVec>,NVec<T>>) && (!std::is_lvalue_reference_v<otherVec>)
	inline void extend(otherVec&& v) {data.insert(data.end(),std::make_move_iterator(v.data.begin()),std::make_move_iterator(v.data.end()));}
    inline void extend(std::initializer_list<T> list) {data.insert(data.end(),list.begin(),list.end());}
    inline void extend(size_t n, T value) {data.insert(data.end(),n,value);}
    template<typename RowProxy> requires requires(RowProxy r){r.rowPtr; r.nCols;}
    inline void extend(const RowProxy& row) {data.insert(data.end(),row.rowPtr,row.rowPtr+row.nCols);}

    // operator + - * / += -= *= /= dot
    inline NVec<T>& operator+=(T value){for(size_t i=0; i<data.size(); ++i) data[i]+=value; return *this;}
    inline NVec<T>& operator-=(T value){for(size_t i=0; i<data.size(); ++i) data[i]-=value; return *this;}
    inline NVec<T>& operator*=(T value){for(size_t i=0; i<data.size(); ++i) data[i]*=value; return *this;}
    inline NVec<T>& operator/=(T value){for(size_t i=0; i<data.size(); ++i) data[i]/=value; return *this;}
    inline NVec<T> operator+(T value) const {NVec v=*this; return v+=value;}
    inline NVec<T> operator-(T value) const {NVec v=*this; return v-=value;}
    inline NVec<T> operator*(T value) const {NVec v=*this; return v*=value;}
    inline NVec<T> operator/(T value) const {NVec v=*this; return v/=value;}
    inline T dot(const NVec<T>& v) const {size_t size=std::min(v.size(),data.size()); T result = T{}; for (size_t i=0; i<size; ++i) result+= (v[i]*data[i]); return result;}
    // unary
    inline NVec<T> operator-() const {NVec<T> result=*this; for (auto& val : result.data) val=-val; return result;}
    inline NVec<T> operator-() && {for (auto& val : data) val=-val; return *this;}
    // binary
    template<typename U>
    inline NVec<T>& operator+=(const NVec<U>& v)
    {
        if (v.data.size()!=data.size()) throw std::runtime_error("Vector size mismatch");
        size_t n=data.size(); for (size_t i=0; i<n; ++i) data[i]=data[i]+static_cast<T>(v.data[i]); return *this;
    }
    template<typename U>
    inline NVec<T>& operator-=(const NVec<U>& v)
    {
        if (v.data.size()!=data.size()) throw std::runtime_error("Vector size mismatch");
        size_t n=data.size(); for (size_t i=0; i<n; ++i) data[i]=data[i]-static_cast<T>(v.data[i]); return *this;
    }
    template<typename U>
    inline NVec<T>& operator*=(const NVec<U>& v)
    {
        if (v.data.size()!=data.size()) throw std::runtime_error("Vector size mismatch");
        size_t n=data.size(); for (size_t i=0; i<n; ++i) data[i]=data[i]*static_cast<T>(v.data[i]); return *this;
    }
    template<typename U>
    inline NVec<T>& operator/=(const NVec<U>& v)
    {
        if (v.data.size()!=data.size()) throw std::runtime_error("Vector size mismatch");
        size_t n=data.size(); for (size_t i=0; i<n; ++i) data[i]=data[i]/static_cast<T>(v.data[i]); return *this;
    }
    // DOT
    template<typename U>
    inline T dot(const Vec<U>& v) const
    {
        if (v.data.size()!=data.size()) throw std::runtime_error("Vector sizes mismatch"); T result=T{};
        for (size_t i=0; i<data.size(); ++i) result+=data[i]*static_cast<T>(v.data[i]); return result;
    }
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
    template<typename otherMatrix> requires requires(otherMatrix m){m.nRows; m.nCols; m.data;}
    NMatrix(const otherMatrix& m) : data(m.data.begin(),m.data.end()), nRows(m.nRows), nCols(m.nCols) {}
    template<typename otherMatrix> requires requires(otherMatrix m){m.nRows; m.nCols; m.data;}
    && (!std::same_as<std::decay_t<otherMatrix>,NMatrix<T>>) && (!std::is_lvalue_reference_v<otherMatrix>)
    NMatrix(otherMatrix&& m) {nRows=m.nRows; nCols=m.nCols; data.assign(std::make_move_iterator(m.data.begin()),std::make_move_iterator(m.data.end()));}
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
            if (list.size()<nCols) data.insert(data.end(),nCols-list.size(),T{});
        }
    }
    NMatrix (const std::vector<std::vector<T>>& m)
    {
        nRows = m.size();
        nCols = (nRows>0)?m[0].size():0;
        for (const auto& row : m)
        {
            nCols = std::max(nCols,row.size());
        }
        data.reserve(nRows*nCols);
        for (const auto& row : m)
        {
            data.insert(data.end(),row.begin(),row.end());
            if (row.size()<nCols) data.insert(data.end(),nCols-row.size(),T{});
        }
    }
    NMatrix (std::vector<std::vector<T>>&& m)
    {
        nRows = m.size();
        nCols = (nRows>0)?m[0].size():0;
        for (const auto& row : m)
        {
            nCols = std::max(nCols,row.size());
        }
        data.reserve(nRows*nCols);
        for (const auto& row : m)
        {
            data.insert(data.end(),std::make_move_iterator(row.begin()),std::make_move_iterator(row.end()));
            if (row.size()<nCols) data.insert(data.end(),nCols-row.size(),T{});
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
        if (nRows>0)
        {
            std::ptrdiff_t rem = i%static_cast<std::ptrdiff_t>(nRows);
            return static_cast<size_t>(rem<0?rem+nRows:rem);
        }
        else {return 0;}
    }
    [[nodiscard]] inline size_t wrap_col(std::ptrdiff_t i) const noexcept
    {
        // assert(nRows!=0 && "Matrix must not be empty");
        assert(nCols!=0 && "Matrix rows must not be empty");
        if (nCols>0)
        {
            std::ptrdiff_t rem = i%static_cast<std::ptrdiff_t>(nCols);
            return static_cast<size_t>(rem<0?rem+nCols:rem);
        }
        else {return 0;}
    }

    struct RowSubMatrix
    {
        T* rowPtr;
        size_t nCols;
        [[nodiscard]] inline size_t wrap_col(std::ptrdiff_t i) const noexcept
        {
            // assert(nRows!=0 && "Matrix must not be empty");
            assert(nCols!=0 && "Matrix rows must not be empty");
            if (nCols>0)
            {
                std::ptrdiff_t rem = i%static_cast<std::ptrdiff_t>(nCols);
                return static_cast<size_t>(rem<0?rem+nCols:rem);
            }
            else {return 0;}
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
            if (nCols>0)
            {
                std::ptrdiff_t rem = i%static_cast<std::ptrdiff_t>(nCols);
                return static_cast<size_t>(rem<0?rem+nCols:rem);
            }
            else {return 0;}
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
	template<typename otherMatrix> requires requires(otherMatrix m){m.nRows; m.nCols; m.data;}
	&& (!std::same_as<std::decay_t<otherMatrix>,NMatrix<T>>) && (!std::is_lvalue_reference_v<otherMatrix>)
	inline NMatrix<T>& operator=(otherMatrix&& m)
    {nRows=m.nRows;nCols=m.nCols; data.assign(std::make_move_iterator(m.data.begin()),std::make_move_iterator(m.data.end()));return *this;}
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
            if (list.size()<nCols) data.insert(data.end(),nCols-list.size(),T{});
        }
        return *this;
    }

    // resize
    inline void reshape(std::ptrdiff_t r, std::ptrdiff_t c)
    {
        std::ptrdiff_t size = static_cast<std::ptrdiff_t>(data.size());
        if (r<-1) r=std::abs(r);
        if (c<-1) c=std::abs(c);
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

    // HELPER
    inline void printm() const
    {
        if (data.empty()) {std::cout << "{}\n"; return;}
        std::cout << "NMatrix, Shape: (" << nRows << "," << nCols << ")\n";
        std::cout << "{ ";
        for (size_t r=0; r<nRows; ++r)
        {
            r==0?std::cout << "{ ":std::cout << "  { ";
            for (size_t c=0; c<nCols-1; ++c)
            {
                std::cout << data[r*nCols+c] << ", ";
            }
            (r==(nRows-1))?std::cout << data[r*nCols+nCols-1] << " } }\n":std::cout << data[r*nCols+nCols-1] << " },\n";
        }
    }
    [[nodiscard]] static NMatrix<T> Zeros(size_t row, size_t col=0)
    {
        if (col==0) return NMatrix(row,row,T{});
        else return NMatrix(row,col,T{});
    }
    [[nodiscard]] static NMatrix<T> Ones(size_t row, size_t col=0)
    {
        if (col==0) return NMatrix(row,row,static_cast<T>(1));
        else return NMatrix(row,col,static_cast<T>(1));
    }
    [[nodiscard]] static NMatrix<T> Eye(size_t row, size_t col=0)
    {
        NMatrix<T> result = NMatrix<T>::Ones(row,col);
        size_t count = std::min(row,result.nCols);
        for (size_t i=0; i<count; ++i) result.data[i*result.nCols+i]=T{};
        return result;
    }
    [[nodiscard]] static NMatrix<T> Identity(size_t row, size_t col=0)
    {
        NMatrix<T> result = NMatrix<T>::Zeros(row,col);
        size_t count = std::min(row,result.nCols);
        for (size_t i=0; i<count; ++i) result.data[i*result.nCols+i]=static_cast<T>(1);
        return result;
    }
    // SLICE
    inline NMatrix<T> operator[](Slice rs, Slice cs) const noexcept
    {
        NMatrix<T> result={};result.resize(rs.size,cs.size);
        for (size_t i=0; i<rs.size; ++i)
        {
            for (size_t j=0; j<cs.size; ++j)
            {
                result[i][j] = (*this)[rs[i],cs[j]];
            }
        }
        return result;
    }
    inline NMatrix<T> operator()(Slice rs, Slice cs) const noexcept {return (*this)[rs,cs];}
    // APPEND/EXTEND
    inline void append_rows(const T& value) {if (nCols==0) return; data.insert(data.end(),nCols,value);nRows+=1;}
    inline void append_cols(const T& value)
    {
        if (nRows==0) return; std::vector<T> newData; newData.reserve(nRows*(nCols+1));
        for (size_t r=0; r<nRows; ++r) {newData.insert(newData.end(),data.begin()+r*nCols,data.begin()+(r+1)*nCols); newData.push_back(value);}
        data=std::move(newData); nCols+=1;
    }
    inline void append_rows(const std::vector<T>& v)
    {
        if (nCols==0) return; assert(v.size()<=nCols && "Vector size and column counts mismatch"); size_t copyCount = std::min(v.size(),nCols);
        data.insert(data.end(),v.begin(),v.begin()+copyCount);if (copyCount<nCols) data.insert(data.end(),nCols-v.size(),T{});nRows+=1;
    }
    inline void append_cols(const std::vector<T>& v)
    {
        if (nRows==0) return; assert(v.size()<=nRows && "Vector size and column counts mismatch");
        size_t copyCount = std::min(v.size(),nRows); std::vector<T> newData; newData.reserve(nRows*(nCols+1));
        for (size_t r=0;r<nRows;++r){newData.insert(newData.end(),data.begin()+r*nCols,data.begin()+(r+1)*nCols);newData.push_back(r<copyCount?v[r]:T{});}
        data=std::move(newData); nCols+=1;
    }
    inline void append_rows(std::vector<T>&& v)
    {
        if (nCols==0) return; assert(v.size()<=nCols && "Vector size and column counts mismatch"); size_t copyCount = std::min(v.size(),nCols);
        data.insert(data.end(),std::make_move_iterator(v.begin()),std::make_move_iterator(v.begin()+copyCount));
        if (copyCount<nCols) data.insert(data.end(),nCols-v.size(),T{}); nRows+=1;
    }
    inline void append_cols(std::vector<T>&& v)
    {
        if (nRows==0) return; assert(v.size()<=nRows && "Vector size and column counts mismatch");
        size_t copyCount = std::min(v.size(),nRows); std::vector<T> newData; newData.reserve(nRows*(nCols+1));
        for (size_t r=0;r<nRows;++r)
        {newData.insert(newData.end(),data.begin()+r*nCols,data.begin()+(r+1)*nCols);newData.push_back(r<copyCount?std::move(v[r]):T{});}
        data=std::move(newData); nCols+=1;
    }
    template<typename otherVec> requires requires(otherVec v){v.data;}
    inline void append_rows(const otherVec& v)
    {
        assert(v.data.size()<=nCols && "Vector size and column counts mismatch"); size_t copyCount = std::min(v.data.size(),nCols);
        data.insert(data.end(),v.data.begin(),v.data.begin()+copyCount); if (copyCount<nCols) data.insert(data.end(),nCols-v.data.size(),T{}); nRows+=1;
    }
    template<typename otherVec> requires requires(otherVec v){v.data;}
    inline void append_cols(const otherVec& v)
    {
        if (nRows==0) return; assert(v.data.size()<=nRows && "Vector size and column counts mismatch");
        size_t copyCount = std::min(v.data.size(),nRows); std::vector<T> newData; newData.reserve(nRows*(nCols+1));
        for (size_t r=0;r<nRows;++r)
        {newData.insert(newData.end(),data.begin()+r*nCols,data.begin()+(r+1)*nCols);newData.push_back(r<copyCount?v.data[r]:T{});}
        data=std::move(newData); nCols+=1;
    }
    template<typename otherVec> requires requires(otherVec v){v.data;}
    inline void append_rows(otherVec&& v)
    {
        assert(v.data.size()<=nCols && "Vector size and column counts mismatch"); size_t copyCount = std::min(v.data.size(),nCols);
        data.insert(data.end(),std::make_move_iterator(v.data.begin()),std::make_move_iterator(v.data.begin()+copyCount));
        if (copyCount<nCols) data.insert(data.end(),nCols-v.data.size(),T{}); nRows+=1;
    }
    template<typename otherVec> requires requires(otherVec v){v.data;}
    inline void append_cols(otherVec&& v)
    {
        if (nRows==0) return; assert(v.data.size()<=nRows && "Vector size and column counts mismatch");
        size_t copyCount = std::min(v.data.size(),nRows); std::vector<T> newData; newData.reserve(nRows*(nCols+1));
        for (size_t r=0;r<nRows;++r)
        {newData.insert(newData.end(),data.begin()+r*nCols,data.begin()+(r+1)*nCols);newData.push_back(r<copyCount?std::move(v.data[r]):T{});}
        data=std::move(newData); nCols+=1;
    }
    inline void append_rows(std::initializer_list<T> list)
    {
        if (nCols==0) return; assert(list.size()<=nCols && "List size and column count mismatch"); size_t copyCount = std::min(list.size(),nCols);
        data.insert(data.end(),list.begin(),list.begin()+copyCount);if (copyCount<nCols) data.insert(data.end(),nCols-list.size(),T{});nRows+=1;
    }
    inline void append_cols(std::initializer_list<T> list)
    {
        if (nRows==0) return; assert(list.size()<=nRows && "Vector size and column counts mismatch");
        size_t copyCount = std::min(list.size(),nRows); std::vector<T> newData; newData.reserve(nRows*(nCols+1));
        for (size_t r=0;r<nRows;++r)
        {newData.insert(newData.end(),data.begin()+r*nCols,data.begin()+(r+1)*nCols);newData.push_back(r<copyCount?*(list.begin()+r):T{});}
        data=std::move(newData); nCols+=1;
    }
    inline void append_rows(std::initializer_list<std::initializer_list<T>> lists)
    {if (nCols==0) return; for (const auto& list : lists) append_rows(list);}
    inline void append_cols(std::initializer_list<std::initializer_list<T>> lists)
    {
        if (nRows==0) return;
        assert(lists.size()<=nRows && "List and row count mismatch"); size_t maxCol=0; size_t copyCount = std::min(nRows,lists.size());
        for (const auto& list : lists) {maxCol=std::max(maxCol,list.size());} std::vector<T> newData; newData.reserve(nRows*(nCols+maxCol)); size_t r=0;
        for (const auto& list : lists)
        {
            if (r>=copyCount) continue;
            newData.insert(newData.end(),data.begin()+r*nCols,data.begin()+(r+1)*nCols);
            newData.insert(newData.end(),list.begin(),list.end());newData.insert(newData.end(),(maxCol-list.size()),T{});
            r+=1;
        }
        if (copyCount<nRows)
        {
        	for (size_t r=copyCount; r<nRows; ++r)
            {
                newData.insert(newData.end(),data.begin()+r*nCols,data.begin()+(r+1)*nCols);
                newData.insert(newData.end(),maxCol,T{});
            }
        }
        data=std::move(newData);nCols+=maxCol;
    }
    inline void append_rows(size_t n,const T& value) {data.insert(data.end(),n*nCols,value);nRows+=n;}
    inline void append_cols(size_t n,const T& value)
    {
        if (nRows==0) return; std::vector<T> newData; newData.reserve(nRows*(nCols+n));
        for (size_t r=0; r<nRows; ++r)
        {newData.insert(newData.end(),data.begin()+r*nCols,data.begin()+(r+1)*nCols);newData.insert(newData.end(),n,value);}
        data=std::move(newData); nCols+=n;
    }
    template<typename RowProxy> requires requires(RowProxy r){r.rowPtr; r.nCols;}
    inline void append_rows(const RowProxy& row)
    {
        assert(row.nCols<=nCols && "Column counts mismatch"); size_t copyCount = std::min(row.nCols,nCols);
        data.insert(data.end(),row.rowPtr,row.rowPtr+copyCount); if (copyCount<nCols) data.insert(data.end(),nCols-row.nCols,T{}); nRows+=1;
    }
    template<typename MatProxy> requires requires(MatProxy m){m.data; m.nRows; m.nCols;}
    inline void append_rows(const MatProxy& mat)
    {
        if (nCols==0) return; assert(mat.nCols<=nCols && "Matrix column count mismatch");
        size_t colCount = std::min(mat.nCols,nCols);
        for (size_t r=0; r<mat.nRows; ++r)
        {
            data.insert(data.end(),mat.data.begin()+r*mat.nCols,mat.data.begin()+r*mat.nCols+colCount);
            if (colCount<nCols) data.insert(data.end(),nCols-colCount,T{});
        }
        nRows+=mat.nRows;
    }
    template<typename MatProxy> requires requires(MatProxy m){m.data; m.nRows; m.nCols;}
    inline void append_cols(const MatProxy& mat)
    {
        if (nRows==0) return; assert(mat.nRows<=nRows && "Matrix row count mismatch"); std::vector<T> newData; newData.reserve(nRows*(nCols+mat.nCols));
        size_t copyCount = std::min(nRows,mat.nRows);
        for (size_t r=0; r<nRows; ++r)
    	{
            newData.insert(newData.end(),data.begin()+r*nCols,data.begin()+(r+1)*nCols);
			(r>=copyCount)?newData.insert(newData.end(),mat.nCols,T{}):newData.insert(newData.end(),mat.data.begin()+r*mat.nCols,mat.data.begin()+(r+1)*mat.nCols);
        }
        data=std::move(newData);nCols+=mat.nCols;
    }
    // operator + - * / += -= *= /=
    inline NMatrix<T>& operator+=(T value){for(size_t i=0; i<data.size(); ++i) data[i]=data[i]+value; return *this;}
    inline NMatrix<T>& operator-=(T value){for(size_t i=0; i<data.size(); ++i) data[i]=data[i]-value; return *this;}
    inline NMatrix<T>& operator*=(T value){for(size_t i=0; i<data.size(); ++i) data[i]=data[i]*value; return *this;}
    inline NMatrix<T>& operator/=(T value){for(size_t i=0; i<data.size(); ++i) data[i]=data[i]/value; return *this;}
    inline NMatrix<T> operator+(T value) const {NMatrix v=*this; return v+=value;}
    inline NMatrix<T> operator-(T value) const {NMatrix v=*this; return v-=value;}
    inline NMatrix<T> operator*(T value) const {NMatrix v=*this; return v*=value;}
    inline NMatrix<T> operator/(T value) const {NMatrix v=*this; return v/=value;}
    // unary
    inline NMatrix<T> operator-() const {NMatrix<T> result=*this; for (auto& val : result.data) val=-val; return result;}
    inline NMatrix<T> operator-() && {for (auto& val : data) val=-val; return *this;}
    // binary
    template<typename U>
    inline NMatrix<T>& operator+=(const NMatrix<U>& m)
    {
        if (m.nRows!=nRows || m.nCols!=nCols) throw std::runtime_error("Matrix dimension mismatch");
        for (size_t r=0; r<nRows; ++r) for (size_t c=0; c<nCols; ++c) data[r*nCols+c]=data[r*nCols+c]+static_cast<T>(m.data[r*m.nCols+c]); return *this;
    }
    template<typename U>
    inline NMatrix<T>& operator-=(const NMatrix<U>& m)
    {
        if (m.nRows!=nRows || m.nCols!=nCols) throw std::runtime_error("Matrix dimension mismatch");
        for (size_t r=0; r<nRows; ++r) for (size_t c=0; c<nCols; ++c) data[r*nCols+c]=data[r*nCols+c]-static_cast<T>(m.data[r*m.nCols+c]); return *this;
    }
    template<typename U>
    inline NMatrix<T>& operator*=(const NMatrix<U>& m)
    {
        if (m.nRows!=nRows || m.nCols!=nCols) throw std::runtime_error("Matrix dimension mismatch");
        for (size_t r=0; r<nRows; ++r) for (size_t c=0; c<nCols; ++c) data[r*nCols+c]=data[r*nCols+c]*static_cast<T>(m.data[r*m.nCols+c]); return *this;
    }
    template<typename U>
    inline NMatrix<T>& operator/=(const NMatrix<U>& m)
    {
        if (m.nRows!=nRows || m.nCols!=nCols) throw std::runtime_error("Matrix dimension mismatch");
        for (size_t r=0; r<nRows; ++r) for (size_t c=0; c<nCols; ++c) data[r*nCols+c]=data[r*nCols+c]/static_cast<T>(m.data[r*m.nCols+c]); return *this;
    }
    // Transpose
    inline NMatrix<T>& Transpose()
    {
        if (nRows==0 || nCols==0) return *this;
        else if (nRows==1 || nCols==1) {std::swap(nRows,nCols); return *this;}
        else if (nCols==nRows)
    	{
            for (size_t r=0; r<nRows; ++r)
                for (size_t c=r+1; c<nCols; ++c)
                    std::swap(data[r*nCols+c],data[c*nRows+r]);
            return *this;
        }
        else
        {
            std::vector<T> tdata(nRows*nCols);
            constexpr size_t BLOCKSIZE = 32;
            for (size_t r0=0; r0<nRows; r0+=BLOCKSIZE)
            {
                size_t rMax = std::min(r0+BLOCKSIZE,nRows);
                for (size_t c0=0; c0<nCols; c0+=BLOCKSIZE)
                {
                    size_t cMax = std::min(c0+BLOCKSIZE,nCols);
                    for (size_t r=r0; r<rMax; ++r)
                        for (size_t c=c0; c<cMax; ++c)
		                    tdata[c*nRows+r] = data[r*nCols+c];
            	}
        	}
        	data = std::move(tdata); std::swap(nRows,nCols); return *this;
        }
    }
};

/******************************************************************
 *                                      \                         *
 *                                       \\                       *
 *                                        \\\                     *
 *                                         \\\\                   *
 *                                          \\\\\                 *
 *  \                                        \\\\\\               *
 *  \\          /XXXXXXXXXX\                  \\\\\\\             *
 *  \\\        ///XXXXXXXX\\\                  \\\\\\\            *
 *   \\\\     ////XXXXXXX\\\\\\                 \\\\/\\\          *
 *   \\\\\   //////XXXXX\\\\\\\\                 ||||||||>>       *
 *   \\\\\\ //////XXXXX\\\\\\\\\\                /||||//          *
 *    \\\\\X//////XXX\\\\\\\\\\\\\\             //|||/            *
 *    \\\\XXX////XX\\\\\\\\\\\\\\\\\           ///||/             *
 *     \\\XXXX/// \\\\\\\\\\\\\\\\\\\         ////|/              *
 *     \\XXXXXX   \\\\\\\\\\\\\\\\\\\\       //////               *
 *      \XXXX\\\\//\\\\\\\\\\\\\\\\\\\\\   ///////                *
 *      /XXX\\\\\\//\\\\\\\\\\\\\\\\\\\\\\///////                 *
 *     ///X\\\\\\\//\\\\\\\\\\\\\\\\\\\\\\\////                   *
 *    ///  \\\\\\\\/\/\/\/\/\/\/\/\/\/\/\/\///                    *
 *   //       \\\\\\/\/\/\/\/\/\/\/\/\/\/\///                     *
 *  /             \\\/\/\/\/\/\/\/\/\/\///                        *
 *                   \/\/\/\/\/\/\/\/\/                           *
 *                      \/\/\/\/\/                                *
 *                       \/\/\/\/                                 *
 *                        \/\/\/                                  *
 *                         ||||                                   *
 *                         ||||                                   *
 *                          ||                                    *
 *                          ||                                    *
 *                          ||                                    *
 *                         ||||                                   *
 *                        //  \\                                  *
 *                      /XX    XX\                                *
 ******************************************************************/

template <typename T, typename U>
inline T dot(const Vec<T>& v1,const Vec<U>& v2, bool forceMultiply=false)
{
    if (forceMultiply) { size_t size=std::min(v1.data.size(),v2.data.size()); T result = T{}; for (size_t i=0; i<size; ++i) result+= (v1[i]*v2[i]); return result; }
    else
    {
        assert(v1.data.size()==v2.data.size() && "Dimension mismatch");
        if (v1.data.size()!=v2.data.size()) throw std::runtime_error("Vector size mismatch");
        size_t size=v1.data.size(); T result = T{};
        for (size_t i=0; i<size; ++i) result+= (v1.data[i]*static_cast<T>(v2.data[i])); return result;
    }
}

template <typename T, typename U>
inline Matrix<T> matmul(const Vec<T>& v1, const Vec<U>& v2)
{
    size_t nRow = v1.size(); size_t nCol = v2.size(); Matrix<T> result(nRow,nCol,T{});
    for (size_t r=0; r<nRow; ++r) for (size_t c=0; c<nCol; ++c) result.data[r*nCol+c] = v1.data[r]*static_cast<T>(v2.data[c]); return result;
}

template <typename T, typename U>
inline Matrix<T> matmul(const Matrix<T>& m1, const Matrix<U>& m2, bool forceMultiply=false)
{
    if (forceMultiply)
    {
        size_t nRow = m1.nRows; size_t nCol = m2.nCols; size_t nMiddle = std::min(m1.nCols,m2.nRows); Matrix<T> result(nRow,nCol,T{});
        for (size_t r=0; r<nRow; ++r)
            for (size_t c=0; c<nCol; ++c)
                for (size_t m=0; m<nMiddle; ++m)
                    result.data[r*nCol+c] += m1.data[r*m1.nCols+m]*static_cast<T>(m2.data[m*m2.nCols+c]);
        return result;
    }
    else
    {
        assert(m1.nCols==m2.nRows && "Dimension mismatch");
        if (m1.nCols!=m2.nRows) throw std::runtime_error("Matrix dimension mismatch");
        size_t nRow = m1.nRows; size_t nCol = m2.nCols; size_t nMiddle = m1.nCols; Matrix<T> result(nRow,nCol,T{});
        for (size_t r=0; r<nRow; ++r)
            for (size_t c=0; c<nCol; ++c)
                for (size_t m=0; m<nMiddle; ++m)
                    result.data[r*nCol+c] += m1.data[r*m1.nCols+m]*static_cast<T>(m2.data[m*m2.nCols+c]);
        return result;
	}
}

template <typename T, typename U>
inline Vec<T> matmul(const Matrix<T>& m, const Vec<U>& v, bool forceMultiply=false)
{
    if (forceMultiply)
    {
        size_t size=m.nRows; size_t nMiddle=std::min(m.nCols,v.data.size()); Vec<T> result(size, T{});
        for (size_t i=0; i<size; ++i) for (size_t j=0; j<nMiddle; ++j) result.data[i] += m.data[i*m.nCols+j]*static_cast<T>(v.data[j]);
        return result;
    }
    else
    {
        assert(m.nCols==v.data.size() && "Dimension mismatch");
        if (m.nCols!=v.data.size()) throw std::runtime_error("Matrix column count and vector size mismatch");
        size_t size=m.nRows; size_t nMiddle=m.nCols; Vec<T> result(size, T{});
        for (size_t i=0; i<size; ++i) for (size_t j=0; j<nMiddle; ++j) result.data[i] += m.data[i*m.nCols+j]*static_cast<T>(v.data[j]);
        return result;
    }
}
template <typename T, typename U>
inline Vec<T> matmul(const Vec<T>& v, const Matrix<U>& m, bool forceMultiply=false)
{
    if (forceMultiply)
    {
        size_t size=m.nCols; size_t nMiddle=std::min(m.nRows,v.data.size()); Vec<T> result(size, T{});
        for (size_t j=0; j<nMiddle; ++j) for (size_t i=0; i<size; ++i)  result.data[i] += static_cast<T>(m.data[j*m.nCols+i])*v.data[j];
        return result;
    }
    else
    {
        assert(m.nRows==v.data.size() && "Dimension mismatch");
        if (m.nRows!=v.data.size()) throw std::runtime_error("Matrix row count and vector size mismatch");
        size_t size=m.nCols; size_t nMiddle=m.nRows; Vec<T> result(size, T{});
        for (size_t j=0; j<nMiddle; ++j) for (size_t i=0; i<size; ++i)  result.data[i] += static_cast<T>(m.data[j*m.nCols+i])*v.data[j];
        return result;
    }
}

template <typename T, typename U>
inline Vec<T> SchurProduct(const Vec<T>& v1, const Vec<U>& v2, bool forceMultiply=false)
{
    if (forceMultiply)
    {
        size_t size=std::min(v1.data.size(),v2.data.size()); Vec<T> result(size,T{});
        for (size_t i=0; i<size; ++i) result[i] = v1[i]*v2[i]; return result;
    }
    else
    {
        assert(v1.data.size()==v2.data.size() && "Vector size mismatch");
        if (v1.data.size()!=v2.data.size()) throw std::runtime_error("Vector size mismatch");
        size_t size=v1.data.size(); Vec<T> result(size,T{});
        for (size_t i=0; i<size; ++i) result[i] = v1[i]*v2[i]; return result;
	}
}

template<typename T, typename U>
inline Matrix<T> SchurProduct(const Matrix<T>& m1, const Matrix<U>& m2, bool forceMultiply=false)
{
    if (forceMultiply)
    {
        size_t nRow=std::min(m1.nRows,m2.nRows); size_t nCol=std::min(m1.nCols,m2.nCols); size_t size=nRow*nCol; Matrix<T> result(nRow,nCol,T{});
        for (size_t r=0; r<nRow; ++r) for (size_t c=0; c<nCol; ++c) result[r,c] = m1[r,c]*m2[r,c]; return result;
    }
    else
    {
        assert((m1.nRows==m2.nRows && m1.nCols==m2.nCols) && "Matrix dimension mismatch");
        if (m1.nRows!=m2.nRows || m1.nCols!=m2.nCols) throw std::runtime_error("Matrix dimension mismatch");
        size_t nRow=m1.nRows; size_t nCol=m1.nCols; size_t size=nRow*nCol; Matrix<T> result(nRow,nCol,T{});
        for (size_t i=0; i<size; ++i) result.data[i] = m1.data[i]*m2.data[i]; return result;
    }
}

// v1 + v2
/***********************************************************************************
 *                                         _                                       *
 *          ||||            ||||          | |        ||||            ||||          *
 *           \\              //           | |         \\              //           *
 *            \\            //            | |          \\            //            *
 *             \\          //        _____| |_____      \\          //             *
 *              \\        //        |_____   _____|      \\        //   ____       *
 *               \\      //  /|           | |             \\      //   /___ \      *
 *                \\    //  /||           | |              \\    //   //   ||      *
 *                 \\  //  //||           | |               \\  //    ||   ||      *
 *                  \\//  // ||           |_|                \\//     ||  //       *
 *                   \/      ||                               \/         //        *
 *                           ||                                         //         *
 *                        ___||___                                     //___       *
 *                       |________|                                   /_____|      *
 *                                                                                 *
 ***********************************************************************************/

template<typename T, typename U>
inline Vec<T> operator+(const Vec<T>& v1, const Vec<U>& v2)
{if (v1.data.size()!=v2.data.size()) throw std::runtime_error("Vector size mismatch"); Vec<T> result=v1; result+=v2; return result;}
template<typename T, typename U>
inline Vec<T> operator+(const Vec<T>& v1, Vec<U>&& v2)
{if (v1.data.size()!=v2.data.size()) throw std::runtime_error("Vector size mismatch"); v2+=v1; return std::move(v2);}
template<typename T, typename U>
inline Vec<T> operator+(Vec<T>&& v1, const Vec<U>& v2)
{if (v1.data.size()!=v2.data.size()) throw std::runtime_error("Vector size mismatch"); v1+=v2; return std::move(v1);}
template<typename T, typename U>
inline Vec<T> operator+(Vec<T>&& v1, Vec<U>&& v2)
{if (v1.data.size()!=v2.data.size()) throw std::runtime_error("Vector size mismatch"); v1+=v2; return std::move(v1);}

template<typename T, typename U>
inline NVec<T> operator+(const NVec<T>& v1, const NVec<U>& v2)
{if (v1.data.size()!=v2.data.size()) throw std::runtime_error("Vector size mismatch"); NVec<T> result=v1; result+=v2; return result;}
template<typename T, typename U>
inline NVec<T> operator+(const NVec<T>& v1, NVec<U>&& v2)
{if (v1.data.size()!=v2.data.size()) throw std::runtime_error("Vector size mismatch"); v2+=v1; return std::move(v2);}
template<typename T, typename U>
inline NVec<T> operator+(NVec<T>&& v1, const NVec<U>& v2)
{if (v1.data.size()!=v2.data.size()) throw std::runtime_error("Vector size mismatch"); v1+=v2; return std::move(v1);}
template<typename T, typename U>
inline NVec<T> operator+(NVec<T>&& v1, NVec<U>&& v2)
{if (v1.data.size()!=v2.data.size()) throw std::runtime_error("Vector size mismatch"); v1+=v2; return std::move(v1);}

// v1 - v2
/***********************************************************************************
 *                                                                                 *
 *          ||||            ||||                     ||||            ||||          *
 *           \\              //                       \\              //           *
 *            \\            //                         \\            //            *
 *             \\          //        _____________      \\          //             *
 *              \\        //        |_____________|      \\        //   ____       *
 *               \\      //  /|                           \\      //   /___ \      *
 *                \\    //  /||                            \\    //   //   ||      *
 *                 \\  //  //||                             \\  //    ||   ||      *
 *                  \\//  // ||                              \\//     ||  //       *
 *                   \/      ||                               \/         //        *
 *                           ||                                         //         *
 *                        ___||___                                     //___       *
 *                       |________|                                   /_____|      *
 *                                                                                 *
 ***********************************************************************************/

template<typename T, typename U>
inline Vec<T> operator-(const Vec<T>& v1, const Vec<U>& v2)
{if (v1.data.size()!=v2.data.size()) throw std::runtime_error("Vector size mismatch"); Vec<T> result=v1; result-=v2; return result;}
template<typename T, typename U>
inline Vec<T> operator-(const Vec<U>& v1, Vec<T>&& v2)
{
    if (v1.data.size()!=v2.data.size()) throw std::runtime_error("Vector size mismatch");
    size_t n=std::min(v1.data.size(),v2.data.size()); for (size_t i=0; i<n; ++i) v2.data[i]=v1.data[i]-v2.data[i]; return std::move(v2);
}
template<typename T, typename U>
inline Vec<T> operator-(Vec<T>&& v1, const Vec<U>& v2)
{if (v1.data.size()!=v2.data.size()) throw std::runtime_error("Vector size mismatch"); v1-=v2; return std::move(v1);}
template<typename T, typename U>
inline Vec<T> operator-(Vec<T>&& v1, Vec<U>&& v2)
{if (v1.data.size()!=v2.data.size()) throw std::runtime_error("Vector size mismatch"); v1-=v2; return std::move(v1);}

template<typename T, typename U>
inline NVec<T> operator-(const NVec<T>& v1, const NVec<U>& v2)
{if (v1.data.size()!=v2.data.size()) throw std::runtime_error("Vector size mismatch"); NVec<T> result=v1; result-=v2; return result;}
template<typename T, typename U>
inline NVec<T> operator-(const NVec<U>& v1, NVec<T>&& v2)
{
    if (v1.data.size()!=v2.data.size()) throw std::runtime_error("Vector size mismatch");
    size_t n=std::min(v1.data.size(),v2.data.size()); for (size_t i=0; i<n; ++i) v2.data[i]=v1.data[i]-v2.data[i]; return std::move(v2);
}
template<typename T, typename U>
inline NVec<T> operator-(NVec<T>&& v1, const NVec<U>& v2)
{if (v1.data.size()!=v2.data.size()) throw std::runtime_error("Vector size mismatch"); v1-=v2; return std::move(v1);}
template<typename T, typename U>
inline NVec<T> operator-(NVec<T>&& v1, NVec<U>&& v2)
{if (v1.data.size()!=v2.data.size()) throw std::runtime_error("Vector size mismatch"); v1-=v2; return std::move(v1);}

// v1 * v2
/******************************************************************************************
 *                                                                                        *
 *          ||||            ||||                            ||||            ||||          *
 *           \\              //          _      _            \\              //           *
 *            \\            //          | \    / |            \\            //            *
 *             \\          //            \ \  / /              \\          //             *
 *              \\        //              \ \/ /                \\        //   ____       *
 *               \\      //  /|            \  /                  \\      //   /___ \      *
 *                \\    //  /||            /  \                   \\    //   //   ||      *
 *                 \\  //  //||           / /\ \                   \\  //    ||   ||      *
 *                  \\//  // ||          / /  \ \                   \\//     ||  //       *
 *                   \/      ||         |_/    \_|                   \/         //        *
 *                           ||                                                //         *
 *                        ___||___                                            //___       *
 *                       |________|                                          /_____|      *
 *                                                                                        *
 ******************************************************************************************/

template<typename T, typename U>
inline Vec<T> operator*(const Vec<T>& v1, const Vec<U>& v2)
{if (v1.data.size()!=v2.data.size()) throw std::runtime_error("Vector size mismatch"); Vec<T> result=v1; result*=v2; return result;}
template<typename T, typename U>
inline Vec<T> operator*(const Vec<T>& v1, Vec<U>&& v2)
{if (v1.data.size()!=v2.data.size()) throw std::runtime_error("Vector size mismatch"); v2*=v1; return std::move(v2);}
template<typename T, typename U>
inline Vec<T> operator*(Vec<T>&& v1, const Vec<U>& v2)
{if (v1.data.size()!=v2.data.size()) throw std::runtime_error("Vector size mismatch"); v1*=v2; return std::move(v1);}
template<typename T, typename U>
inline Vec<T> operator*(Vec<T>&& v1, Vec<U>&& v2)
{if (v1.data.size()!=v2.data.size()) throw std::runtime_error("Vector size mismatch"); v1*=v2; return std::move(v1);}

template<typename T, typename U>
inline NVec<T> operator*(const NVec<T>& v1, const NVec<U>& v2)
{if (v1.data.size()!=v2.data.size()) throw std::runtime_error("Vector size mismatch"); NVec<T> result=v1; result*=v2; return result;}
template<typename T, typename U>
inline NVec<T> operator*(const NVec<T>& v1, NVec<U>&& v2)
{if (v1.data.size()!=v2.data.size()) throw std::runtime_error("Vector size mismatch"); v2*=v1; return std::move(v2);}
template<typename T, typename U>
inline NVec<T> operator*(NVec<T>&& v1, const NVec<U>& v2)
{if (v1.data.size()!=v2.data.size()) throw std::runtime_error("Vector size mismatch"); v1*=v2; return std::move(v1);}
template<typename T, typename U>
inline NVec<T> operator*(NVec<T>&& v1, NVec<U>&& v2)
{if (v1.data.size()!=v2.data.size()) throw std::runtime_error("Vector size mismatch"); v1*=v2; return std::move(v1);}

// v1 * v2
/******************************************************************************************
 *                                                                                        *
 *          ||||            ||||                            ||||            ||||          *
 *           \\              //                 _            \\              //           *
 *            \\            //                 / |            \\            //            *
 *             \\          //                 / /              \\          //             *
 *              \\        //                 / /                \\        //   ____       *
 *               \\      //  /|             / /                  \\      //   /___ \      *
 *                \\    //  /||            / /                    \\    //   //   ||      *
 *                 \\  //  //||           / /                      \\  //    ||   ||      *
 *                  \\//  // ||          |_/                        \\//     ||  //       *
 *                   \/      ||                                      \/         //        *
 *                           ||                                                //         *
 *                        ___||___                                            //___       *
 *                       |________|                                          /_____|      *
 *                                                                                        *
 ******************************************************************************************/

template<typename T, typename U>
inline Vec<T> operator/(const Vec<T>& v1, const Vec<U>& v2)
{if (v1.data.size()!=v2.data.size()) throw std::runtime_error("Vector size mismatch"); Vec<T> result=v1; result/=v2; return result;}
template<typename T, typename U>
inline Vec<T> operator/(const Vec<U>& v1, Vec<T>&& v2)
{
    if (v1.data.size()!=v2.data.size()) throw std::runtime_error("Vector size mismatch");
    size_t n=std::min(v1.data.size(),v2.data.size()); for (size_t i=0; i<n; ++i) v2.data[i]=v1.data[i]/v2.data[i]; return std::move(v2);
}
template<typename T, typename U>
inline Vec<T> operator/(Vec<T>&& v1, const Vec<U>& v2)
{if (v1.data.size()!=v2.data.size()) throw std::runtime_error("Vector size mismatch"); v1/=v2; return std::move(v1);}
template<typename T, typename U>
inline Vec<T> operator/(Vec<T>&& v1, Vec<U>&& v2)
{if (v1.data.size()!=v2.data.size()) throw std::runtime_error("Vector size mismatch"); v1/=v2; return std::move(v1);}

template<typename T, typename U>
inline NVec<T> operator/(const NVec<T>& v1, const NVec<U>& v2)
{if (v1.data.size()!=v2.data.size()) throw std::runtime_error("Vector size mismatch"); NVec<T> result=v1; result/=v2; return result;}
template<typename T, typename U>
inline NVec<T> operator/(const NVec<U>& v1, NVec<T>&& v2)
{
    if (v1.data.size()!=v2.data.size()) throw std::runtime_error("Vector size mismatch");
    size_t n=std::min(v1.data.size(),v2.data.size()); for (size_t i=0; i<n; ++i) v2.data[i]=v1.data[i]/v2.data[i]; return std::move(v2);
}
template<typename T, typename U>
inline NVec<T> operator/(NVec<T>&& v1, const NVec<U>& v2)
{if (v1.data.size()!=v2.data.size()) throw std::runtime_error("Vector size mismatch"); v1/=v2; return std::move(v1);}
template<typename T, typename U>
inline NVec<T> operator/(NVec<T>&& v1, NVec<U>&& v2)
{if (v1.data.size()!=v2.data.size()) throw std::runtime_error("Vector size mismatch"); v1/=v2; return std::move(v1);}


// m1 + m2
/************************************************************************************************
 *                                                _                                             *
 *         |\\                //|                | |             |\\                //|         *
 *         ||\\              //||                | |             ||\\              //||         *
 *         || \\            // ||                | |             || \\            // ||         *
 *         ||  \\          //  ||           _____| |_____        ||  \\          //  ||         *
 *         ||   \\        //   ||          |_____   _____|       ||   \\        //   ||   ___   *
 *         ||    \\      //    ||    /|          | |             ||    \\      //    ||  /___\  *
 *         ||     \\    //     ||   /||          | |             ||     \\    //     || //   || *
 *         ||      \\  //      ||  //||          | |             ||      \\  //      || ||   || *
 *         ||       \\//       || // ||          |_|             ||       \\//       || ||  //  *
 *         ||        \/        ||    ||                          ||        \/        ||    //   *
 *                                   ||                                                   //    *
 *                                ___||___                                               //___  *
 *                               |________|                                             /_____| *
 *                                                                                              *
 ************************************************************************************************/

template<typename T, typename U>
inline Matrix<T> operator+(const Matrix<T>& m1, const Matrix<U>& m2)
{if (m1.nRows!=m2.nRows || m1.nCols!=m2.nCols) throw std::runtime_error("Matrix dimension mismatch"); Matrix<T> result=m1; result+=m2; return result;}
template<typename T, typename U>
inline Matrix<T> operator+(const Matrix<T>& m1, Matrix<U>&& m2)
{if (m1.nRows!=m2.nRows || m1.nCols!=m2.nCols) throw std::runtime_error("Matrix dimension mismatch"); m2+=m1; return std::move(m2);}
template<typename T, typename U>
inline Matrix<T> operator+(Matrix<T>&& m1, const Matrix<U>& m2)
{if (m1.nRows!=m2.nRows || m1.nCols!=m2.nCols) throw std::runtime_error("Matrix dimension mismatch"); m1+=m2; return std::move(m1);}
template<typename T, typename U>
inline Matrix<T> operator+(Matrix<T>&& m1, Matrix<U>&& m2)
{if (m1.nRows!=m2.nRows || m1.nCols!=m2.nCols) throw std::runtime_error("Matrix dimension mismatch"); m1+=m2; return std::move(m1);}

template<typename T, typename U>
inline NMatrix<T> operator+(const NMatrix<T>& m1, const NMatrix<U>& m2)
{if (m1.nRows!=m2.nRows || m1.nCols!=m2.nCols) throw std::runtime_error("Matrix dimension mismatch"); NMatrix<T> result=m1; result+=m2; return result;}
template<typename T, typename U>
inline NMatrix<T> operator+(const NMatrix<T>& m1, NMatrix<U>&& m2)
{if (m1.nRows!=m2.nRows || m1.nCols!=m2.nCols) throw std::runtime_error("Matrix dimension mismatch"); m2+=m1; return std::move(m2);}
template<typename T, typename U>
inline NMatrix<T> operator+(NMatrix<T>&& m1, const NMatrix<U>& m2)
{if (m1.nRows!=m2.nRows || m1.nCols!=m2.nCols) throw std::runtime_error("Matrix dimension mismatch"); m1+=m2; return std::move(m1);}
template<typename T, typename U>
inline NMatrix<T> operator+(NMatrix<T>&& m1, NMatrix<U>&& m2)
{if (m1.nRows!=m2.nRows || m1.nCols!=m2.nCols) throw std::runtime_error("Matrix dimension mismatch"); m1+=m2; return std::move(m1);}

// m1 - m2
/************************************************************************************************
 *                                                                                              *
 *         |\\                //|                                |\\                //|         *
 *         ||\\              //||                                ||\\              //||         *
 *         || \\            // ||                                || \\            // ||         *
 *         ||  \\          //  ||           _____________        ||  \\          //  ||         *
 *         ||   \\        //   ||          |_____________|       ||   \\        //   ||   ___   *
 *         ||    \\      //    ||    /|                          ||    \\      //    ||  /___\  *
 *         ||     \\    //     ||   /||                          ||     \\    //     || //   || *
 *         ||      \\  //      ||  //||                          ||      \\  //      || ||   || *
 *         ||       \\//       || // ||                          ||       \\//       || ||  //  *
 *         ||        \/        ||    ||                          ||        \/        ||    //   *
 *                                   ||                                                   //    *
 *                                ___||___                                               //___  *
 *                               |________|                                             /_____| *
 *                                                                                              *
 ************************************************************************************************/

template<typename T, typename U>
inline Matrix<T> operator-(const Matrix<T>& m1, const Matrix<U>& m2)
{if (m1.nRows!=m2.nRows || m1.nCols!=m2.nCols) throw std::runtime_error("Matrix dimension mismatch"); Matrix<T> result=m1; result-=m2; return result;}
template<typename T, typename U>
inline Matrix<T> operator-(const Matrix<U>& m1, Matrix<T>&& m2)
{
    if (m1.nRows!=m2.nRows || m1.nCols!=m2.nCols) throw std::runtime_error("Matrix dimension mismatch");
    size_t row=std::min(m1.nRows,m2.nRows); size_t col=std::min(m1.nCols,m2.nCols);
    for (size_t r=0; r<row; ++r) for (size_t c=0; c<col; ++c) m2.data[r*m2.nCols+c]=m1.data[r*m1.nCols+c]-m2.data[r*m2.nCols+c];return std::move(m2);
}
template<typename T, typename U>
inline Matrix<T> operator-(Matrix<T>&& m1, const Matrix<U>& m2)
{if (m1.nRows!=m2.nRows || m1.nCols!=m2.nCols) throw std::runtime_error("Matrix dimension mismatch"); m1-=m2; return std::move(m1);}
template<typename T, typename U>
inline Matrix<T> operator-(Matrix<T>&& m1, Matrix<U>&& m2)
{if (m1.nRows!=m2.nRows || m1.nCols!=m2.nCols) throw std::runtime_error("Matrix dimension mismatch"); m1-=m2; return std::move(m1);}

template<typename T, typename U>
inline NMatrix<T> operator-(const NMatrix<T>& m1, const NMatrix<U>& m2)
{if (m1.nRows!=m2.nRows || m1.nCols!=m2.nCols) throw std::runtime_error("Matrix dimension mismatch"); NMatrix<T> result=m1; result-=m2; return result;}
template<typename T, typename U>
inline NMatrix<T> operator-(const NMatrix<U>& m1, NMatrix<T>&& m2)
{
    if (m1.nRows!=m2.nRows || m1.nCols!=m2.nCols) throw std::runtime_error("Matrix dimension mismatch");
    size_t row=std::min(m1.nRows,m2.nRows); size_t col=std::min(m1.nCols,m2.nCols);
    for (size_t r=0; r<row; ++r) for (size_t c=0; c<col; ++c) m2.data[r*m2.nCols+c]=m1.data[r*m1.nCols+c]-m2.data[r*m2.nCols+c];return std::move(m2);
}
template<typename T, typename U>
inline NMatrix<T> operator-(NMatrix<T>&& m1, const NMatrix<U>& m2)
{if (m1.nRows!=m2.nRows || m1.nCols!=m2.nCols) throw std::runtime_error("Matrix dimension mismatch"); m1-=m2; return std::move(m1);}
template<typename T, typename U>
inline NMatrix<T> operator-(NMatrix<T>&& m1, NMatrix<U>&& m2)
{if (m1.nRows!=m2.nRows || m1.nCols!=m2.nCols) throw std::runtime_error("Matrix dimension mismatch"); m1-=m2; return std::move(m1);}

// m1 * m2
/************************************************************************************************
 *                                                                                              *
 *         |\\                //|                                |\\                //|         *
 *         ||\\              //||            _      _            ||\\              //||         *
 *         || \\            // ||           | \    / |           || \\            // ||         *
 *         ||  \\          //  ||            \ \  / /            ||  \\          //  ||         *
 *         ||   \\        //   ||             \ \/ /             ||   \\        //   ||   ___   *
 *         ||    \\      //    ||    /|        \  /              ||    \\      //    ||  /___\  *
 *         ||     \\    //     ||   /||        /  \              ||     \\    //     || //   || *
 *         ||      \\  //      ||  //||       / /\ \             ||      \\  //      || ||   || *
 *         ||       \\//       || // ||      / /  \ \            ||       \\//       || ||  //  *
 *         ||        \/        ||    ||     |_/    \_|           ||        \/        ||    //   *
 *                                   ||                                                   //    *
 *                                ___||___                                               //___  *
 *                               |________|                                             /_____| *
 *                                                                                              *
 ************************************************************************************************/

template<typename T, typename U>
inline Matrix<T> operator*(const Matrix<T>& m1, const Matrix<U>& m2)
{if (m1.nRows!=m2.nRows || m1.nCols!=m2.nCols) throw std::runtime_error("Matrix dimension mismatch"); Matrix<T> result=m1; result*=m2; return result;}
template<typename T, typename U>
inline Matrix<T> operator*(const Matrix<T>& m1, Matrix<U>&& m2)
{if (m1.nRows!=m2.nRows || m1.nCols!=m2.nCols) throw std::runtime_error("Matrix dimension mismatch"); m2*=m1; return std::move(m2);}
template<typename T, typename U>
inline Matrix<T> operator*(Matrix<T>&& m1, const Matrix<U>& m2)
{if (m1.nRows!=m2.nRows || m1.nCols!=m2.nCols) throw std::runtime_error("Matrix dimension mismatch"); m1*=m2; return std::move(m1);}
template<typename T, typename U>
inline Matrix<T> operator*(Matrix<T>&& m1, Matrix<U>&& m2)
{if (m1.nRows!=m2.nRows || m1.nCols!=m2.nCols) throw std::runtime_error("Matrix dimension mismatch"); m1*=m2; return std::move(m1);}

template<typename T, typename U>
inline NMatrix<T> operator*(const NMatrix<T>& m1, const NMatrix<U>& m2)
{if (m1.nRows!=m2.nRows || m1.nCols!=m2.nCols) throw std::runtime_error("Matrix dimension mismatch"); NMatrix<T> result=m1; result*=m2; return result;}
template<typename T, typename U>
inline NMatrix<T> operator*(const NMatrix<T>& m1, NMatrix<U>&& m2)
{if (m1.nRows!=m2.nRows || m1.nCols!=m2.nCols) throw std::runtime_error("Matrix dimension mismatch"); m2*=m1; return std::move(m2);}
template<typename T, typename U>
inline NMatrix<T> operator*(NMatrix<T>&& m1, const NMatrix<U>& m2)
{if (m1.nRows!=m2.nRows || m1.nCols!=m2.nCols) throw std::runtime_error("Matrix dimension mismatch"); m1*=m2; return std::move(m1);}
template<typename T, typename U>
inline NMatrix<T> operator*(NMatrix<T>&& m1, NMatrix<U>&& m2)
{if (m1.nRows!=m2.nRows || m1.nCols!=m2.nCols) throw std::runtime_error("Matrix dimension mismatch"); m1*=m2; return std::move(m1);}

// m1 * m2
/************************************************************************************************
 *                                                                                              *
 *         |\\                //|                                |\\                //|         *
 *         ||\\              //||                   _            ||\\              //||         *
 *         || \\            // ||                  / |           || \\            // ||         *
 *         ||  \\          //  ||                 / /            ||  \\          //  ||         *
 *         ||   \\        //   ||                / /             ||   \\        //   ||   ___   *
 *         ||    \\      //    ||    /|         / /              ||    \\      //    ||  /___\  *
 *         ||     \\    //     ||   /||        / /               ||     \\    //     || //   || *
 *         ||      \\  //      ||  //||       / /                ||      \\  //      || ||   || *
 *         ||       \\//       || // ||      |_/                 ||       \\//       || ||  //  *
 *         ||        \/        ||    ||                          ||        \/        ||    //   *
 *                                   ||                                                   //    *
 *                                ___||___                                               //___  *
 *                               |________|                                             /_____| *
 *                                                                                              *
 ************************************************************************************************/

template<typename T, typename U>
inline Matrix<T> operator/(const Matrix<T>& m1, const Matrix<U>& m2)
{if (m1.nRows!=m2.nRows || m1.nCols!=m2.nCols) throw std::runtime_error("Matrix dimension mismatch"); Matrix<T> result=m1; result/=m2; return result;}
template<typename T, typename U>
inline Matrix<T> operator/(const Matrix<U>& m1, Matrix<T>&& m2)
{
    if (m1.nRows!=m2.nRows || m1.nCols!=m2.nCols) throw std::runtime_error("Matrix dimension mismatch");
    size_t row=std::min(m1.nRows,m2.nRows); size_t col=std::min(m1.nCols,m2.nCols);
    for (size_t r=0; r<row; ++r) for (size_t c=0; c<col; ++c) m2.data[r*m2.nCols+c]=m1.data[r*m1.nCols+c]/m2.data[r*m2.nCols+c]; return std::move(m2);
}
template<typename T, typename U>
inline Matrix<T> operator/(Matrix<T>&& m1, const Matrix<U>& m2)
{if (m1.nRows!=m2.nRows || m1.nCols!=m2.nCols) throw std::runtime_error("Matrix dimension mismatch"); m1/=m2; return std::move(m1);}
template<typename T, typename U>
inline Matrix<T> operator/(Matrix<T>&& m1, Matrix<U>&& m2)
{if (m1.nRows!=m2.nRows || m1.nCols!=m2.nCols) throw std::runtime_error("Matrix dimension mismatch"); m1/=m2; return std::move(m1);}

template<typename T, typename U>
inline NMatrix<T> operator/(const NMatrix<T>& m1, const NMatrix<U>& m2)
{if (m1.nRows!=m2.nRows || m1.nCols!=m2.nCols) throw std::runtime_error("Matrix dimension mismatch"); NMatrix<T> result=m1; result/=m2; return result;}
template<typename T, typename U>
inline NMatrix<T> operator/(const NMatrix<U>& m1, NMatrix<T>&& m2)
{
    if (m1.nRows!=m2.nRows || m1.nCols!=m2.nCols) throw std::runtime_error("Matrix dimension mismatch");
    size_t row=std::min(m1.nRows,m2.nRows); size_t col=std::min(m1.nCols,m2.nCols);
    for (size_t r=0; r<row; ++r) for (size_t c=0; c<col; ++c) m2.data[r*m2.nCols+c]=m1.data[r*m1.nCols+c]/m2.data[r*m2.nCols+c]; return std::move(m2);
}
template<typename T, typename U>
inline NMatrix<T> operator/(NMatrix<T>&& m1, const NMatrix<U>& m2)
{if (m1.nRows!=m2.nRows || m1.nCols!=m2.nCols) throw std::runtime_error("Matrix dimension mismatch"); m1/=m2; return std::move(m1);}
template<typename T, typename U>
inline NMatrix<T> operator/(NMatrix<T>&& m1, NMatrix<U>&& m2)
{if (m1.nRows!=m2.nRows || m1.nCols!=m2.nCols) throw std::runtime_error("Matrix dimension mismatch"); m1/=m2; return std::move(m1);}

// Transpose
/********************************************************************************************************************************
 *                                                                                                                              *
 *    ||||||||||||   |\\\\\\          /\          |\     ||     /||||\     ||||||\        /||||\        /||||\     |||||||||    *
 *         ||        ||    ||        //\\         |\\    ||    //    \\    ||    \\     ////  \\\\     //    \\    ||     ||    *
 *         ||        ||    ||       //  \\        ||\\   ||   ||      ||   ||     ||   ///      \\\   ||      ||   ||           *
 *         ||        |//////       //    \\       || \\  ||    \\\\        ||    //    ||        ||    \\\\        ||\\\\       *
 *         ||        |\\\         /||||||||\      ||  \\ ||       \\\\\    ||||||/     ||        ||       \\\\\    ||////       *
 *         ||        || \\       //        \\     ||   \\||   ||      ||   ||          \\\      ///   ||      ||   ||           *
 *         ||        ||  \\     //          \\    ||    \\|    \\    //    ||           \\\\  ////     \\    //    ||     ||    *
 *         ||        ||   \\   //            \\   ||     \|     \||||/     ||             \||||/        \||||/     |||||||||    *
 *                                                                                                                              *
 ********************************************************************************************************************************/

template<typename T>
inline Matrix<T> Transpose(const Matrix<T>& m)
{
    if (m.nRows==0 || m.nCols==0) {Matrix<T> result=m; return result;}
    else if (m.nRows==1 || m.nCols==1) {Matrix<T> result=m; std::swap(result.nRows,result.nCols); return result;}
    else if (m.nCols==m.nRows)
    {
        Matrix<T> result = m;
        for (size_t r=0; r<m.nRows; ++r)
            for (size_t c=r+1; c<m.nCols; ++c)
                std::swap(result.data[r*m.nCols+c],result.data[c*m.nRows+r]);
        return result;
    }
    else
    {
        Matrix<T> result(m.nCols,m.nRows,T{});
        constexpr size_t BLOCKSIZE = 32;
        for (size_t r0=0; r0<m.nRows; r0+=BLOCKSIZE)
        {
            size_t rMax = std::min(r0+BLOCKSIZE,m.nRows);
            for (size_t c0=0; c0<m.nCols; c0+=BLOCKSIZE)
            {
                size_t cMax = std::min(c0+BLOCKSIZE,m.nCols);
                for (size_t r=r0; r<rMax; ++r)
                    for (size_t c=c0; c<cMax; ++c)
                        result.data[c*m.nRows+r] = m.data[r*m.nCols+c];
            }
        }
        return result;
    }
}

template<typename T>
inline Matrix<T> Transpose(Matrix<T>&& m)
{
    m.Transpose(); return std::move(m);
}


template<typename T>
inline NMatrix<T> Transpose(const NMatrix<T>& m)
{
    if (m.nRows==0 || m.nCols==0) {NMatrix<T> result=m; return result;}
    else if (m.nRows==1 || m.nCols==1) {NMatrix<T> result=m; std::swap(result.nRows,result.nCols); return result;}
    else if (m.nCols==m.nRows)
    {
        NMatrix<T> result = m;
        for (size_t r=0; r<m.nRows; ++r)
            for (size_t c=r+1; c<m.nCols; ++c)
                std::swap(result.data[r*m.nCols+c],result.data[c*m.nRows+r]);
        return result;
    }
    else
    {
        NMatrix<T> result(m.nCols,m.nRows,T{});
        constexpr size_t BLOCKSIZE = 32;
        for (size_t r0=0; r0<m.nRows; r0+=BLOCKSIZE)
        {
            size_t rMax = std::min(r0+BLOCKSIZE,m.nRows);
            for (size_t c0=0; c0<m.nCols; c0+=BLOCKSIZE)
            {
                size_t cMax = std::min(c0+BLOCKSIZE,m.nCols);
                for (size_t r=r0; r<rMax; ++r)
                    for (size_t c=c0; c<cMax; ++c)
                        result.data[c*m.nRows+r] = m.data[r*m.nCols+c];
            }
        }
        return result;
    }
}

template<typename T>
inline NMatrix<T> Transpose(NMatrix<T>&& m)
{
    m.Transpose(); return std::move(m);
}
