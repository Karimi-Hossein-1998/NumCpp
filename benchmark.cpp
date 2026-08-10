#include "include/NumCpp.hpp"
#include <chrono>
#include <cstddef>
#include <iostream>
#include <numeric>

struct Timer
{
	std::chrono::high_resolution_clock::time_point start;
	const char* label;

	Timer(const char* name) : label(name), start(std::chrono::high_resolution_clock::now()) {}
	~Timer()
	{
		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double,std::milli> ms = end - start;
		std::cout << " " << label << " : " << ms.count() << " ms\n";
	}
};

int main()
{
    constexpr size_t repeat = 10000;
	constexpr size_t ROW = 1000;
	constexpr size_t COL = 1000;
	std::cout << "===== RUNNING BENCHMARK (" << ROW << " x " << COL << ") elements =====\n\n";
    std::cout << "[1] Allocation and Construction Overhead:\n";
    {Timer t("Flat Matrix<double>"); for (size_t c=0; c<repeat; ++c) Matrix m(ROW,COL,1.0);}
    {Timer t("std::vector<std::vector<double>>"); for (size_t c=0; c<repeat; ++c) std::vector<std::vector<double>> vm(ROW,std::vector<double>(COL,1.0));}
    {
        Timer t("Rae double** Pointer Array");
        for (size_t c=0; c<repeat; ++c)
        {
            double** rm = new double* [ROW];
            for (size_t i=0; i<ROW; ++i)
            {
                rm[i] = new double [COL];
                std::fill_n(rm[i],COL,1.0);
            }
            for (size_t i=0; i<ROW; ++i) delete [] rm[i];
            delete [] rm;
        }
    }

    std::cout << "\n[2] Element Access and Modification Loop (m[i][j]):\n";

    Matrix m(ROW,COL,1.0);
    std::vector<std::vector<double>> vm(ROW,std::vector<double>(COL,1.0));
    double** rm = new double* [ROW];
    for (size_t i=0; i<ROW; ++i) rm[i] = new double [COL]{1.0};
	// for (size_t i=0; i<m.size(); ++i) m.ptr()[i] = 1.0;
    volatile double dummy_sum = 0.0;
    {
        Timer t("Flat Matrix<double> (RowSubMatrix proxy)");
        for (size_t c=0; c<repeat; ++c)
        {
            for (size_t i=0; i<ROW; ++i)
            {
                auto row = m[i];
                for (size_t j=0; j<COL; ++j)
                {
                    row[j] += 1.0001;
                }
            }
            dummy_sum += m[0][0];
        }
    }

    {
        Timer t("std::vector<std::vector<double>>");
        for (size_t c=0; c<repeat; ++c)
        {
            for (size_t i=0; i<ROW; ++i)
                for (size_t j=0; j<COL; ++j)
                    vm[i][j] += 1.0001;
            dummy_sum += vm[0][0];
        }
    }

    {
        Timer t("Raw double** Pointer Array");
        for (size_t c=0; c<repeat; ++c)
        {
            for (size_t i=0; i<ROW; ++i)
                for (size_t j=0; j<COL; ++j)
                    rm[i][j] += 1.0001;
            dummy_sum += rm[0][0];
        }
    }

    std::cout << "\n[3] Bulk Row Assignment (m[i]=v):\n";

    Vec<double> sourceVec(COL,3.14159565);
    std::vector<double> stdSourceVec(COL,3.14159565);

    {
        Timer t("Flat Matrix<double> (m[i] = std::vector, via std::copy)");
        for (size_t c=0; c<repeat; ++c) for (size_t i=0; i<ROW; ++i) m[i] = stdSourceVec;
    }

    {
        Timer t("Flat Matrix<double> (m[i] = Vec, via std::copy)");
        for (size_t c=0; c<repeat; ++c) for (size_t i=0; i<ROW; ++i) m[i] = sourceVec;
    }

    {
        Timer t("std::vector<std::vector<double>>");
        for (size_t c=0; c<repeat; ++c) for (size_t i=0; i<ROW; ++i) vm[i] = stdSourceVec;
    }

    {
        Timer t("Raw double** Pointer Array (m[i] = v, via std::copy)");
        for (size_t c=0; c<repeat; ++c) for (size_t i=0; i<ROW; ++i) std::copy(stdSourceVec.begin(),stdSourceVec.end(),rm[i]);
    }

    for (size_t i=0; i<ROW; ++i) delete [] rm[i];
    delete [] rm;
    return 0;
}
