
#include "Matrix.hpp"

int main()
{
	Matrix<std::float64_t> A({{1.0,2.22,3.86},{5.2,1.1,-2.0},{2.5,-7.25,0.1}});
	Matrix<std::float64_t> b({{0.2,-0.2},{2.5,-2.5},{-1.2,1.2}});
	std::println("Before applying GaussJordan");
	std::println("A"); A.printm(5,3); std::println("b"); b.printm(5,3);
	Matrix<std::float64_t> x = GaussJordan(A,b);
	// GaussJordanInPlace(A,b);
	std::println("After applying GaussJordan");
	std::println("A"); A.printm(5,3); std::println("b"); b.printm(5,3);
	std::println("x"); x.printm();
	std::println("Ax"); Matrix<std::float64_t>(MatMul(A,x)).printm();
	Matrix<std::float64_t> A_nill({{5.0,2.0,0.1},{1.0,2.0,4.0},{0.5,1.0,2.0}});
	Matrix<std::float64_t> x_nill = GaussJordan(A_nill,b); std::println("x_nill"); x_nill.printm();
}
