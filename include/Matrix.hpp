#pragma once
#include <cstddef>
#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <print>
#include <ranges>
#include <vector>
#include <string>
#include <concepts>
#include <execution>
#include <stdfloat>
#include <type_traits>
#include <stdexcept>
#include <span>
#include <cmath>
#include <complex>

static constexpr const size_t BLOCKSIZE = 32; // Side length of the square block of matrix (contiguous memory chunck of BLOCKSIZE*BLOCKSIZE size) to try and make operation cache-friendly!

// complex type
template <typename T>
struct is_complex : std::false_type {};
template <typename T>
struct is_complex<std::complex<T>> : std::true_type {};
template <typename T>
inline constexpr bool is_complex_v = is_complex<T>::value;

template <typename T>
concept Complex = is_complex_v<T>;
template <typename T>
concept FPComplex = is_complex_v<T> && std::floating_point<typename T::value_type>;
template <typename T>
concept Real = std::is_arithmetic_v<T>;
template <typename T>
concept FPReal = std::floating_point<T>;
template <typename T>
concept Number = Real<T> || Complex<T>;
template <typename T>
concept FPNumber = FPReal<T> || FPComplex<T>;

template <typename R>
concept Range2D = std::ranges::forward_range<R> && std::ranges::forward_range<std::ranges::range_reference_t<R>>;

inline std::uint64_t wrap_index(std::int64_t i,std::uint64_t nIndex)
{
    if (nIndex>0)
    {
        std::int64_t rem = i%static_cast<std::int64_t>(nIndex);
        return static_cast<std::uint64_t>(rem<0?rem+nIndex:rem);
    }
    else {return 0;}

}

template <typename T>
class Ranges
{
	private:
        T start=0;
        T stop=0;
        T step=1;
        std::uint64_t size=0;
	public:
        Ranges() = default;
        Ranges(const Ranges&) = default;
        Ranges(Ranges&&) noexcept = default;
        explicit Ranges(T start_, T stop_, T step_) : start(start_), stop(stop_), step(step_)
        {
            if (step==0 || (step>0 && stop<=start) || (step<0 && stop>=start)) size = 0;
            else
            {
                std::float64_t diff = static_cast<std::float64_t>(stop-start); std::float64_t dStep = static_cast<std::float64_t>(step);
                std::float64_t divided = diff/dStep;
                std::uint64_t newSize = static_cast<std::uint64_t>(divided);
                std::float64_t eps=std::max<std::float64_t>(1e-14,divided*std::numeric_limits<std::float64_t>::epsilon()*10.0);
                size = (divided-static_cast<double>(newSize))>eps?newSize+1:newSize;
            }
        }
        [[nodiscard]] inline T Start() const noexcept {return start;}
        [[nodiscard]] inline T Stop() const noexcept {return stop;}
        [[nodiscard]] inline T Step() const noexcept {return step;}
        [[nodiscard]] inline std::uint64_t Size() const noexcept {return size;}
        [[nodiscard]] inline T operator[](std::int64_t index) const noexcept
        {
            if(index<size && size>=0) return start+index*step;
            else return start+wrap_index(index,size)*step;
        }
        Ranges<T>& operator=(const Ranges&) = default;
        Ranges<T>& operator=(Ranges&&) noexcept = default;
};

using Slice = Ranges<std::int64_t>;

template <Number T=std::float64_t>
class Matrix
{
	private:
        std::vector<T> data {};
        std::uint64_t  nRows{};
        std::uint64_t  nCols{};

	public:
        /*******************
         *                 *
         *        X        *
         *       /X\       *
         *     ///X\\\     *
         *   /////X\\\\\   *
         *                 *
         *******************/
        Matrix() = default;
        ~Matrix() = default;
        Matrix(const Matrix&) = default;
        Matrix(Matrix&&) noexcept = default;
        explicit Matrix(std::uint64_t nRows_, std::uint64_t nCols_, T value = T{})
        : nRows(nRows_), nCols(nCols_), data(nRows_*nCols_,static_cast<T>(value)) {}
        // Matrix(std::uint64_t nRows_, std::uint64_t nCols_) : nRows(nRows_), nCols(nCols_) {data.reserve(nRows*nCols);}
        // Spans
		Matrix(std::span<const T> span_, bool h=true) : data(span_.begin(),span_.end())
        {
            if (data.size()==0) {nRows=0; nCols=0;}
            else
            {
            	if (h) {nRows=1; nCols=span_.size();}
            	else   {nRows=span_.size(); nCols=1;}
            }
        }
        // Initializer List
        template <Number U>
        Matrix(std::initializer_list<U> list, bool h) : data(list.begin(),list.end())
        {
            if (data.size()==0) {nRows=0; nCols=0;}
            else
            {
            	if (h) {nRows=1; nCols=list.size();}
            	else   {nRows=list.size(); nCols=1;}
            }
        }
        template <Number U>
        Matrix(std::initializer_list<std::initializer_list<U>> lists)
        {
            nRows = lists.size();
            if (nRows==0) nCols=0;
            else
            {
				 nCols = 0; for (auto& list : lists) nCols = std::max(list.size(),nCols);
				 data.reserve(nRows*nCols);
				 for (auto& list : lists)
				 {
				     data.insert(data.end(),list.begin(),list.end());
				     if(list.size()<nCols) data.insert(data.end(),nCols-list.size(),T{});
				 }
			}
        }
        // std::vector rvalue
        Matrix(std::vector<T>&& vec, bool h=true) : data(std::move(vec))
        {
            if (data.size()==0) {nRows=0; nCols=0;}
            else
            {
                if (h) {nRows=1; nCols=data.size();}
                else {nRows=data.size(); nCols=1;}
            }
        }
        // 2D range
        template <Range2D R>
        Matrix(R&& grid)
        {
            nRows = std::ranges::distance(grid);
            if (nRows==0) nCols = 0;
            else
            {
	            nCols = 0; for (auto&& row : grid) nCols = std::max<std::uint64_t>(nCols,std::ranges::distance(row));
	            data.reserve(nRows*nCols);
	            for (auto&& row : grid)
	            {
	                data.insert(data.end(),std::ranges::begin(row),std::ranges::end(row));
	                if (std::ranges::distance(row)<nCols) data.insert(data.end(),nCols-std::ranges::distance(row),T{});
	            }
	        }
        }
        /*********************************
         *                               *
         *   =========================   *
         *   =========================   *
         *                               *
         *   =========================   *
         *   =========================   *
         *                               *
         *********************************/
		inline Matrix& operator=(const Matrix&) = default;
        inline Matrix& operator=(Matrix&&) noexcept = default;
        // std::span<const T>
		inline Matrix& operator=(std::span<const T> span_)
        {
            if (span_.size()==0) {nRows=0; nCols=0; data.clear(); return *this;}
            else
            {
                if (nCols==1) nRows=span_.size(); else {nRows=1; nCols=span_.size();}
                data.clear(); data(span_.begin(),span_.end()); return *this;
            }
        }
		// std::initializer_list
		template <Number U>
		inline Matrix& operator=(std::initializer_list<U> list)
        {
            if (list.size()==0) {nRows=0; nCols=0; data.clear(); return *this;}
            else
            {
                if (nCols==1) nRows=list.size(); else {nRows=1; nCols=list.size();}
                data.clear(); data(list.begin(),list.end()); return *this;
            }
        }
		template <Number U>
		inline Matrix& operator=(std::initializer_list<std::initializer_list<U>> lists)
        {
            data.clear(); nRows = lists.size();
            if (nRows==0) {nCols=0;  return *this;}
            nCols = 0; for (auto& list : lists) nCols = std::max(list.size(),nCols);
            data.reserve(nRows*nCols);
            for (auto& list : lists)
            {
                data.insert(data.end(),list.begin(),list.end());
                if(list.size()<nCols) data.insert(data.end(),nCols-list.size(),T{});
            }
            return *this;
        }
		// std::vector&&
		template <Number U>
		inline Matrix& operator=(std::vector<U>&& vec)
        {
            if (vec.size()==0) {nRows=0; nCols=0; data.clear(); return *this;}
            else
            {
                if (nCols==1) nRows=vec.size(); else {nRows=1; nCols=vec.size();}
                data.clear(); data(std::move(vec)); return *this;
            }
        }
        // 2D Range
        template <Range2D R>
        inline Matrix& operator=(R&& grid)
        {
            data.clear(); nRows = std::ranges::distance(grid);
            if (nRows==0) {nCols = 0; return *this;}
            nCols = 0; for (auto&& row : grid) nCols = std::max<std::uint64_t>(nCols,std::ranges::distance(row));
            data.reserve(nRows*nCols);
            for (auto&& row : grid)
            {
                data.insert(data.end(),std::ranges::begin(row),std::ranges::end(row));
                if (std::ranges::distance(row)<nCols) data.insert(data.end(),nCols-std::ranges::distance(row),T{});
            }
            return *this;
        }

