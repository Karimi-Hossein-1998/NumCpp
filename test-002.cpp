#include "include/NumCpp.hpp"

template<typename T>
NVec<T> return_nvec(size_t n, T value)
{
	return NVec<T>(n,value)+value;
};

int main()
{
    Vec v1 = return_nvec(10,1.5);
    v1.printv();
    NVec nv1 = return_nvec(10,2.25);
    nv1.printv();
    Vec v2 = nv1;
    v2.printv();
    nv1.printv();

    NVec<std::string> nv2 = return_nvec(10,std::string("Hello"));
    Vec<std::string> v3(nv2);
    nv2.printv();
    v3.printv();
    Vec v4=v2/0.0;
    v4.printv();
    return 0;
}
