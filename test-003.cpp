#include "include/NumCpp.hpp"

int main()
{
	NMatrix<double> nm(10,10,1.0);
	nm.append_cols(5.0);
	nm.printm();
	NMatrix nm1(10,11,2.0);
	NMatrix nm2 = nm1/nm;
	nm2.printm();
	return 0;
}
