#include "Matrix.hpp"
#include <stdfloat>

template <typename T>
inline std::vector<T> ret_vec(std::uint64_t N, T value) {return std::vector<T>(N,value);}

int main()
{
    std::uint64_t N {4}, M{5};
    Matrix<std::float64_t> m1(N,M,2.123456789);

    for (std::uint64_t i{}; i<N; ++i)
    {
        for (std::uint64_t j{}; j<M; ++j)
        {
            m1[i,j] = (i+1)*(j+1);
        }
    }
    std::println("m1 (floating point type)"); m1.printm(10,6);

    Matrix<std::float64_t> m2 = SwapRows(Transpose(SwapCols(m1,1,4)),2,3);
    std::println("m2 m1->swap cols 1,4-> transpose -> swap rows 2,3"); m2.printm(5,4);

    m2.AppendRows({{1.0,2.0,3.0,4.0}, {5.0,6.0,7.0}});
    std::println("m2 (added rows)"); m2.printm(5,4);

    std::vector<std::vector<std::float64_t>> stdMat1 = {{-1.0,-2.0,-3.0,-4.0,-5.0},{-6.0,-7.0,-8.0,-9.0,-10.0}};
    m1.AppendRows(stdMat1);
    std::println("m1 (added rows)"); m1.printm(10,6);
    std::println("m1.Shape() = {}", m1.Shape());

    m1.AppendCols({{-1.0,-4.0,-7.0,-10.0,-13.0,-16.0,-19.0},{-2.0,-5.0,-8.0},{-3.0,-6.0,-9.0,-12.0,-15.0,-18.0}});
    std::println("m1 (added cols)"); m1.printm(10,6);
    std::println("m1.Shape() = {}",m1.Shape());

    m1.AppendCols(stdMat1);
    std::println("m1 (added another set of columns)"); m1.printm(10,6);
    std::println("m1.Shape() = {}",m1.Shape());

    m1.AppendCols(1.25,5);
    std::println("m1 (added columns of 1.25)"); m1.printm(5,5);
    std::println("m1.Shape() = {}",m1.Shape());

    Matrix<std::float64_t> m3(m2*m2);
    std::println("m3 = m2*m2"); m3.printm(10,6);

    m1.JoinHorizontal(m1);
    std::println("m1 join m1 (horizontal)"); m1.printm(5,5);
    std::println("m1.Shape() = {}",m1.Shape());

}
