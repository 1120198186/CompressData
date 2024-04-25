//
// Created by Yuhong Song on 2022/5/29.
//

#include "global_notation.h"

// Mark [[1]]
double mark[1][1] = {{1}};
Matrix MARK1 = Matrix((double**)mark, 1, 1);/* NOLINT */

// Zeros
double zeros[2][1] = {{1},
                      {0}};
Matrix ZEROS = Matrix((double**)zeros, 2, 1);/* NOLINT */

// Ones
double ones[2][1] = {{0},
                     {1}};
Matrix ONES = Matrix((double**)ones, 2, 1);/* NOLINT */

// Plus |+>
double pluss[2][1] = {{1.0 / sqrt(2)},
                      {1.0 / sqrt(2)}};
Matrix PLUS = Matrix((double**)pluss, 2, 1);/* NOLINT */

// Plus |->
double minuss[2][1] = {{1.0 / sqrt(2)},
                       {-1.0 / sqrt(2)}};
Matrix MINUS = Matrix((double**)minuss, 2, 1); /* NOLINT */



// --------------Gate-----------------
// Hardamard Gate
double h[2][2] = {{1.0 / sqrt(2), 1.0 / sqrt(2)},
                  {1.0 / sqrt(2), -1.0 / sqrt(2)}};
Matrix H = Matrix((double**)h, 2, 2);/* NOLINT */

// Identity Gate
double ide[2][2] = {{1, 0},
                    {0, 1}};
Matrix IDE = Matrix((double**)ide, 2, 2);/* NOLINT */

// NOT Gate (X Gate)
double xx[2][2] = {{0, 1},
                   {1, 0}};
Matrix X = Matrix((double**)xx, 2, 2);/* NOLINT */

// Z Gate
double zz[2][2] = {{1, 0},
                   {0, -1}};
Matrix Z = Matrix((double**)zz, 2, 2);/* NOLINT */

// CH Gate
double ch[4][4] = {{1, 0, 0, 0},
                   {0, 1.0/ sqrt(2), 0, 1.0/ sqrt(2)},
                   {0, 0, 1, 0},
                   {0, 1.0/ sqrt(2), 0, -1.0/ sqrt(2)}};
Matrix CH = Matrix((double**)ch, 4, 4);/* NOLINT */

// Swap Gate
double swp[4][4] = {{1, 0, 0, 0},
                    {0, 0, 1, 0},
                    {0, 1, 0, 0},
                    {0, 0, 0, 1}};
Matrix SWAP = Matrix((double**)swp, 4, 4);/* NOLINT */

// CNOT Gate (CX Gate)
double cx[4][4] = {{1, 0, 0, 0},
                   {0, 1, 0, 0},
                   {0, 0, 0, 1},
                   {0, 0, 1, 0}};
Matrix CX = Matrix((double**)cx, 4, 4);/* NOLINT */

// 0-CNOT Gate (0-CX Gate)
double zero_cx[4][4] = {{0, 1, 0, 0},
                        {1, 0, 0, 0},
                        {0, 0, 1, 0},
                        {0, 0, 0, 1}};
Matrix ZCX = Matrix((double**)zero_cx, 4, 4);/* NOLINT */

// CZ Gate
double cz[4][4] = {{1, 0, 0, 0},
                   {0, 1, 0, 0},
                   {0, 0, 1, 0},
                   {0, 0, 0, -1}};
Matrix CZ = Matrix((double**)cz, 4, 4);/* NOLINT */

// DCX Gate
double dcx[4][4] = {{1, 0, 0, 0},
                    {0, 0, 0,1},
                    {0, 1, 0, 0},
                    {0, 0, 1, 0}};
Matrix DCX = Matrix((double**)dcx, 4, 4);/* NOLINT */

// Toffoli Gate (CCX Gate)
double toffoli[8][8] = {{1, 0, 0, 0, 0, 0, 0, 0},
                        {0, 1, 0, 0, 0, 0, 0, 0},
                        {0, 0, 1, 0, 0, 0, 0, 0},
                        {0, 0, 0, 1, 0, 0, 0, 0},
                        {0, 0, 0, 0, 1, 0, 0, 0},
                        {0, 0, 0, 0, 0, 1, 0, 0},
                        {0, 0, 0, 0, 0, 0, 0, 1},
                        {0, 0, 0, 0, 0, 0, 1, 0}};
Matrix TOFFOLI = Matrix((double**)toffoli, 8, 8);/* NOLINT */

