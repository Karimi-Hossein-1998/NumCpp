#include "include/NumCpp.hpp"

Matrix<double> make_matrix(size_t n) {Matrix<double> m(n,n,1.0); return m;}

int main()
{
    Matrix m({{1.0,2.0},{4.0},{7.0,8.0,9.0}});
    m.printm();
    Matrix mm(10,10,100.0);
    Matrix copym = make_matrix(100);
    mm = make_matrix(5);
    mm.printm();
    Matrix mNew = mm;
    mNew.printm();
    mNew = m;
    mNew.printm();
    for (size_t i=0; i<100; ++i)
    	for (size_t j=0; j<100; ++j)
            copym[i][j] = (i+1)*(j+1);
    NMatrix nm = copym;
    NMatrix nmslice = nm[Slice(99,-101,-50),Slice(99,-101,-50)];
    nm.printm();
    nmslice.printm();
    return 0;
}