        /********************
         *   ____    ____   *
         *  |  __|  |__  |  *
         *  | |        | |  *
         *  | |        | |  *
         *  | |        | |  *
         *  | |        | |  *
         *  | |__    __| |  *
         *  |____|  |____|  *
         *                  *
         ********************/
        // operator[]
        // Matrix[i,j]
        inline T& operator[](std::uint64_t i, std::uint64_t j)
        {
            if (i>=nRows) throw std::runtime_error(std::to_string(i)+" out of bound!");
            if (j>=nCols) throw std::runtime_error(std::to_string(j)+" out of bound!");
            return data[i*nCols+j];
        }
        inline const T& operator[](std::uint64_t i, std::uint64_t j) const
        {
            if (i>=nRows) throw std::runtime_error(std::to_string(i)+" out of bound!");
            if (j>=nCols) throw std::runtime_error(std::to_string(j)+" out of bound!");
            return data[i*nCols+j];
        }
        // Matrix[i]
        inline std::span<T> operator[](std::uint64_t i)
        {
            if (i>=nRows) throw std::runtime_error(std::to_string(i)+" is out of bound");
            std::span<T> spanData {data}; return spanData.subspan(i*nCols,nCols);
        }
        inline std::span<const T> operator[](std::uint64_t i) const
        {
            if (i>=nRows) throw std::runtime_error(std::to_string(i)+" is out of bound");
            std::span<T> spanData {data}; return spanData.subspan(i*nCols,nCols);
        }
        // Slice and Dice
        inline Matrix operator[](Slice rs, Slice cs) const
        {
            if (rs.Start() < 0 || rs.Start() > static_cast<std::int64_t>(nRows-1) || rs.Stop() < -1 || rs.Stop() > static_cast<std::int64_t>(nRows) ||
                cs.Start() < 0 || cs.Start() > static_cast<std::int64_t>(nCols-1) || cs.Stop() < -1 || cs.Stop() > static_cast<std::int64_t>(nCols))
        	{throw std::runtime_error("Index out of bounds");} Matrix<T> sliced(rs.Size(),cs.Size(),T{});
            for (std::uint64_t i{}; i<rs.Size(); ++i)
                for (std::uint64_t j{}; j<cs.Size(); ++j)
                    sliced[i,j] = data[rs[i]*nCols+cs[j]];
            return sliced;
        }

        /*********************
         *                   *
         *        //  \\     *
         *      //      \\   *
         *     //        \\  *
         *    ||          || *
         *    ||          || *
         *     \\        //  *
         *      \\      //   *
         *        \\  //     *
         *                   *
         *********************/
        // Matrix(i,j)
        inline T& operator()(std::int64_t i, std::int64_t j) {return data[wrap_index(i,nRows)*nCols+wrap_index(j,nCols)];}
        inline const T& operator()(std::int64_t i, std::int64_t j) const {return data[wrap_index(i,nRows)*nCols+wrap_index(j,nCols)];}
        // Matrix(i)
        inline std::span<T> operator()(std::int64_t i)
        {std::span<T> spanData {data};return spanData.subspan(wrap_index(i, nRows)*nCols,nCols);}
        inline std::span<const T> operator()(std::int64_t i) const
        {std::span<T> spanData {data};return spanData.subspan(wrap_index(i, nRows)*nCols,nCols);}
        // Slice and Dice
        inline Matrix operator()(Slice rs, Slice cs) const noexcept
        {
            Matrix<T> sliced(rs.Size(),cs.Size(),T{});
            for (std::uint64_t i{}; i<rs.Size(); ++i)
                for (std::uint64_t j{}; j<cs.Size(); ++j)
                    sliced[i,j] = data[wrap_index(rs[i],nRows)*nCols+wrap_index(cs[j],nCols)];
            return sliced;
        }

