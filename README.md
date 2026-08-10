# NumPy Style **Vec**tor and **Matrix**

This is an attempt to recreate numpy style vectors and matrices.

**Examples:**

+ `Vec v(N,0.0); std::vector<double> stdv(N,0.0); \\ Simple std::vector wrapper`
+ `v=stdv;`
+ `Vec v2(N,0.0); v2=v;`
+ `NVec nv(N,0.0); \\nv={0.1,0.2,0.3,....,0.1*N};`
+ `printf("nv[%d] = %g", -5,nv[-5]);\\NumPy Style indexing`
+ `Matrix m(N,N,0.0); std::vector<std::vector<double>> stdm(N,std::vector<double>(N,0.0));\\ flat std wrapper (slightly faster in large sizes)`
+ `for (size_t i=0; i<N; ++i) m[i] = nv;`
+ `Vec v3(N,0.0); v3=m[0];`
+ `NMatrix nm(N,N,0.0); nm = 5.0;\\NumPy Style indexing`
+ `printf("nm [%d][%d] = %g",-10,-5,nm[-10][-5]);\\printf("nm [%d,%d] = %g",-10,-5,nm[-10,-5]);`
+ `NMatrix nm1(N,N,0.0); nm1 = m;` 

You can assign `Vec` to `NVec` and vice versa, you caan assign a `initializer_list` to `Vec` and `NVec`, you can assign `Vec` and `NVec` to `Matrix` and `NMatrix` *rows* and vice versa, you can assign a singular number to all (sets all elements as the given number, like the constructor), you can assign `Matrix` to `NMatrix` and vice versa.

There are also `resize` and `reshape` functions for both `Matrix` and `NMatrix` types.

```C++
#include "include/NumCpp.hpp"
#include <time.h>
int main()
{
	size_t N = 10;
	Vec<double> v(N,0.0);
	for (size_t i=0; i<N; ++i)
	{
		v[i] = static_cast<double>(i+1);
		printf("v[%zu] = %g\n", i%N, v[i]);
	}
	Matrix<double> m(2*N,N,0.0);
    NMatrix<double> nm(2*N,N,1.0);
    for (size_t i=0; i<2*N; ++i)
    {
        m[i] = v;
        nm[-(i+1)] = v;
        for (size_t j=0; j<N-1; ++j)
        {
            m[i][j] = (v[i%N]+(i/N)*v[N-1])*v[j];
            nm[-(i+1),-(j+1)] = (v[i%N]+(i/N)*v[N-1])*v[j];
            printf("%g\t",m[i][j]);
        }
        m[i][N-1] = (v[i%N]+(i/N)*v[N-1])*v[N-1];
        nm[-(i+1)][-N] = (v[i%N]+i/N*v[N-1])*v[N-1];
        printf("%g\n",m[i][N-1]);
    }
    for (size_t i=0; i<2*N; ++i)
    {
        for (size_t j=0; j<N-1; ++j)
        {
            printf("%g\t",nm[i][j]);
        }
        printf("%g\n",nm[i][N-1]);
    }
    Vec v2(N,0.0), v3(N,0.0), v4(N,0.0), v5(N,0.0), v6(N,0.0), v7(N,0.0);
    NVec nv2(N,3.14159565);
    std::vector<double> stdv(N,10.0);
    v2 = m[N-1];
    v3 = nm[-1];
    v4 = stdv;
    v5 = v3;
    v6 = nv2;
    v7 = {0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0};
    for (size_t i=0; i<N; ++i)
    {
        printf("V2[%zu] = %g\n",i,v2[i]);
        printf("V3[%zu] = %g\n",i,v3[i]);
        printf("V4[%zu] = %g\n",i,v4[i]);
        printf("V5[%zu] = %g\n",i,v5[i]);
        printf("V6[%zu] = %g\n",i,v6[i]);
        printf("V7[%zu] = %g\n",i,v7[i]);
    }
    NVec nv3(N,0.0),nv4(N,0.0),nv5(N,0.0),nv6(N,0.0),nv7(N,0.0);
	nv3=v7;nv4=m[2];nv5=nm[-21];nv6=nv2;
	nv7={-0.1,-0.2,-0.3,-0.4,-0.5,-0.6,-0.7,-0.8,-0.9,-1.0};
	for (size_t i=0; i<N; ++i)
    {
        printf("NV2[%d] = %g\n",-i-1,nv2[-(i+1)]);
        printf("NV3[%d] = %g\n",-i-1,nv3[-(i+1)]);
        printf("NV4[%d] = %g\n",-i-1,nv4[-(i+1)]);
        printf("NV5[%zu] = %g\n",i,nv5[i]);
        printf("NV6[%zu] = %g\n",i,nv6[i]);
        printf("NV7[%zu] = %g\n",i,nv7[i]);
    }
    return 0;
}
```
