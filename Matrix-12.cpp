#include "include/Matrix.hpp"
#include "include/LTMatrix.hpp"
#include "include/UTMatrix.hpp"
#include "include/TDMatrix.hpp"

int main()
{
	std::print("Hello TDMatrix\n");
	TDMatrix<double> TD1(10,2.0);
	std::println("Tri-Diagonal Matrix #1"); TD1.printm(5,3);

	TDMatrix<size_t> TD2(10,1,2,3);
	std::println("Tri-Diagonal Matrix #2"); TD2.printm(5,3);

	std::vector<float> lower{{0.9,1.9,2.9,3.9}};
	std::vector<float> diag{{1.1,2.1,3.1,4.1,5.1}};
	std::vector<float> upper{{1.3,2.3,3.3,4.3}};
	TDMatrix<float> TD3(lower,diag,upper);
	std::println("Tri-Diagonal Matrix #3"); TD3.printm(5,3);

	TDMatrix<int> TD4(0,1,2,3);
	std::println("Tri-Diagonal Matrix #4"); TD4.printm(5,3);

	TDMatrix<int> TD5(1,1,2,3);
	std::println("Tri-Diagonal Matrix #5"); TD5.printm(5,3);

	TDMatrix<std::float64_t> A(4,1.0,2.0,3.0);
	std::println("A"); A.printm(5,3);
	Matrix<std::float64_t> b({{0.2,-0.2},{2.5,-2.5},{-1.2,1.2},{-7.23,7.23}});
	std::println("b"); b.printm(5,3);
	auto x = TDSolve(A,b);
	std::println("x"); x.printm(8,4);
	std::println("Ax"); (MatMul(A.ToDense(),x)).printm(8,4);

	// Matrix<std::float64_t> x(LUSolve(L,U,b));
	// std::println("x (Ax=b)"); x.printm();
	// Matrix<std::float64_t> xn(LUSolveNormal(LN,UN,b));
	// std::println("x (Ax=b) (Using normal matrices, not specialized LT and UT matrices)"); xn.printm();
	// // LUSolveInPlace(L,U,b);
	// // std::println("b (After solving with LU decomposition)"); b.printm();
	// // std::println("Ax"); MatMul(A,b).printm();

	// // auto AInv(Matrix<std::float64_t>::I(3));
	// // LUSolveInPlace(L,U,AInv);
	// // std::println("AInv"); AInv.printm();
	// // std::println("A.AInv"); MatMul(A,AInv).printm();
	// auto AInverse(A.Inverse());
	// std::println("A Inverse"); AInverse.printm();

	// auto det{A.Determinant()};
	// std::println("Det(A) is {}", det);

}
