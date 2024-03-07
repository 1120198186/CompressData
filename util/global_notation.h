//
// Created by Yuhong Song on 2022/5/29.
//

#define FREE
#include <bits/stdc++.h>
using namespace std;

#ifndef QUANTUM_DD_GATE_NOTATION_H
#define QUANTUM_DD_GATE_NOTATION_H

// ----------Matrix structure
class Matrix {
public:
    double **data; // data in matrix
    long long row, col; // row, column and number of Non-zeros

    // Constructor
    Matrix() {
        data = nullptr;
        row = 0;
        col = 0;
    } // default

    // initialize a all-zero matrix
    Matrix(long long r, long long c) { 
        row = r;
        col = c;
        data = new double *[row];
        for (long long i = 0; i < row; i++) {
            data[i] = new double[col];
            memset(data[i], 0.0, col * sizeof(double));
        }
    }

    // construct a matrix using value temp
    Matrix(double **temp, long long r, long long c) {
        row = r;
        col = c;
        data = new double *[row];
        for (long long i = 0; i < row; i++) {
            data[i] = new double[col];
            for (long long j = 0; j < col; j++) {
                data[i][j] = *((double *) temp + i * col + j);
            }
        }
    }

    Matrix(const Matrix &matrx) {
        row = matrx.row;
        col = matrx.col;
        data = new double*[row];
        for (long long i = 0; i < row; i++){
            data[i] = new double[col];
            memcpy(data[i], matrx.data[i], col * sizeof(double));
        }
    }

    // print a Matrix
    void print() const {
        cout << "-----Matrix: [" << row << "] * [" << col << "]-----" << endl;
        cout.setf(std::ios::left);
        if (data == nullptr) {
            cout << "Matrix is Empty!" << endl;
        } else {
            for (long long i = 0; i < row; i++) {
                for (long long j = 0; j < col; j++) {
                    cout.width(8);
                    cout << std::setprecision(4) << data[i][j];
                }
                cout << endl;
            }
        }
    }

    // Allocate Matrix memory
    void allocate_matrix(long long r, long long c){
        if (data != nullptr) {
            // cout << "allocate_matrix not null" << endl;
            for (long long i = 0; i < row; i++){
                delete[] data[i];
            }
            delete[] data;
        }
        data = nullptr;
        row = r; col = c;
        data = new double*[row];
        for (long long i = 0; i < row; i++){
            data[i] = new double[col];
            memset(data[i], 0.0, col * sizeof(double));
        }
    }

    // copy Matrix from M
    void copy(const Matrix& M){
        if (data) {
            // cout << "copy matrix not null" << endl;
            for (long long i = 0; i < row; i++){
                delete[] data[i];
            }
            delete[] data;
        }
        data = nullptr;
        row = M.row; col = M.col;
        data = new double*[row];
        for (long long i = 0; i < row; i++){
            data[i] = new double[col];
            memcpy(data[i], M.data[i], col * sizeof(double));
        }
    }

    // scalar multiplication for Matrix, this function will change the original Matrix
    void _scalar_multiplication(double scalar) const {
        for (long long i = 0; i < row; i++){
            for (long long j = 0; j < col; j++){
                data[i][j] = scalar * data[i][j];
            }
        }
    }

    // allocate and assign data
    void assign_matrix(double** temp, long long r, long long c) {
        if (data != nullptr) {
            // cout << "assign matrix not null" << endl;
            for (long long i = 0; i < row; i++){
                delete[] data[i];
            }
            delete[] data;
        }
        data = nullptr;
        row = r; col = c;
        data = new double*[row];
        for (long long i = 0; i < row; i++){
            data[i] = new double[col];
            for (long long j = 0; j < col; j++){
                data[i][j] = *((double*) temp + i * col + j);
            }
        }
    }

    void free() {
        if (data) {
            // cout << "copy matrix not null" << endl;
            for (long long i = 0; i < row; i++){
                delete[] data[i];
            }
            delete[] data;
        }
        data = nullptr;
    }

// #ifdef FREE
    // Destructor
    // ~Matrix(){
    //     if (data) {
    //         for (long long i = 0; i < row; i++){
    //             delete[] data[i];
    //         }
    //         delete[] data;
    //     }
    //     data = nullptr;
    //     cout << "~Matrix Class Destruct Finished!" << endl;
    // }
// #endif
};

// ------------notation--------------
// Mark [[1]]
extern Matrix MARK1;

// Zeros
extern Matrix ZEROS;

// Ones
extern Matrix ONES;

// Plus |+>
extern Matrix PLUS;

// Plus |->
extern Matrix MINUS;


// --------------Gate-----------------
// Hardamard Gate (0%)
extern Matrix H;

// Identity Gate (50%)
extern Matrix IDE;

// X Gate (NOT Gate, 50%)
extern Matrix X;

// Z Gate (50%)
extern Matrix Z;

// CH Gate (62.5%)
extern Matrix CH;

// Swap gate (75%)
extern Matrix SWAP;

// CNOT Gate (CX Gate, 75%)
extern Matrix CX;

// 0-CNOT Gate (0-CX Gate, 75%)
extern Matrix ZCX;

// CZ Gate (75%)
extern Matrix CZ;

//DCX Gate (75%)
extern Matrix DCX;

// Toffoli Gate (CCX Gate, 87.5%)
extern Matrix TOFFOLI;

// CSwap Gate (87.5%)
extern Matrix CSWAP;

extern Matrix RY;

// For QuanTrans //////////////
extern Matrix MERGE;

extern Matrix MSWAP;

///////////////////////////////

extern map <string, Matrix*> Gate_dict; // a global gate dictionary

void Rotation_Y(double theta, Matrix &RY); // the rotation Y gate

//////////////////////////////

/**
 * @brief Initialize a matrix to be IDE
 * 
 * @param row the number of rows and cols
 * @param R return value
 */
void Matrix_Init_IDE(long long row, Matrix& R);


/**
 * @brief Initialize a all-zero matrix
 * 
 * @param row 
 * @param R 
 */
void Matrix_Init_ZERO(long long row, Matrix& R);


/**
 * @brief TensorProduct using Matrix
 * 
 * @param A 
 * @param B 
 * @param C return A tensor-product B
 */
void Matrix_TensorProduct(Matrix& A, Matrix& B, Matrix& C);


/**
 * @brief Matrix addition A += B
 * 
 * @param A 
 * @param B 
 */
void Matrix_Add(Matrix& A, Matrix& B);


/**
 * @brief Matrix multiplication using Matrix
 * 
 * @param A 
 * @param B 
 * @param C return C = A · B
 */
void Matrix_Multiplication(Matrix& A, Matrix& B, Matrix& C);

#endif //QUANTUM_DD_GATE_NOTATION_H
