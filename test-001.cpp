#include "include/NumCpp.hpp"


int main()
{
    Vec v1 = {0.1,0.1,0.2,0.3,0.5,0.8,1.3,2.1};v1+=1.0;
    Vec<int> v2 = {1,1,2,3,5,8,13,21};
    Vec v3 = v1+v2;
    std::cout << "V3 = V1+V2\n"; v3.printv();
    Vec v4 = v1-v2;
    std::cout << "V4 = V1-V2\n"; v4.printv();
    Vec v5 = v1*v2;
    std::cout << "V5 = V1*V2\n"; v5.printv();
    Vec v6 = v1/v2;
    std::cout << "V6 = V1/V2\n"; v6.printv();
    Vec<int> v7 = v2+v1;
    std::cout << "V7 = V2+V1\n"; v7.printv();
    Vec<int> v8 = v2-v1;
    std::cout << "V8 = V2-V1\n"; v8.printv();
    Vec<int> v9 = v2*v1;
    std::cout << "V9 = V2*V1\n"; v9.printv();
    Vec<int> v10 = v2/v1;
    std::cout << "V10 = V2/V1\n"; v10.printv();
    auto dot1Product2 = dot(v1,v2);
    std::cout << "V1 . V2 = " << dot1Product2 << '\n';
    auto dot2Product1 = dot(v2,v1);
    std::cout << "V2 . V1 = " << dot2Product1 << '\n';
    auto dot3Product5 = v3.dot(v5);
    std::cout << "V3 . V5 = " << dot3Product5 << '\n';
    return 0;
}
