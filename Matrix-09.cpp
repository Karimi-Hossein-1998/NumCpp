#include "include/Matrix.hpp"

#if defined(__SIZEOF_INT128__)
constexpr __int128_t operator""_i128(const char* str)
{
	__int128_t result;
	bool negative = false;
	if (*str=='-') {negative=true; ++str;}
	while (*str)
	{
		if (*str>='0' && *str<='9')
			result = result*10 + (*str - '0');
		++str;
	}
	return negative?-result:result;
}
constexpr __int128_t operator""_i128(const char* str, std::uint64_t len)
{
	__int128_t result;
	bool negative = false;
	std::uint64_t i{};
	if (len>0 && str[0]=='-') {negative=true; ++i;}
	for (;i<len; ++i)
		if (str[i]>='0' && str[i]<='9')
			result = result*10 + (str[i]-'0');
	return negative?-result:result;
}
#endif


int main()
{
	Matrix<std::float64_t> A({{1.0,2.22,3.86},{5.2,1.1,-2.0},{2.5,-7.25,0.1}});
	Matrix<std::float64_t> b({{0.2,-0.2},{2.5,-2.5},{-1.2,1.2}});
	std::println("Before LU-decomposition");
	std::println("A"); A.printm(5,3); std::println("b"); b.printm(5,3);
	auto [L,U] = LUDecomposeNormal(A);
	std::println("L"); L.printm();
	std::println("U"); U.printm();
	auto A_lu = MatMul(L,U);
	std::println("A = LU"); A_lu.printm();

	Matrix<std::float64_t> x(LUSolveNormal(L,U,b));
	LUSolveInPlaceNormal(L,U,b);
	std::println("b (After solving with LU decomposition)"); b.printm();
	std::println("Ax"); MatMul(A,b).printm();
	std::println("x (Ax=b)"); x.printm();	

	auto AInv(Matrix<std::float64_t>::I(3));
	LUSolveInPlaceNormal(L,U,AInv);
	std::println("AInv"); AInv.printm();
	std::println("A.AInv"); MatMul(A,AInv).printm();
	auto AInverse(A.Inverse());
	std::println("A Inverse"); AInverse.printm();

	auto det{A.Determinant()};
	std::println("Det(A) is {}", det);

	// Matrix<__int128_t> matint({{"1"_i128,"12"_i128,"123"_i128,"1234"_i128,"12345"_i128,"123456"_i128,"1234567"_i128},
	// {"12345678"_i128,"123456789"_i128,"1234567890"_i128,"12345678901"_i128,"123456789012"_i128,"1234567890123"_i128,"12345678901234"_i128},
	// {"123456789012345"_i128,"1234567890123456"_i128,"12345678901234567"_i128,"123456789012345678"_i128,"1234567890123456789"_i128,"12345678901234567890"_i128,"123456789012345678901"_i128},
	// {"1234567890123456789012"_i128,"12345678901234567890123"_i128,"123456789012345678901234"_i128,"1234567890123456789012345"_i128,"12345678901234567890123456"_i128,"123456789012345678901234567"_i128,"1234567890123456789012345678"_i128}});
	Matrix<__int128_t> matint({{1_i128,12_i128,123_i128,1234_i128,12345_i128,123456_i128,1234567_i128},
							  {12345678_i128,123456789_i128,1234567890_i128,12345678901_i128,123456789012_i128,1234567890123_i128,12345678901234_i128},
							  {123456789012345_i128,1234567890123456_i128,12345678901234567_i128,123456789012345678_i128,1234567890123456789_i128,12345678901234567890_i128,123456789012345678901_i128},
							  {1234567890123456789012_i128,12345678901234567890123_i128,123456789012345678901234_i128,1234567890123456789012345_i128,12345678901234567890123456_i128,123456789012345678901234567_i128,1234567890123456789012345678_i128}});
	matint.printm(30);
}
