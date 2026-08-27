#include "Matrix.hpp"
#include <stdfloat>

template <typename T>
inline std::vector<T> ret_vec(std::uint64_t N, T value) {return std::vector<T>(N,value);}

int main()
{
    std::uint64_t N {4}, M{5};
    Matrix<std::float64_t> m1(N,M,2.123456789);
    std::println("m1 (floating point type)");
    m1.printm(10,6);

    for (std::uint64_t i{}; i<N; ++i)
    {
        for (std::uint64_t j{}; j<M; ++j)
        {
            m1[i,j] = (1.1*i+1)*(1.1*j+1);
        }
    }
    std::println("Changed m1");
    m1.printm(5,4);

    Matrix<std::float64_t> m2 = m1;
    std::println("m2 = m1"); m2.printm(5,4);

    Matrix<std::float64_t> m3 = m1; m3.resize(2*N,2*M);
    std::println("m3 = m1 (resize({},{}))", 2*N,2*M); m3.printm(5,4);

    m1.reshape(-1,2); std::println("m1.reshape(-1,5)"); m1.printm(5,4);

    Matrix<std::float64_t> m4 = m3 + 1LLU;
    std::println("m4 = m3 + 1LLU"); m4.printm(5,4);
    std::println("m1.empty() = {}", m1.empty());
    std::println("m1.size() = {}", m1.size());
    std::println("m1.Rows() = {}",m1.Rows());
    std::println("m1.Cols() = {}",m1.Cols());
    std::println("m1.Shape() = {}",m1.Shape());

    Matrix<std::float64_t> m5(Transpose(m1));
    std::println("m5= Transpose(m1)"); m5.printm(5,4);

    Matrix<std::float64_t> m6(MakeSquare(m1));
    std::println("m6 = MakeSquare(m1)"); m6.printm(5,4);
}
