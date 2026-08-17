#include "include/NumCpp.hpp"

int main()
{
	Matrix<double> m1({{0.0,1.0,2.0},{2.0,1.0},{0.0,2.0,1.0}});
	std::cout << "M1\n"; m1.printm();
	Matrix<double> m2({{0.5},{1.0,0.5},{1.5,1.0,0.5}});
	std::cout << "M2\n"; m2.printm();
	Matrix<double> m3({{0.1,0.2,0.3,0.4},{8.0,7.0,6.0,5.0}});
	std::cout << "M3\n"; m3.printm();
	Matrix<double> m4 = matmul(m2,m3,true);
	std::cout << "M4 = M2 M3 (Forced multiply)\n"; m4.printm();
	Matrix<double> m5 = m3; m5.append_rows({0.25,0.5,0.75,1.0});
	std::cout << "M5\n"; m5.printm();
	return 0;
}
