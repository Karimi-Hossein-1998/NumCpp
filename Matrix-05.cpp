#include "Matrix.hpp"
#include <stdfloat>

template <typename T>
inline std::vector<T> ret_vec(std::uint64_t N, T value) {return std::vector<T>(N,value);}

int main()
{
    std::uint64_t N {10}, M{10};
    Matrix<std::float64_t> m1(N,M,2.123456789);

    for (std::uint64_t i{}; i<N; ++i)
    {
        for (std::uint64_t j{}; j<M; ++j)
        {
            m1[i,j] = (i+j+1)*(j+1);
        }
    }
    std::println("m1 (floating point type)"); m1.printm(5,4);

    Matrix<std::float64_t> m2(ApplyFunc(m1,[](std::float64_t x) -> std::float64_t {return 1.0/x;},false));
    std::println("m2 = sin(m1)"); m2.printm(8,4);
}