        /*****************************
         *       _                   *
         *      | |                  *
         *   ___| |___    _________  *
         *  |___   ___|  |_________| *
         *      | |                  *
         *      |_|                  *
         *  _      _            _    *
         * | \    / |          / |   *
         *  \ \  / /          / /    *
         *   \ \/ /          / /     *
         *    \  /          / /      *
         *    /  \         / /       *
         *   / /\ \       / /        *
         *  / /  \ \     / /         *
         * |_/    \_|   |_/          *
         *                           *
         *****************************/
        // + (Plus)
        template <Number U>
        inline Matrix& operator+=(U value) {for (std::uint64_t i{}; i<data.size(); ++i) data[i] += value; return *this;}
        template <Number U>
        inline Matrix operator+(U value) const {Matrix<T> m=(*this); return m+=value;}
        // - (Minus)
        template <Number U>
        inline Matrix& operator-=(U value) {for (std::uint64_t i{}; i<data.size(); ++i) data[i] -= value; return *this;}
        template <Number U>
        inline Matrix operator-(U value) const {Matrix<T> m=(*this); return m-=value;}
        // * (Times)
        template <Number U>
        inline Matrix& operator*=(U value) {for (std::uint64_t i{}; i<data.size(); ++i) data[i] *= value; return *this;}
        template <Number U>
        inline Matrix operator*(U value) const {Matrix<T> m=(*this); return m*=value;}
        // / (Divide)
        template <Number U>
        inline Matrix& operator/=(U value) {for (std::uint64_t i{}; i<data.size(); ++i) data[i] /= value; return *this;}
        template <Number U>
        inline Matrix operator/(U value) const {Matrix<T> m=(*this); return m/=value;}

        // Matrix Matrix arithmetics
        // + (Plus)
        template <Number U>
        inline Matrix& operator+=(const Matrix<U>& m)
        {
            if (nRows!=m.nRows || nCols!=m.nCols) throw std::runtime_error("Dimension mismatch for Matrix element-wise addition");
            else {for (std::uint64_t i{}; i<data.size(); ++i) data[i] += m.data[i]; return *this;}
        }
        template <Number U>
        inline Matrix operator+(const Matrix<U>& m) {Matrix<T> nm(*this); return nm+=m;}
        // - (Minus)
        template <Number U>
        inline Matrix& operator-=(const Matrix<U>& m)
        {
            if (nRows!=m.nRows || nCols!=m.nCols) throw std::runtime_error("Dimension mismatch for Matrix element-wise addition");
            else {for (std::uint64_t i{}; i<data.size(); ++i) data[i] -= m.data[i]; return *this;}
        }
        template <Number U>
        inline Matrix operator-(const Matrix<U>& m) {Matrix<T> nm(*this); return nm-=m;}
        // * (Times)
        template <Number U>
        inline Matrix& operator*=(const Matrix<U>& m)
        {
            if (nRows!=m.nRows || nCols!=m.nCols) throw std::runtime_error("Dimension mismatch for Matrix element-wise addition");
            else {for (std::uint64_t i{}; i<data.size(); ++i) data[i] *= m.data[i]; return *this;}
        }
        template <Number U>
        inline Matrix operator*(const Matrix<U>& m) {Matrix<T> nm(*this); return nm*=m;}
        // / (Divide)
        template <Number U>
        inline Matrix& operator/=(const Matrix<U>& m)
        {
            if (nRows!=m.nRows || nCols!=m.nCols) throw std::runtime_error("Dimension mismatch for Matrix element-wise addition");
            else {for (std::uint64_t i{}; i<data.size(); ++i) data[i] /= m.data[i]; return *this;}
        }
        template <Number U>
        inline Matrix operator/(const Matrix<U>& m) {Matrix<T> nm(*this); return nm/=m;}

		// Row and Column arithmetics
		template <Number U>
		inline Matrix& RowPlus(std::uint64_t row, U value) {std::uint64_t start = row*nCols; for (std::uint64_t i{}; i<nCols; ++i) data[start+i] += value; return *this;}
		template <Number U>
		inline Matrix& RowMinus(std::uint64_t row, U value) {std::uint64_t start = row*nCols; for (std::uint64_t i{}; i<nCols; ++i) data[start+i] -= value; return *this;}
		template <Number U>
		inline Matrix& RowMultiply(std::uint64_t row, U value) {std::uint64_t start = row*nCols; for (std::uint64_t i{}; i<nCols; ++i) data[start+i] *= value; return *this;}
		template <Number U>
		inline Matrix& RowDivide(std::uint64_t row, U value) {std::uint64_t start = row*nCols; for (std::uint64_t i{}; i<nCols; ++i) data[start+i] /= value; return *this;}
		template <Number U>
		inline Matrix& ColPlus(std::uint64_t col, U value) {for (std::uint64_t i{}; i<nRows; ++i) data[i*nCols+col] += value;}
		template <Number U>
		inline Matrix& ColMinus(std::uint64_t col, U value) {for (std::uint64_t i{}; i<nRows; ++i) data[i*nCols+col] -= value; return *this;}
		template <Number U>
		inline Matrix& ColMultiply(std::uint64_t col, U value) {for (std::uint64_t i{}; i<nRows; ++i) data[i*nCols+col] *= value; return *this;}
		template <Number U>
		inline Matrix& ColDivide(std::uint64_t col, U value) {for (std::uint64_t i{}; i<nRows; ++i) data[i*nCols+col] /= value; return *this;}

        /*****************************
         * _________________________ *
         * _________________________ *
         * _________________________ *
         * _________________________ *
         * _________________________ *
         * _________________________ *
         * _________________________ *
         *                  _        *
         *     //  //      | |       *
         *    //  //    ___| |___    *
         *   ||  ||    |___   ___|   *
         *    \\  \\       | |       *
         *     \\  \\      |_|       *
         *                           *
         *****************************/
        // append rows
        inline Matrix& AppendRows(T value, std::uint64_t n=1);
        inline Matrix& AppendRows(std::span<const T> row);
        inline Matrix& AppendRows(std::initializer_list<T> list);
        inline Matrix& AppendRows(std::initializer_list<std::initializer_list<T>> lists);
        template <Range2D R>
        inline Matrix& AppendRows(R&& grid);
        template <Number U>
        inline Matrix& JoinVertical(const Matrix<U>& m);

