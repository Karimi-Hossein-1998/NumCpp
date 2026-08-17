#include "include/NumCpp.hpp"


int main()
{
    NVec<double> nv1(Ranges<double>(0.1,1.1,0.1).range());
    NVec<double> nv2=nv1[Slice(1,5,1)]; std::cout << "V1[1:5]\n";
    nv2.printv();
    std::cout << "V1[-1] = " << nv1[-1] << "\nV1\n";
    nv1.printv();
    Vec<double> v1 = Ranges<double>(2.5,7.5,0.5).range();
    NVec<double> nv3 = nv1+NVec(v1);
    std::cout << "V3[7:4:-1]\n"; nv3[Slice(7,4,-1)].printv();
    std::cout << "V3\n"; nv3.printv();
    return 0;
}
