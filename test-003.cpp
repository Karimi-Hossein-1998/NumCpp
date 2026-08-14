#include "include/NumCpp.hpp"

int main()
{
	NMatrix<double> nm(10,10,1.0);
	nm.append_cols(5.0);
	nm.printm();
	return 0;
}
