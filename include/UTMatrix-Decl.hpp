#pragma once
#include "Declarations.hpp"

// UTMatrix (Upper Triangular Matrix Class)
template <Number T>
class UTMatrix
{
	using value_type = T;
	private:
		std::uint64_t nDim{};
		std::vector<T> data;
		std::string name_of_the_type = get_type_name<value_type>();
	public:
		/*******************
		*                 *
		*        X        *
		*       /X\       *
		*     ///X\\\     *
		*   /////X\\\\\   *
		*                 *
		*******************/
		UTMatrix() = default;
		UTMatrix(const UTMatrix&) = default;
		UTMatrix(UTMatrix&&) noexcept = default;
		explicit UTMatrix(std::uint64_t n, T value=T{}) : nDim(n), data(n*(n+1)/2,value) {}
		UTMatrix(std::span<const T> span_)
		{
			std::uint64_t ssize{span_.size()};
			data(span_.begin(),span_.end());
			std::uint64_t rMin{}, rMax{1}; while(rMin*rMax<(ssize<<1)) {++rMin; ++rMax;} nDim = rMax;
			if (((rMin*rMax)>>1)!=ssize) {data.reserve((rMin*rMax)>>1); data.insert(data.end(),((rMin*rMax)>>1)-ssize,T{});}
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
		UTMatrix& operator=(const UTMatrix&) = default;
		UTMatrix& operator=(UTMatrix&&) noexcept = default;
		UTMatrix& operator=(T value) {for (std::uint64_t i{}; i<data.size(); ++i) data[i]=value; return *this;}
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
			if (i>=nDim) throw std::runtime_error(std::to_string(i)+" out of bound!");
			if (j>=nDim) throw std::runtime_error(std::to_string(j)+" out of bound!");
			if (i>j) throw std::runtime_error("Cannot assign values to zero upper triangles!");
			else return data[((((nDim<<1)-i+1)*i)>>1)+j-i];
		}
		inline const T& operator[](std::uint64_t i, std::uint64_t j) const
		{
			if (i>=nDim) throw std::runtime_error(std::to_string(i)+" out of bound!");
			if (j>=nDim) throw std::runtime_error(std::to_string(j)+" out of bound!");
			if (i>j) throw std::runtime_error("Cannot assign values to zero upper triangles!");
			else return data[((((nDim<<1)-i+1)*i)>>1)+j-i];
		}
		// Matrix[i]
		inline std::span<T> operator[](std::uint64_t i)
		{
			if (i>=nDim) throw std::runtime_error(std::to_string(i)+" is out of bound");
			std::span<T> spanData {data}; return spanData.subspan((i*((nDim<<1)-i+1))>>1,nDim-i);
		}
		inline std::span<const T> operator[](std::uint64_t i) const
		{
			if (i>=nDim) throw std::runtime_error(std::to_string(i)+" is out of bound");
			std::span<T> spanData {data}; return spanData.subspan((i*((nDim<<1)-i+1))>>1,nDim-i);
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
		// info
		[[nodiscard]] inline std::uint64_t Rows() const noexcept { return nDim; }
		[[nodiscard]] inline std::uint64_t Cols() const noexcept { return nDim; }
		[[nodiscard]] inline std::uint64_t size() const noexcept { return data.size(); }
		[[nodiscard]] inline std::pair<std::uint64_t,std::uint64_t> Shape() const noexcept { return {nDim,nDim}; }
		[[nodiscard]] inline bool empty() const noexcept { return data.empty(); }
		[[nodiscard]] inline T* ptr() noexcept { return data.data(); }
		[[nodiscard]] inline const T* ptr() const noexcept { return data.data(); }

		inline void printm(std::uint16_t width=10, std::uint16_t accuracy=8) const;
		inline Matrix<T> ToDense() const;
};

// print
template <Number T>
inline void UTMatrix<T>::printm(std::uint16_t width, std::uint16_t accuracy) const
{
	std::print("UTMatrix ({}), Shape: ({}, {})\n",name_of_the_type,nDim,nDim);
	if (nDim==0) return;
	if (data.empty()) {std::print("{}","{}\n"); return;}
	std::print("{{");
	if constexpr (std::is_integral_v<T>)
	{
		for (size_t r=0; r<nDim; ++r)
		{
			r==0?std::print(" {{"):std::print("  {{");
			for (size_t c=0; c<nDim-1; ++c)
			{
				std::print(" {0:^{1}d},",r>c?T{}:data[((((nDim<<1)-r+1)*r)>>1)+c-r],width);
			}
			(r==(nDim-1))?std::print(" {0:^{1}d} }} }}\n",r>(nDim-1)?T{}:data[((((nDim<<1)-r+1)*r)>>1)+nDim-1-r],width):std::print(" {0:^{1}d}}},\n",r>(nDim-1)?T{}:data[((((nDim<<1)-r+1)*r)>>1)+nDim-1-r],width);
		}
	}
	else
	{
		for (size_t r=0; r<nDim; ++r)
		{
			r==0?std::print(" {{"):std::print("  {{");
			for (size_t c=0; c<nDim-1; ++c)
			{
				std::print(" {0:^{1}.{2}g},",r>c?T{}:data[((((nDim<<1)-r+1)*r)>>1)+c-r],width,accuracy);
			}
			(r==(nDim-1))?std::print(" {0:^{1}.{2}g} }} }}\n",r>(nDim-1)?T{}:data[((((nDim<<1)-r+1)*r)>>1)+nDim-1-r],width,accuracy):std::print(" {0:^{1}.{2}g} }},\n",r>(nDim-1)?T{}:data[((((nDim<<1)-r+1)*r)>>1)+nDim-1-r],width,accuracy);
		}
	}
}

