#include<iostream>
#include "LcVec.hpp"
#include <random>
#include <ctime>
#include <immintrin.h>
constexpr Lc::Matrix<float, 4, 4> operator*(const Lc::Matrix<float, 4, 4>& a, const Lc::Matrix<float, 4, 4>& b) noexcept
{
    Lc::Matrix<float, 4, 4> result;
    const float* pA = &a.data(0);
    const float* pB = &b.data(0);
    float* pResult = &result.data(0);
    for (int i = 0; i < 4; ++i) {
        const float* a_row_ptr = &pA[i * 4];
        __m128 b_row0 = _mm_load_ps(&pB[0 * 4]);
        __m128 b_row1 = _mm_load_ps(&pB[1 * 4]);
        __m128 b_row2 = _mm_load_ps(&pB[2 * 4]);
        __m128 b_row3 = _mm_load_ps(&pB[3 * 4]);
        __m128 result_row = _mm_mul_ps(_mm_set1_ps(a_row_ptr[0]), b_row0);
        result_row = _mm_add_ps(result_row, _mm_mul_ps(_mm_set1_ps(a_row_ptr[1]), b_row1));
        result_row = _mm_add_ps(result_row, _mm_mul_ps(_mm_set1_ps(a_row_ptr[2]), b_row2));
        result_row = _mm_add_ps(result_row, _mm_mul_ps(_mm_set1_ps(a_row_ptr[3]), b_row3));
        _mm_store_ps(&pResult[i * 4], result_row);
    }
    return result;
}
constexpr Lc::Matrix<double, 4, 4> operator*(const Lc::Matrix<double, 4, 4>& a, const Lc::Matrix<double, 4, 4>& b) noexcept
{
    Lc::Matrix<double, 4, 4> result;
    const double* pA = &a.data(0);
    const double* pB = &b.data(0);
    double* pResult = &result.data(0);
    for (int i = 0; i < 4; ++i) {
        const double* a_row = &pA[i * 4];
        __m128d b0_01 = _mm_load_pd(&pB[0 * 4]);
        __m128d b0_23 = _mm_load_pd(&pB[0 * 4 + 2]);
        __m128d b1_01 = _mm_load_pd(&pB[1 * 4]);
        __m128d b1_23 = _mm_load_pd(&pB[1 * 4 + 2]);
        __m128d b2_01 = _mm_load_pd(&pB[2 * 4]);
        __m128d b2_23 = _mm_load_pd(&pB[2 * 4 + 2]);
        __m128d b3_01 = _mm_load_pd(&pB[3 * 4]);
        __m128d b3_23 = _mm_load_pd(&pB[3 * 4 + 2]);
        __m128d sum01 = _mm_add_pd(
                            _mm_add_pd(
                                _mm_mul_pd(_mm_set1_pd(a_row[0]), b0_01),
                                _mm_mul_pd(_mm_set1_pd(a_row[1]), b1_01)),
                            _mm_add_pd(
                                _mm_mul_pd(_mm_set1_pd(a_row[2]), b2_01),
                                _mm_mul_pd(_mm_set1_pd(a_row[3]), b3_01)));
        __m128d sum23 = _mm_add_pd(
                            _mm_add_pd(
                                _mm_mul_pd(_mm_set1_pd(a_row[0]), b0_23),
                                _mm_mul_pd(_mm_set1_pd(a_row[1]), b1_23)),
                            _mm_add_pd(
                                _mm_mul_pd(_mm_set1_pd(a_row[2]), b2_23),
                                _mm_mul_pd(_mm_set1_pd(a_row[3]), b3_23)));
        _mm_store_pd(&pResult[i * 4], sum01);
        _mm_store_pd(&pResult[i * 4 + 2], sum23);
    }
    return result;
}
// print matrix
template<typename T, size_t R, size_t C>
void printMatrix(const Lc::Matrix<T, R, C>& mat) {
    for (size_t i = 0; i < R; ++i) {
        for (size_t j = 0; j < C; ++j) {
            std::cout << mat[i][j] << ',';
        }
        std::cout << '\n';
    }
    std::cout << '\n';
}
template<size_t Rows, size_t Cols>
void randomInitialize(Lc::MatrixInt<Rows, Cols>& mat, int minVal, int maxVal) {
    for (size_t i = 0; i < Rows; ++i) {
        for (size_t j = 0; j < Cols; ++j) {
            mat[i][j] =rand()%100+1;
        }
    }
}
template<size_t Rows, size_t Cols>
void randomInitialize(Lc::MatrixFloat<Rows, Cols>& mat, int minVal, int maxVal) {
    for (size_t i = 0; i < Rows; ++i) {
        for (size_t j = 0; j < Cols; ++j) {
            mat[i][j] =rand()%100+1;
        }
    }
}
template<size_t Rows, size_t Cols>
void randomInitialize(Lc::MatrixDouble<Rows, Cols>& mat, int minVal, int maxVal) {
    for (size_t i = 0; i < Rows; ++i) {
        for (size_t j = 0; j < Cols; ++j) {
            mat[i][j] =rand()%100+1;
        }
    }
}
int main() {
    
    Lc::Matrix<float, 4, 4> m1,m2;
    randomInitialize(m1,1,50);
    randomInitialize(m2,1,50);
    printMatrix(m1);
    printMatrix(m2);
    printMatrix(m1*m2);


// Vec base op
/*
    Lc::VecDouble<4> vc,vc2;
    Lc::MatrixDouble<4,4> md;


    randomInitialize(vc,1,50);
    randomInitialize(md,1,50);
    printMatrix(vc);
    printMatrix(md);
    printMatrix(vc * md);
*/


// Matrix Compound
/*
    {
        Lc::MatrixDouble<2,2> result,result2;
        randomInitialize(result, 1, 50);
        result2=result;
        result==result2;
        printMatrix(result);
        printMatrix(result2+result);
        result-=1.0;
        printMatrix(result);
        result+=2.0;
        printMatrix(result);
        printMatrix(result++);
        printMatrix(++result);
        result*=10000.0;
        printMatrix(result);
        result/=10000.0;
        printMatrix(result);
    }
*/

// Matrix And Matrix Compare
/*
    Lc::MatrixDouble<2,2> m22,result;
    randomInitialize(m22, 1, 50);
    randomInitialize(result, 1, 50);
    printMatrix(m22);
    printMatrix(result);
    if(m22!=result)
        printf("!=\n");
    else
        printf("==\n");
*/
// Matrix And Value +/-* AND Value And Matrix +/-*
/*
    {
        Lc::MatrixDouble<2,2> m42,result;
        randomInitialize(m42, 1, 50);
        printMatrix(m42);
        Lc::MatrixFundamentalSp<double,2,2>::scalar_add_matrix(3,m42,result);
        printMatrix(result);
        Lc::MatrixFundamentalSp<double,2,2>::scalar_mul_matrix(2,m42,result);
        printMatrix(result);
        printMatrix(1000.0*m42);
        printMatrix(1000.0+m42);
        printMatrix(1000.0-m42);
        printMatrix(1000.0/m42);
        printMatrix(m42*1000.0);
        printMatrix(m42+1000.0);
        printMatrix(m42-1000.0);
        printMatrix(m42/1000.0);

    }
*/
// A * B Matrix Matrix
/*
    {
        Lc::MatrixInt<2,4> m24;
        Lc::MatrixInt<4,2> m42;
        Lc::MatrixInt<4,5> m45;
        Lc::MatrixInt<2,2>result;
        
        randomInitialize(m24, 1, 50);
        randomInitialize(m42, -10, 10);
        randomInitialize(m45, 1, 10);

        std::cout << "Result of A multiply B:\n";
        Lc::MatrixMultiplySp<int,2,4,2>::multiply(m24,m42,result);
        printMatrix(result);
        std::cout << "Result of A * B :\n";
        printMatrix(m24*m42);
    }
*/
// A + B Matrix Matrix
/*
    {
        Lc::MatrixInt<2,3> m24_0,m24_1,result;
        randomInitialize(m24_0, 1, 50);
        randomInitialize(m24_1, -10, 10);
        Lc::MatrixAddSp<int,2,3>::add(m24_0,m24_1,result);

        printMatrix(m24_0);
        printMatrix(m24_1);

        printMatrix(result);
        printMatrix(m24_0+m24_1);
    }
*/
// A / B Matrix Matrix
/*
    {
        Lc::MatrixDouble<3,3>m24;
        Lc::MatrixDouble<3,3>m44;


        randomInitialize(m24, 1, 50);
        randomInitialize(m44, -10, 10);

        printMatrix(m24);
        printf("\n");
        printMatrix(m44);
        printf("\n");
        printMatrix(m24/m44);
        
    }
*/
// A - B Matrix Matrix
/*
    {
        Lc::MatrixInt<2,3> m24_0,m24_1,result;
        randomInitialize(m24_0, 1, 50);
        randomInitialize(m24_1, -10, 10);
        Lc::MatrixSubtractSp<int,2,3>::subtract(m24_0,m24_1,result);

        printMatrix(m24_0);
        printMatrix(m24_1);

        printMatrix(result);
        printMatrix(m24_0-m24_1);
    }
*/
// A * B Vec Matrix
/*
    {
        Lc::VecInt<4> vc, result;
        Lc::MatrixInt<4,4> mc;
        randomInitialize(vc,1,50);
        randomInitialize(mc,1,50);
        printMatrix(vc);
        printMatrix(mc);
        Lc::MatrixMultiplySp<int,1,4,4>::multiply(vc,mc,result);
        printMatrix(result);
        Lc::VecMatrixMultiplySp<int,4,4>::multiply(vc,mc,result);
        printMatrix(result);
    }
*/
// A + B Vec Vec
/*
    {
        Lc::Vec<int,4> vc1, vc2,result;
        randomInitialize(vc1,1,50);
        randomInitialize(vc2,1,50);
        printMatrix(vc1);
        printMatrix(vc2);
        Lc::VecMatrixAddSp<int,4>::add(vc1,vc2,result);
        printMatrix(result);
        printMatrix(vc1+vc2);
    }
*/
// A - B Vec Vec
/*
    {
        Lc::Vec<int,4> vc1, vc2,result;
        randomInitialize(vc1,1,50);
        randomInitialize(vc2,1,50);
        printMatrix(vc1);
        printMatrix(vc2);
        Lc::VecMatrixSubtractSp<int,4>::subtract(vc1,vc2,result);
        printMatrix(result);
        _scalar_add_matrix(100,vc1,result);
        printMatrix(result);
        printMatrix(vc1-vc2);
        vc1++;
        --vc1;
        printMatrix(vc1);
        vc1+vc2;
    }
*/

}
