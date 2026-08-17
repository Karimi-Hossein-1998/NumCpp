#include "include/NumCpp.hpp"

int main()
{
    NMatrix<double> m1 = std::vector<std::vector<double>>({Ranges(1.25,3.75,0.25).range(),
                        									Ranges(0.25,2.75,0.25).range(),
									                        Ranges(-0.75,1.75,0.25).range()});
	std::cout << "M1\n"; m1.printm();
    NMatrix<double> m2 =  {{0.0,1.5,3.0},
        					{-1.0,0.5,2.0},
                        	{-2.0,-0.5,1.0},
        					{-3.0,-1.5,0.0},
        					{-4.0,-2.5,-1.0},
                            {-5.0,-3.5,-2.0},
                            {-6.0,-4.5,-3.0},
                            {-7.0,-5.5,-4.0},
                            {-8.0,-6.5,-5.0},
                            {-9.0,-7.5,-6.0}};
	std::cout << "M2\n"; m2.printm();
	NMatrix m3 = NMatrix<double>(matmul(Matrix<double>(m1),Matrix<double>(m2)));
    std::cout << "M3 = M1 M2\n"; m3.printm();
    NMatrix m4 = NMatrix<double>(matmul(Matrix<double>(m2),Matrix<double>(m1)));
    std::cout << "M4 = M2 M1\n"; m4.printm();
    std::cout << "M4[-1:0:-2,-1:0:-2]\n"; m4[Slice(9,-1,-2),Slice(9,-1,-2)].printm();

    std::cout << "M4[-25,35] = " << m4[-25,35] << "\n";
	return 0;
}
