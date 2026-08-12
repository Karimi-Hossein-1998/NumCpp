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
    return 0;
}