        /******************************************
         *                               _        *
         * ||||||||||||||||||||         /_\       *
         * ||||||||||||||||||||        // \\      *
         * ||||||||||||||||||||       // _ \\     *
         * ||||||||||||||||||||      // /_\ \\    *
         * ||||||||||||||||||||      / // \\ \    *
         * ||||||||||||||||||||       //   \\     *
         * ||||||||||||||||||||      //     \\    *
         * ||||||||||||||||||||      /       \    *
         * ||||||||||||||||||||          _        *
         * ||||||||||||||||||||         | |       *
         * ||||||||||||||||||||      ___| |___    *
         * ||||||||||||||||||||     |___   ___|   *
         * ||||||||||||||||||||         | |       *
         * ||||||||||||||||||||         |_|       *
         *                                        *
         ******************************************/
        // NOTE: I sincerely don't recommend using these family of functions (they are memory intensive)
        inline Matrix& AppendCols(T value, std::uint64_t n=1);
        inline Matrix& AppendCols(std::span<const T> col);
        inline Matrix& AppendCols(std::initializer_list<T> col);
        inline Matrix& AppendCols(std::initializer_list<std::initializer_list<T>> cols);
        template <Range2D R>
        inline Matrix<T>& AppendCols(R&& grid);
        template <Number U>
        inline Matrix& JoinHorizontal(const Matrix<U>& m);

        // append cols
		/*****************************
         *           ____      ____  *
         *          / / /     / / /  *
         *        /////     /////    *
         *      / / /     / / /      *
         *    /////     /////        *
         *  |||||     |||||          *
         *    \\\\\     \\\\\        *
         *      \ \ \     \ \ \      *
         *        \\\\\     \\\\\    *
         *          \_\_\     \_\_\  *
         *                           *
         *****************************/
        // info
        [[nodiscard]] inline std::uint64_t Rows() const noexcept { return nRows; }
        [[nodiscard]] inline std::uint64_t Cols() const noexcept { return nCols; }
        [[nodiscard]] inline std::uint64_t size() const noexcept { return data.size(); }
        [[nodiscard]] inline std::pair<std::uint64_t,std::uint64_t> Shape() const noexcept { return {nRows,nCols}; }
        [[nodiscard]] inline bool empty() const noexcept { return data.empty(); }
        [[nodiscard]] inline T* ptr() noexcept { return data.data(); }
        [[nodiscard]] inline const T* ptr() const noexcept { return data.data(); }

        // SWAP
        // swap rows
        inline Matrix& SwapRows(std::int64_t r1, std::int64_t r2) noexcept;
        template <Number U>
        inline friend Matrix<U> SwapRows(const Matrix<U>& m, std::int64_t r1, std::int64_t r2) noexcept;
        template <Number U>
        inline friend Matrix<U> SwapRows(Matrix<U>&& m, std::int64_t r1, std::int64_t r2) noexcept;
        // swap cols
        inline Matrix& SwapCols(std::int64_t c1, std::int64_t c2) noexcept;
        template <Number U>
        inline friend Matrix<U> SwapCols(const Matrix<U>& m, std::int64_t c1, std::int64_t c2) noexcept;
        template <Number U>
        inline friend Matrix<U> SwapCols(Matrix<U>&& m, std::int64_t c1, std::int64_t c2) noexcept;
        // make square
		inline Matrix& MakeSquare() noexcept;
        template <Number U>
        inline friend Matrix<U> MakeSquare(const Matrix<U>& m) noexcept;
        template <Number U>
        inline friend Matrix<U> MakeSquare(Matrix<U>&& m) noexcept;
        // transpose
		inline Matrix& Transpose() noexcept;
        template <Number U>
        inline friend Matrix<U> Transpose(const Matrix<U>& m) noexcept;
        template <Number U>
        inline friend Matrix<U> Transpose(Matrix<U>&& m) noexcept;
        // resize
        inline Matrix& resize(std::uint64_t r, std::uint64_t c);
        template <Number U>
        inline friend Matrix<U> resize(const Matrix<U>& m, std::uint64_t r, std::uint64_t c) noexcept;
        template <Number U>
        inline friend Matrix<U> resize(Matrix<U>&& m, std::uint64_t r, std::uint64_t c) noexcept;
        // reshape
        inline Matrix& reshape(std::int64_t r, std::int64_t c);
        template <Number U>
        inline friend Matrix<U> reshape(const Matrix<U>& m, std::int64_t r, std::int64_t c);
        template <Number U>
        inline friend Matrix<U> reshape(Matrix<U>&& m, std::int64_t r, std::int64_t c);
        // print
        inline void printm(std::uint16_t width=10, std::uint16_t accuracy=8);
		inline const T Trace() const noexcept;

