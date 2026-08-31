#include "Matrix.hpp"

int main()
{
	Matrix<std::float64_t> A({{1.0,2.22,3.86},{5.2,1.1,-2.0},{2.5,-7.25,0.1}});
	Matrix<std::float64_t> b({{0.2,-0.2},{2.5,-2.5},{-1.2,1.2}});
	std::println("Before LU-decomposition");
	std::println("A"); A.printm(5,3); std::println("b"); b.printm(5,3);
	auto [L,U] = LUDecompose(A);
	std::println("L"); L.printm();
	std::println("U"); U.printm();
	auto A_lu = MatMul(L,U);
	std::println("A = LU"); A_lu.printm();

	Matrix<std::float64_t> x(LUSolve(L,U,b));
	LUSolveInPlace(L,U,b);
	std::println("b (After solving with LU decomposition)"); b.printm();
	std::println("Ax"); MatMul(A,b).printm();
	std::println("x (Ax=b)"); x.printm();	

	auto AInv(Matrix<std::float64_t>::I(3));
	LUSolveInPlace(L,U,AInv);
	std::println("AInv"); AInv.printm();
	std::println("A.AInv"); MatMul(A,AInv).printm();
	auto AInverse(A.Inverse());
	std::println("A Inverse"); AInverse.printm();
}
