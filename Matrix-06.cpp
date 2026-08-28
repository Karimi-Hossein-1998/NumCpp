#include "include/Matrix.hpp"
#include <stdfloat>

template <Number T>
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

    Matrix<std::float64_t> m3(m2-m1);
    std::println("m3 = m2-m1"); m3.printm(8,4);

    Matrix<std::float64_t> m4(abs(m3,false));
    std::println("m4 = abs(m3)"); m4.printm(8,4);

    Matrix<std::float64_t> m5(power(m1,-2,false));
    std::println("m5 = m1^-2"); m5.printm(10,4);

	Matrix<std::float64_t> m6(power(m3,-2,false));
    std::println("m6 = m3^-1.5"); m6.printm(10,4);

    Matrix<std::float64_t> m7(sin(m3,false));
    std::println("m7 = sin(m3)"); m7.printm(10,4);

    Matrix<std::float64_t> m8(cos(m3,false));
    std::println("m8 = cos(m3)"); m8.printm(10,4);
}