// CSwap
double cswap[8][8] = {{1, 0, 0, 0, 0, 0, 0, 0},
                      {0, 1, 0, 0, 0, 0, 0, 0},
                      {0, 0, 1, 0, 0, 0, 0, 0},
                      {0, 0, 0, 1, 0, 0, 0, 0},
                      {0, 0, 0, 0, 1, 0, 0, 0},
                      {0, 0, 0, 0, 0, 0, 1, 0},
                      {0, 0, 0, 0, 0, 1, 0, 0},
                      {0, 0, 0, 0, 0, 0, 0, 1}};
Matrix CSWAP = Matrix((double**)cswap, 8, 8);/* NOLINT */

double theta1 = 0.4;
double ry[2][2] = {{cos(theta1 / 2), -sin(theta1 / 2)},
                   {sin(theta1 / 2), cos(theta1 / 2)}};
Matrix RY = Matrix((double **)ry, 2, 2);

map <string, Matrix*> Gate_dict = { // real gate
        {"IDE",     &IDE},
        {"H",       &H},
        {"X",       &X},
        {"Z",       &Z},
        {"CX",      &CX},
        {"ZCX",     &ZCX},
        {"SWAP",    &SWAP},
        {"TOFFOLI", &TOFFOLI},
        {"CH",      &CH},
        {"CZ",      &CZ},
        {"DCX",     &DCX},
        {"CSWAP",   &CSWAP},
        {"RY",      &RY},
        {"MERGE",   &IDE},  // For QuanTrans
        {"MSWAP",   &SWAP}, // For QuanTrans
        {"PHINV",   &IDE},
};
// a global gate dictionary


// rotation gate
void Rotation_Y(double theta, Matrix &RY) {
    double ry[2][2] = {{cos(theta/2), -sin(theta/2)},
                       {sin(theta/2), cos(theta/2)}};
    RY.assign_matrix((double **)ry, 2, 2);
}


/**
 * @brief Initialize a matrix to be IDE
 * 
 * @param row the number of rows and cols
 * @param R return value
 */
void Matrix_Init_IDE(long long row, Matrix& R) {
    R.allocate_matrix(row, row);
    for (long long i = 0; i < row; ++ i) {
        R.data[i][i] = 1;
    }
}


/**
 * @brief Initialize a all-zero matrix
 * 
 * @param row 
 * @param R 
 */
void Matrix_Init_ZERO(long long row, Matrix& R) {
    R.allocate_matrix(row, row);
}


/**
 * @brief TensorProduct using Matrix
 * 
 * @param A 
 * @param B 
 * @param C return A tensor-product B
 */
void Matrix_TensorProduct(Matrix& A, Matrix& B, Matrix& C) {
    // Matrix C = Matrix A x Matrix B
    // Implement the tensor product for 2D matrix A and B.
    // Assume A.shape = m * n, B.shape = p * q.
    // Then, the result of TensorProduct(A, B).shape = mp * nq
    // :return: a 2D-[mp, nq] matrix C
    C.allocate_matrix(A.row * B.row, A.col * B.col);
    for (long long ar = 0; ar < A.row; ar++){
        for (long long ac = 0; ac < A.col; ac++){
            if (A.data[ar][ac] == 0.0) continue;
            for (long long br = 0; br < B.row; br++){
                for (long long bc = 0; bc < B.col; bc++){
                    C.data[ar * B.row + br][ac * B.col + bc] = A.data[ar][ac] * B.data[br][bc];
                }
            }
        }
    }
}


/**
 * @brief Matrix addition A += B
 * 
 * @param A 
 * @param B 
 */
void Matrix_Add(Matrix& A, Matrix& B) {
    for (long long i = 0; i < A.row; ++ i) {
        for (long long j = 0; j < A.col; ++ j){
            A.data[i][j] += B.data[i][j];
        }
    }
}


/**
 * @brief Matrix multiplication using Matrix
 * 
 * @param A 
 * @param B 
 * @param C return C = A · B
 */
void Matrix_Multiplication(Matrix& A, Matrix& B, Matrix& C) {
    // Matrix C = Matrix A · Matrix B
    // Assume A.shape = m * n, B.shape = n * q, A.col = B.row
    // Then, the result of Multiplication(A, B).shape = m * q
    if (A.col != B.row) {
        cout << "[ERROR] Mat Mul: A.col != B.row. " << endl;
        exit(1);
    }
    C.allocate_matrix(A.row, B.col);
    for (long long i = 0; i < A.row; i++){ // matrix multiplication
        for (long long k = 0; k < A.col; k++){
            for (long long j = 0; j < B.col; j++){
                C.data[i][j] += A.data[i][k] * B.data[k][j];
            }
        }
    }
}