		/***********************************************
         *                 _____________               *
         *         \\\   |             |         \\\   *
         *          \\\  |             |          \\\  *
         * ==========\\\ |             | ==========\\\ *
         * ==========/// |             | ==========/// *
         *          ///  |             |          ///  *
         *         ///   |_____________|         ///   *
         *                                             *
         ***********************************************/
        // Apply Function
        template <typename Func>
        inline Matrix& ApplyFunc(Func&& f, bool parallel=false)
        {
            if (nRows==0 || nCols==0) return *this;
            if (!parallel) {std::transform(std::execution::unseq,data.begin(),data.end(),data.begin(),std::forward<Func>(f)); return *this;}
            else {std::transform(std::execution::par_unseq,data.begin(),data.end(),data.begin(),std::forward<Func>(f)); return *this;}
        }
		template <typename Func>
        inline friend Matrix<T> ApplyFunc(const Matrix<T>& m, Func&& f, bool parallel=false)
        {
            Matrix<T> result(m.nRows,m.nCols); if (m.nRows==0 || m.nCols==0) return result;
            if (!parallel) {std::transform(std::execution::unseq,m.data.cbegin(),m.data.cend(),result.data.begin(),std::forward<Func>(f)); return result;}
            else {std::transform(std::execution::par_unseq,m.data.cbegin(),m.data.cend(),result.data.begin(),std::forward<Func>(f)); return result;}
        }
        template <typename Func>
        inline friend Matrix<T> ApplyFunc(Matrix<T>&& m, Func&& f, bool parallel = false) {return m.ApplyFunc(f,parallel);}
        // abs
        [[nodiscard]] inline Matrix& abs(bool parallel=false) {return (*this).ApplyFunc([](T x)->T{return std::abs(x);},parallel);}
        [[nodiscard]] inline friend Matrix<T> abs(const Matrix<T>& m, bool parallel = false) {Matrix<T> result(m); return result.ApplyFunc([](T x)->T{return std::abs(x);},parallel);}
        [[nodiscard]] inline friend Matrix<T> abs(Matrix<T>&& m, bool parallel = false) {return m.abs(parallel);}
        // power
        template <Number U>
        [[nodiscard]] inline Matrix& power(U exponent,bool parallel=false) {return (*this).ApplyFunc([exponent](T x)->T{return std::pow(x,exponent);},parallel);}
        template <Number U>
        [[nodiscard]] inline friend Matrix<T> power(const Matrix<T>& m, U exponent, bool parallel = false) {Matrix<T> result(m); return result.ApplyFunc([exponent](T x)->T{return std::pow(x,exponent);},parallel);}
        template <Number U>
        [[nodiscard]] inline friend Matrix<T> power(Matrix<T>&& m, U exponent, bool parallel = false) {return m.power(exponent,parallel);}
        // sqrt
        [[nodiscard]] inline Matrix& sqrt(bool parallel=false) {return (*this).ApplyFunc([](T x)->T{return std::sqrt(x);},parallel);}
        [[nodiscard]] inline friend Matrix<T> sqrt(const Matrix<T>& m, bool parallel = false) {Matrix<T> result(m); return result.ApplyFunc([](T x)->T{return std::sqrt(x);},parallel);}
        [[nodiscard]] inline friend Matrix<T> sqrt(Matrix<T>&& m, bool parallel = false) {return m.sqrt(parallel);}
        // sin
        [[nodiscard]] inline Matrix& sin(bool parallel=false) {return (*this).ApplyFunc([](T x)->T{return std::sin(x);},parallel);}
        [[nodiscard]] inline friend Matrix<T> sin(const Matrix<T>& m, bool parallel = false) {Matrix<T> result(m); return result.ApplyFunc([](T x)->T{return std::sin(x);},parallel);}
        [[nodiscard]] inline friend Matrix<T> sin(Matrix<T>&& m, bool parallel = false) {return m.sin(parallel);}
        // cos
        [[nodiscard]] inline Matrix& cos(bool parallel=false) {return (*this).ApplyFunc([](T x)->T{return std::cos(x);},parallel);}
        [[nodiscard]] inline friend Matrix<T> cos(const Matrix<T>& m, bool parallel = false) {Matrix<T> result(m); return result.ApplyFunc([](T x)->T{return std::cos(x);},parallel);}
        [[nodiscard]] inline friend Matrix<T> cos(Matrix<T>&& m, bool parallel = false) {return m.cos(parallel);}
};

/*****************************
 * _________________________ *
 * _________________________ *
 * _________________________ *
 * _________________________ *
 * _________________________ *
 * _________________________ *
 * _________________________ *
 *                  _        *
 *     //  //      | |       *
 *    //  //    ___| |___    *
 *   ||  ||    |___   ___|   *
 *    \\  \\       | |       *
 *     \\  \\      |_|       *
 *                           *
 *****************************/
// value
template <Number T>
inline Matrix<T>& Matrix<T>::AppendRows(T value, std::uint64_t n)
{
    if (nCols==0) return *this;
    data.reserve((nRows+n)*nCols); data.insert(data.end(),n*nCols,value);
    nRows += n; return *this;
}
// std::span
template <Number T>
inline Matrix<T>& Matrix<T>::AppendRows(std::span<const T> row)
{
    if (nCols==0) return *this;
    data.reserve((nRows+1)*nCols); std::uint64_t rsize = row.size();
    if (rsize >= nCols) {data.insert(data.end(),row.begin(),row.begin()+nCols); nRows += 1; return *this;}
    else {data.insert(data.end(),row.begin(),row.end()); data.insert(data.end(),nCols-rsize,T{}); nRows += 1; return *this;}
}
// std::initializer_list
template <Number T>
inline Matrix<T>& Matrix<T>::AppendRows(std::initializer_list<T> list)
{
    if (nCols==0) return *this;
    data.reserve((nRows+1)*nCols); std::uint64_t rsize = list.size();
    if (rsize >= nCols) {data.insert(data.end(),list.begin(),list.begin()+nCols); nRows += 1; return *this;}
    else {data.insert(data.end(),list.begin(),list.end()); data.insert(data.end(),nCols-rsize,T{}); nRows += 1; return *this;}
}
template <Number T>
inline Matrix<T>& Matrix<T>::AppendRows(std::initializer_list<std::initializer_list<T>> lists)
{
    if (nCols==0) return *this; for (auto& list : lists) (*this).AppendRows(list); return *this;
}
// 2D span
template <Number T> template <Range2D R>
inline Matrix<T>& Matrix<T>::AppendRows(R&& grid)
{
    if (nCols==0) return *this; for (auto&& row : grid) (*this).AppendRows(row); return *this;
}
template <Number T> template <Number U>
inline Matrix<T>& Matrix<T>::JoinVertical(const Matrix<U>& m)
{
    if (nCols==0) return *this; if(m.empty()) return *this; data.reserve((nRows+m.nRows)*nCols);
    if (nCols>=m.nCols) {for (std::uint64_t i{}; i<m.nRows; ++i) data.insert(data.end(),m.data.begin()+i*m.nCols,m.data.begin()+i*m.nCols+nCols); nRows += m.nRows; return *this;}
    else {for (std::uint64_t i{}; i<m.nRows; ++i) {data.insert(data.end(),m.data.begin(),m.data.end()); data.insert(data.end(),nCols-m.nCols,T{});} nRows += m.nRows; return *this;}
}

/******************************************
 *                               _        *
 * ||||||||||||||||||||         /_\       *
 * ||||||||||||||||||||        // \\      *
 * ||||||||||||||||||||       // _ \\     *
 * ||||||||||||||||||||      // /_\ \\    *
 * ||||||||||||||||||||      / // \\ \    *
 * ||||||||||||||||||||       //   \\     *
 * ||||||||||||||||||||      //     \\    *
 * ||||||||||||||||||||      /       \    *
 * ||||||||||||||||||||          _        *
 * ||||||||||||||||||||         | |       *
 * ||||||||||||||||||||      ___| |___    *
 * ||||||||||||||||||||     |___   ___|   *
 * ||||||||||||||||||||         | |       *
 * ||||||||||||||||||||         |_|       *
 *                                        *
 ******************************************/
