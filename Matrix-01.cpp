#include "Matrix.hpp"
#include <stdfloat>

template <typename T>
inline std::vector<T> ret_vec(std::uint64_t N, T value) {return std::vector<T>(N,value);}

int main()
{
    Matrix<double> m1(10,10,2.123456789);
    std::println("m1 (floating point type)");
    m1.printm(10,6);

    Matrix<std::uint16_t> m2(10,10,65'000);
    std::println("m2 (integral type)");
    m2.printm(7,5);

    Matrix<double>  m1_5(m1[5]);
    auto m2_5 = m2[5];
    std::span<const std::uint16_t> m2_6 = m2[6];
    for (auto& value : m1[5]) value +=1.75;
    std::print("Checking to see if the copy has changed!\n");
    m1_5.printm(12,10);
    for (auto& value : m1[7]) value-=1.75;
    std::println("m1, after trying to manipulate sixth row.");
    m1.printm(12, 10);
	std::vector<int> m3v {1,2,3,4,5,6,7,8,9,10};
    std::span<int> m3s{m3v};

    Matrix<int> m3(m3s,false);
    std::println("m3");
    m3.printm(12);
    int i=5; int j = 5;
    m1[i][j] = 25.52;
    double m1ij = m1[i][j];

    std::vector<std::float64_t> v4{0.1,0.01,0.001,0.0001,0.00001};
    Matrix<std::float64_t> m4(v4);
    std::println("m4");
    m4.printm(8,7);
    std::print("m1[{},{}] = {}\n",i,j,m1ij);

    // std::vector<std::int64_t> vtest = ret_vec(11,static_cast<std::int64_t>(25));
    // std::println("{}",vtest);
    Matrix<std::int64_t> m5(ret_vec(11,static_cast<std::int64_t>(25)));
    std::println("m5");
    m5.printm(4);

    std::vector<std::vector<int>> nested_vec {{1,2,3},{4,5},{7}};
    Matrix m6(nested_vec);
    std::println("m6");
    m6.printm(3);
    return 0;
}
