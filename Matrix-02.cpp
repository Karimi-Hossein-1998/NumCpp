#include "Matrix.hpp"

template <typename T>
inline std::vector<T> ret_vec(std::uint64_t N, T value) {return std::vector<T>(N,value);}

int main()
{
    std::uint64_t N {10}, M{10};
    Matrix<double> m1(N,M,2.123456789);
    std::println("m1 (floating point type)");
    m1.printm(10,6);

    for (std::uint64_t i{}; i<N; ++i)
    {
        for (std::uint64_t j{}; j<M; ++j)
        {
            m1[i,j] = (i+1)*(j+1);
        }
    }
    std::println("Changed m1");
    m1.printm(5);

    Matrix<int> m2(N,M,0);
    for (std::uint64_t i{}; i<N; ++i)
    {
        for (std::uint64_t j{}; j<M; ++j)
        {
            m2[i][j] = (i+1)*(j+1);
        }
    }
    std::println("m2");
    m2.printm(5);

    int m2_11 = m2(-9,-9);
    // int m2_11 = m2(-9][]-9); //This will cause an error!
    std::println("m2({},{}) = {}",-9,-9,m2_11);

    Slice rs(N-1,-1,-2);
    Slice cs(M-1,-1,-2);
    // for (std::uint64_t i{}; i<rs.Size(); ++i) std::println("{}", rs[i]);
    Matrix<double> m1sliced(m1[rs,cs]);
    std::println("Sliced m1");
    m1sliced.printm(3);
    rs = Slice(2*N-1,-1-N,-5);
    cs = Slice(2*M-1,-1-M,-2);
    // Matrix<double> m1sliced_repeated(m1[rs,cs]); // Error
    Matrix<double> m1sliced_repeated(m1(rs,cs));
    std::println("Sliced repeated m1");
    m1sliced_repeated.printm(3);
}