// NOTE: I sincerely don't recommend using these family of functions (they are memory intensive)
// value
template <Number T>
inline Matrix<T>& Matrix<T>::AppendCols(T value, std::uint64_t n)
{
    if (nRows==0) return *this; std::vector<T> NewData; NewData.reserve(nRows*(nCols+n));
    for (std::uint64_t i{}; i<nRows; ++i)
    {
        NewData.insert(NewData.end(),data.begin()+i*nCols,data.begin()+(i+1)*nCols); NewData.insert(NewData.end(),n,value);
    }
    nCols += n; data = std::move(NewData); return *this;
}
// std::span
template <Number T>
inline Matrix<T>& Matrix<T>::AppendCols(std::span<const T> col)
{
    if (nRows==0) return *this; std::uint64_t csize = col.size();
    std::vector<T> NewData; NewData.reserve(nRows*(nCols+1));
    if (csize >= nRows)
    {
	    for (std::uint64_t i{}; i<nRows; ++i)
        {
            NewData.insert(NewData.end(),data.begin()+i*nCols,data.begin()+(i+1)*nCols); NewData.push_back(col[i]);
        }
    }
    else
    {
        for (std::uint64_t i{}; i<nRows; ++i)
        {
            NewData.insert(NewData.end(),data.begin()+i*nCols,data.begin()+(i+1)*nCols); (i<csize)?NewData.push_back(col[i]):NewData.push_back(T{});
        }
    }
    data = std::move(NewData); nCols += 1; return *this;
}
// std::initializer_list
template <Number T>
inline Matrix<T>& Matrix<T>::AppendCols(std::initializer_list<T> col)
{
    if (nRows==0) return *this; std::uint64_t csize = col.size();
    std::vector<T> NewData; NewData.reserve(nRows*(nCols+1));
    if (csize >= nRows)
    {
        for (std::uint64_t i{}; i<nRows; ++i)
        {
            NewData.insert(NewData.end(),data.begin()+i*nCols,data.begin()+(i+1)*nCols); NewData.push_back(col[i]);
        }
    }
    else
    {
        for (std::uint64_t i{}; i<nRows; ++i)
        {
            NewData.insert(NewData.end(),data.begin()+i*nCols,data.begin()+(i+1)*nCols); (i<csize)?NewData.push_back(col[i]):NewData.push_back(T{});
        }
    }
    data = std::move(NewData); nCols += 1; return *this;
}
template <Number T>
inline Matrix<T>& Matrix<T>::AppendCols(std::initializer_list<std::initializer_list<T>> cols)
{
    if (nRows == 0) return *this; std::uint64_t necols = cols.size();
    std::vector<T> NewData; NewData.reserve(nRows*(nCols+necols));
    for (std::uint64_t i{}; i<nRows; ++i)
    {
        NewData.insert(NewData.end(),data.begin()+i*nCols,data.begin()+(i+1)*nCols);
        for (auto& col : cols)
        {
            (i<col.size())?NewData.push_back(*(col.begin()+i)):NewData.push_back(T{});
        }
    }
    data = std::move(NewData); nCols += necols; return *this;
}
// 2D Range
template <Number T> template <Range2D R>
inline Matrix<T>& Matrix<T>::AppendCols(R&& grid)
{
    if (nRows == 0) return *this; std::uint64_t necols = std::ranges::distance(grid);
    std::vector<T> NewData; NewData.reserve(nRows*(nCols+necols));
    for (std::uint64_t i{}; i<nRows; ++i)
    {
        NewData.insert(NewData.end(),data.begin()+i*nCols,data.begin()+(i+1)*nCols);
        for (auto&& col : grid)
        {
            (i<std::ranges::distance(col))?NewData.push_back(*(std::ranges::begin(col)+i)):NewData.push_back(T{});
        }
    }
    data = std::move(NewData); nCols += necols; return *this;
}
template <Number T> template <Number U>
inline Matrix<T>& Matrix<T>::JoinHorizontal(const Matrix<U>& m)
{
    if (nRows==0) return *this; if (m.empty()) return *this; std::vector<T> NewData; NewData.reserve(nRows*(nCols+m.nCols));
    if (m.nRows>=nRows)
    {
        for (std::uint64_t i{}; i<nRows; ++i)
        {
            NewData.insert(NewData.end(),data.begin()+i*nCols,data.begin()+(i+1)*nCols);
            NewData.insert(NewData.end(),m.data.begin()+i*m.nCols,m.data.begin()+(i+1)*m.nCols);
        }
        data = std::move(NewData); nCols += m.nCols; return *this;
    }
    else
    {
        for (std::uint64_t i{}; i<m.nRows; ++i)
        {
            NewData.insert(NewData.end(),data.begin()+i*nCols,data.begin()+(i+1)*nCols);
            NewData.insert(NewData.end(),m.data.begin()+i*m.nCols,m.data.begin()+(i+1)*m.nCols);
        }
        for (std::uint64_t i{m.nRows}; i<nRows; ++i)
        {
            NewData.insert(NewData.end(),data.begin()+i*nCols,data.begin()+(i+1)*nCols);
            NewData.insert(NewData.end(),m.nCols, T{});
        }
        data = std::move(NewData); nCols += m.nCols; return *this;
    }
}

/*****************************
 *           ____      ____  *
 *          / / /     / / /  *
 *        /////     /////    *
 *      / / /     / / /      *
 *    /////     /////        *
 *  |||||     |||||          *
 *    \\\\\     \\\\\        *
 *      \ \ \     \ \ \      *
 *        \\\\\     \\\\\    *
 *          \_\_\     \_\_\  *
 *                           *
 *****************************/
// swap rows
template <Number T>
inline Matrix<T>& Matrix<T>::SwapRows(std::int64_t r1, std::int64_t r2) noexcept
{
    if (nRows <= 1) return *this;
    else
    {
        std::uint64_t wrap_r1 = wrap_index(r1,nRows);
        std::uint64_t wrap_r2 = wrap_index(r2,nRows);
        for (std::uint64_t i{}; i<nCols; ++i)
            std::swap(data[wrap_r1*nCols+i],data[wrap_r2*nCols+i]);
        return *this;
    }
}
template <Number T>
inline Matrix<T> SwapRows(const Matrix<T>& m, std::int64_t r1, std::int64_t r2) noexcept {Matrix<T> nm = m; return nm.SwapRows(r1,r2);}
template <Number T>
inline Matrix<T> SwapRows(Matrix<T>&& m, std::int64_t r1, std::int64_t r2) noexcept {return m.SwapRows(r1,r2);}

// wrap cols
template <Number T>
inline Matrix<T>& Matrix<T>::SwapCols(std::int64_t c1, std::int64_t c2) noexcept
{
    if (nCols <= 1) return *this;
    else
    {
        std::uint64_t wrap_c1 = wrap_index(c1,nCols);
        std::uint64_t wrap_c2 = wrap_index(c2,nCols);
        for (std::uint64_t i{}; i<nRows; ++i)
            std::swap(data[i*nCols+wrap_c1],data[i*nCols+wrap_c2]);
        return *this;
    }
}
template <Number T>
inline Matrix<T> SwapCols(const Matrix<T>& m, std::int64_t c1, std::int64_t c2) noexcept {Matrix<T> nm = m; return nm.SwapCols(c1,c2);}
template <Number T>
inline Matrix<T> SwapCols(Matrix<T>&& m, std::int64_t c1, std::int64_t c2) noexcept {return m.SwapCols(c1,c2);}

// make square
template <Number T>
inline Matrix<T>& Matrix<T>::MakeSquare() noexcept
{
    if (nRows==0 || nCols==0) return *this;
    if (nRows==nCols) return *this;
    else if (nRows<nCols)
    {
        data.resize(nCols*nCols);
        nRows = nCols; return *this;
    }
    else
    {
        data.reserve(nRows*nRows);
        std::uint64_t aberration = nRows - nCols;
        for (std::uint64_t i{}; i<nRows; ++i) data.insert(data.begin()+i*nRows+nCols,aberration,T{});
        nCols = nRows; return *this;
    }
}
template <Number T>
inline Matrix<T> MakeSquare(const Matrix<T>& m) noexcept {Matrix<T> nm(m); return nm.MakeSquare();}
template <Number T>
inline Matrix<T> MakeSquare(Matrix<T>&& m) noexcept {return m.MakeSquare();}

// transpose
template <Number T>
inline Matrix<T>& Matrix<T>::Transpose() noexcept
{
    if (nRows==0 || nCols==0) return *this;
    if (nRows==1 || nCols==1) {std::swap(nRows,nCols); return *this;}
    if (nRows==nCols)
    {
        for (size_t r=0; r<nRows; ++r)
            for (size_t c=r+1; c<nCols; ++c)
                std::swap(data[r*nCols+c],data[c*nRows+r]);
        return *this;
    }
    else
    {
        std::vector<T> tdata(nRows*nCols);
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
template <Number T>
inline Matrix<T> Transpose(const Matrix<T>& m) noexcept {Matrix<T> nm(m); return nm.Transpose();}
template <Number T>
inline Matrix<T> Transpose(Matrix<T>&& m) noexcept {return m.Transpose();}

// resize
template <Number T>
inline Matrix<T>& Matrix<T>::resize(std::uint64_t r, std::uint64_t c)
{
    if (r*c==data.size()) {nRows=r; nCols=c; return *this;}
    else if (r==0 || c==0) {nRows=0; nCols=0; data.clear(); return *this;}
    else {nRows=r; nCols=c; data.resize(r*c); return *this;}
}
template <Number T>
inline Matrix<T> resize(const Matrix<T>& m, std::uint64_t r, std::uint64_t c) noexcept {Matrix<T> nm(m); return nm.resize(r,c);}
template <Number T>
inline Matrix<T> resize(Matrix<T>&& m, std::uint64_t r, std::uint64_t c) noexcept {return m.resize(r,c);}

// reshape
template <Number T>
inline Matrix<T>& Matrix<T>::reshape(std::int64_t r, std::int64_t c)
{
    std::int64_t size = static_cast<std::int64_t>(data.size());
    if (r==0 || c==0) if (size!=0) throw std::runtime_error("Cannot resuce the size to zero while reshaping... Use Matrix<T>::resize()!");
    if (r<-1) r = -r; if (c<-1) c = -c;
    if (r==-1)
    {
        if (c==-1) throw std::runtime_error("Impossible to determine dimensions with both values being -1!");
        r = size / c; if (r*c!=size) throw std::runtime_error("Requested dimension does not match the data size... Couldn't infer nRows!");
    }
    else if (c==-1)
    {
        c = size / r; if (r*c!=size) throw std::runtime_error("Requested dimension does not match the data size... Couldn't infer nCols");
    }
    else
    {
        if (r*c!=size) throw std::runtime_error("Requested dimensions do not match the data size... Use Matrix<T>::resize()!");
    }
    nRows = static_cast<std::uint64_t>(r); nCols = static_cast<std::uint64_t>(c); return *this;
}
template <Number T>
inline Matrix<T> reshape(const Matrix<T>& m, std::int64_t r, std::int64_t c) {Matrix<T> nm(m); return nm.reshape(r,c);}
template <Number T>
inline Matrix<T> reshape(Matrix<T>&& m, std::int64_t r, std::int64_t c) {return m.reshape(r,c);}

// print
template <Number T>
inline void Matrix<T>::printm(std::uint16_t width, std::uint16_t accuracy)
{
    std::print("Matrix, Shape: ({}, {})\n",nRows,nCols);
    if (nRows==0 || nCols==0) return;
    if (data.empty()) {std::print("{}","{}\n"); return;}
    std::print("{{ ");
    if constexpr (std::is_integral_v<T>)
    {
        for (size_t r=0; r<nRows; ++r)
        {
            r==0?std::print("{{ "):std::print("  {{ ");
            for (size_t c=0; c<nCols-1; ++c)
            {
                std::print("{0:^{1}d}, ",data[r*nCols+c],width);
            }
            (r==(nRows-1))?std::print("{0:^{1}d} }} }}\n",data[r*nCols+nCols-1],width):std::print("{0:^{1}d} }},\n",data[r*nCols+nCols-1],width);
        }
    }
    else
    {
        for (size_t r=0; r<nRows; ++r)
        {
            r==0?std::print("{{ "):std::print("  {{ ");
            for (size_t c=0; c<nCols-1; ++c)
            {
                std::print("{0:^{1}.{2}g}, ",data[r*nCols+c],width,accuracy);
            }
            (r==(nRows-1))?std::print("{0:^{1}.{2}g} }} }}\n",data[r*nCols+nCols-1],width,accuracy):std::print("{0:^{1}.{2}g} }},\n",data[r*nCols+nCols-1],width,accuracy);
        }
    }
}

template <Number T>
inline const T Matrix<T>::Trace() const noexcept
{
	T result{}; if (nRows!=nCols) {std::println("Matrix is not square!"); return result;}
	for (std::uint64_t i{}; i<nCols; ++i) result += data[i*nCols+i]; return result;
}

/*********************************
 *                               *
 *                 ///|||\\\     9
 *                /////|\\\\\    9
 *               ////      \\\   9
 *    |||||||||||||||            *
 *               \\\\      ///   *
 *                \\\\\|/////    *
 *                 \\\|||///     *
 *                               *
 *********************************/
// Gauss-Jordan Solver (Linear Equation System) (In Place Solver)
template <FPNumber T>
inline void GaussJordanInPlace(Matrix<T>& SystemMatrix, Matrix<T>& RHSMatrix)
{
	const std::uint64_t NumEqs(SystemMatrix.Rows()); const std::uint64_t NumUnKnowns(RHSMatrix.Rows()); const std::uint64_t NumRhs(RHSMatrix.Cols());
	if (NumEqs!=SystemMatrix.Cols()) {std::print("Not a standard problem! Number of Eqs ({}) is not equal to number of Unknowns ({})!\nTherefore quitting...\n",NumEqs,SystemMatrix.Cols()); return;}
	if (NumEqs!=NumUnKnowns) {std::print("Not a standard problem! Number of Unknowns ({}) is not equal to number of Eqs ({})!\nTherefore quitting...\n",NumUnKnowns,NumEqs); return;}

	// Counting pivots
	std::vector<std::uint32_t> is_pivoted(NumEqs,0);
	// Recording pivot locations
	std::vector<std::uint64_t> row_pivot_indices(NumEqs);
	std::vector<std::uint64_t> col_pivot_indices(NumEqs);
	T pivot_reciprocal; T pivot_value{}; T max_value{}; uint64_t pivot_row{}; std::uint64_t pivot_col{}; T factor{};
	for (std::uint64_t step{}; step<NumEqs; ++step)
	{
		// Finding the largest value to pivot (pivot point)
		max_value = 0.0;
		for (std::uint64_t r{}; r<NumEqs; ++r)
		{
			if (is_pivoted[r]!=1)
			{
				for (std::uint64_t c{}; c<NumEqs; ++c)
				{
					if (is_pivoted[c]==0)
					{
						std::float64_t abs_rc = std::abs(SystemMatrix[r][c]);
						if (abs_rc>max_value)
						{
							max_value = abs_rc; pivot_row = r; pivot_col = c;
						}
					}
				}
			}
		}
		++is_pivoted[pivot_col];
		// Normalize the diagonal
		if (pivot_row!=pivot_col)
		{
			SystemMatrix.SwapRows(pivot_row,pivot_col); RHSMatrix.SwapRows(pivot_row,pivot_col);
		}
		row_pivot_indices[step] = pivot_row; col_pivot_indices[step] = pivot_col;
		pivot_value = SystemMatrix[pivot_col,pivot_col]; 
		if (std::abs(pivot_value)<=std::numeric_limits<T>::epsilon()) {std::println("Impossible to pivot... Quiting gracefully!"); return;}
		pivot_reciprocal = static_cast<T>(1.0/pivot_value);
		for (std::uint64_t c{}; c<NumEqs; ++c) SystemMatrix[pivot_col,c] *= pivot_reciprocal;
		for (std::uint64_t c{}; c<NumRhs; ++c) RHSMatrix[pivot_col,c] *= pivot_reciprocal;
		SystemMatrix[pivot_col][pivot_col] = static_cast<T>(1.0); 

		// Reduce the rows to 0
		for (std::uint64_t r{}; r<NumEqs; ++r)
		{
			if (r!=pivot_col)
			{
				factor = SystemMatrix[r,pivot_col];
				for (std::uint64_t c{}; c<NumEqs; ++c) SystemMatrix[r,c] -= SystemMatrix[pivot_col,c]*factor;
				SystemMatrix[r,pivot_col] = T{};
				for (std::uint64_t c{}; c<NumRhs; ++c) RHSMatrix[r,c] -= RHSMatrix[pivot_col,c]*factor;
			}
		}
	}
}

// Gauss-Jordan Solver (Linear Equation System)
template <typename T>
inline Matrix<T> GaussJordan(const Matrix<T>& SystemMatrix, const Matrix<T>& RHSMatrix)
{Matrix<T> A(SystemMatrix); Matrix<T> b(RHSMatrix); GaussJordanInPlace(A,b); return b;}

// Matrix Multiplication (Contraction)
template <Number T, Number U>
inline Matrix<T> MatMul(const Matrix<T>& m1, const Matrix<U>& m2, bool ForceMultiply=false)
{
	if (ForceMultiply)
	{
		std::uint64_t rows = m1.Rows(); std::uint64_t cols = m2.Cols(); std::uint64_t mid = std::min(m1.Cols(),m2.Rows());
		Matrix<T> result(rows,cols);
		for (std::uint64_t r{}; r<rows; ++r) for (std::uint64_t c{}; c<cols; ++c) for (std::uint64_t m{}; m<mid; ++m) result[r,c] += m1[r,m]*m2[m,c];
		return result;
	}
	else
	{
		std::uint64_t rows = m1.Rows(); std::uint64_t cols = m2.Cols(); std::uint64_t mid = m1.Cols(); std::uint64_t mid2 = m2.Rows();
		Matrix<T> result(rows,cols);
		if (mid!=mid2)
		{
			std::println("Dimension mismatch {} != {}...\nReturning a matrix of zeros of shape ({},{})",mid,mid2,rows,cols);
			return result;
		}
		else
		{
			for (std::uint64_t r{}; r<rows; ++r) for (std::uint64_t c{}; c<cols; ++c) for(std::uint64_t m{}; m<mid; ++m) result[r,c] += m1[r,m]*m2[m,c];
			return result;
		}
	}
}
