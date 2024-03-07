#pragma once

#include <iostream>
#include <fstream>
#include <cmath>

#include "global_notation.h"

using namespace std;

class Gate {
public:
    string gateId_;
    int controlBit_, targetBit_; // starts from 0
    vector<int> extraControlBits_; // used for the number of contorl bits is greater than 1
    Matrix m_;

    Gate(string gateId, int targetBit) : 
        gateId_(gateId), controlBit_(targetBit), targetBit_(targetBit), m_(*Gate_dict[gateId_]) {}

    Gate(string gateId, int controlBit, int targetBit) : 
        gateId_(gateId), controlBit_(controlBit), targetBit_(targetBit), m_(*Gate_dict[gateId_]) {}

    Gate(string gateId, int controlBit, int targetBit, const Matrix& m) : 
        gateId_(gateId), controlBit_(controlBit), targetBit_(targetBit) {
        m_.copy(m);
    }

    Gate(const Gate& other) {
        gateId_ = other.gateId_;
        controlBit_ = other.controlBit_;
        targetBit_ = other.targetBit_;
        m_.copy(other.m_);
    }

    Gate& operator=(const Gate& other) {
        gateId_ = other.gateId_;
        controlBit_ = other.controlBit_;
        targetBit_ = other.targetBit_;
        m_.copy(other.m_);
        return *this;
    }
};

class MatrixImp {
public:
    string gate_name_;
    Matrix gate_;
    bool is_control_gate_ = false; // is control gate?
    bool is_target_gate_ = false; // is target bit?
    int control_qubit_ = 0;
    int target_qubit_ = 0;

    MatrixImp() {};

    MatrixImp(string gate) : gate_name_(gate), gate_(*Gate_dict[gate]) {
        // gate_.copy(*Gate_dict[gate]); 
    }

    // "X", true, false, j, j + 1
    MatrixImp(string gate, bool is_control_gate, bool is_target_gate, 
            int control_qubit, int controlled_qubit) : 
            gate_name_(gate), 
            gate_(*Gate_dict[gate]), 
            is_control_gate_(is_control_gate), 
            is_target_gate_(is_target_gate), 
            control_qubit_(control_qubit), 
            target_qubit_(controlled_qubit) {
        // gate_.copy(*Gate_dict[gate]);
    }

    MatrixImp(const MatrixImp& other) {
        gate_name_ = other.gate_name_;
        gate_ = other.gate_;
        is_control_gate_ = other.is_control_gate_;
        is_target_gate_ = other.is_target_gate_;
        control_qubit_ = other.control_qubit_;
        target_qubit_ = other.target_qubit_;
    }
    
    MatrixImp(string gate, bool is_control_gate, bool is_target_gate, 
            int control_qubit, int controlled_qubit, Matrix other) : 
            gate_name_(gate), 
            gate_(other),
            is_control_gate_(is_control_gate), 
            is_target_gate_(is_target_gate), 
            control_qubit_(control_qubit), 
            target_qubit_(controlled_qubit) {
        // gate_.copy(*Gate_dict[gate]);
    }

    MatrixImp& operator=(const MatrixImp& other) {
        gate_.copy(other.gate_);
        is_control_gate_ = other.is_control_gate_;
        control_qubit_ = other.control_qubit_;
        is_target_gate_ = other.is_target_gate_; 
        target_qubit_ = other.target_qubit_;
        this->gate_name_ = other.gate_name_;
        return *this;
    }

    void copy(MatrixImp matrix) {
        gate_.copy(matrix.gate_);
        is_control_gate_ = matrix.is_control_gate_;
        control_qubit_ = matrix.control_qubit_;
        is_target_gate_ = matrix.is_target_gate_; 
        target_qubit_ = matrix.target_qubit_;
        this->gate_name_ = matrix.gate_name_;
    }
};


/**
 * @brief Used for qHiPSTER, SingleQE, where gate structures are record in <Gate>
 * 
 * @param gates save the generated circuit structure
 * @param fname filename
 */
void GenerateGatesFromFile(vector<vector<Gate>> &gates, string fname);


/**
 * @brief Used for MultiQE, QuanPath and QuanTrans, where gate structures are record in <MatrixImp>
 * 
 * @param gates save the generated circuit structure
 * @param fname filename
 */
void GenerateMatrixImpsFromFile(vector<vector<MatrixImp>> &gates, string fname);


// void GenerateQFTGatesAllSteps(vector<vector<MatrixImp>> &gates, int numQubits) {
//     for (int stage = 0; stage < numQubits; stage++) {
//         int n = numQubits - stage;
//         vector<MatrixImp> tmp_matrix;
//         for (int i = 0; i < n; i++) {
//             tmp_matrix.clear();
//             for (int j = numQubits-1; j > n-1; j--) {
//                 tmp_matrix.push_back(MatrixImp("IDE"));
//             }
//             if (i == 0) {
//                 tmp_matrix.push_back(MatrixImp("H"));
//             } else {
//                 double** tmp = new double*[2];
//                 tmp[0] = new double[2];
//                 tmp[1] = new double[2];
//                 tmp[0][0] = 1;
//                 tmp[0][1] = 0;
//                 tmp[1][0] = 0;
//                 tmp[1][1] = cos(2 * M_PI / pow(2, i + 1));
//                 tmp_matrix.push_back(MatrixImp("UROT", true, false, n-i-1, n-1, Matrix(tmp, 2, 2)));
//                 // tmp_matrix.push_back(MatrixImp("UROT", true, true, n-i-1, n-1, Matrix(tmp, 2, 2)));
//                 delete [] tmp[0];
//                 delete [] tmp[1];
//                 delete [] tmp;
//             }
//             for (int j = n-i-2; j >= 0; j--) {
//                 tmp_matrix.push_back(MatrixImp("IDE"));
//             }
//             gates.push_back(tmp_matrix);
//         }
//     }
// }

// void GenerateGroverGatesAllSteps(vector<vector<MatrixImp>> &gates, int numQubits) {
//     vector<MatrixImp> hs0;
//     for (int i = 0; i < numQubits; i++) {
//         hs0.emplace_back("H");
//     }
//     gates.push_back(std::move(hs0));
//     hs0.clear();
//     const long long numIteration = (M_PI / 4.0) * (sqrt(pow(2, numQubits)));
//     for (long long iter = 0; iter < numIteration; iter++) {
//         hs0.emplace_back("Z");
//         for (int i = 1; i < numQubits; i++) {
//             hs0.emplace_back("IDE");
//         }
//         gates.push_back(std::move(hs0));
//         hs0.clear();

//         for (int i = 0; i < numQubits; i++) {
//             hs0.emplace_back("H");
//         }
//         gates.push_back(std::move(hs0));
//         hs0.clear();

//         for (int i = 0; i < numQubits; i++) {
//             hs0.emplace_back("X");
//         }
//         gates.push_back(std::move(hs0));
//         hs0.clear();

//         hs0.emplace_back("Z");
//         for (int i = 1; i < numQubits; i++) {
//             hs0.emplace_back("IDE");
//         }
//         gates.push_back(std::move(hs0));
//         hs0.clear();

//         for (int i = 0; i < numQubits; i++) {
//             hs0.emplace_back("X");
//         }
//         gates.push_back(std::move(hs0));
//         hs0.clear();

//         double** ryTemp = new double*[2];
//         ryTemp[0] = new double[2];
//         ryTemp[1] = new double[2];
//         ryTemp[0][0] = -1;
//         ryTemp[0][1] = 0;
//         ryTemp[1][0] = 0;
//         ryTemp[1][1] = -1;
//         for (int i = 0; i < numQubits - 1; i++) {
//             hs0.emplace_back("IDE");
//         }
//         hs0.emplace_back("RY", false, false, 0, 0, Matrix(ryTemp, 2, 2));
//         gates.push_back(std::move(hs0));
//         hs0.clear();
//         delete [] ryTemp[0];
//         delete [] ryTemp[1];
//         delete [] ryTemp;

//         for (int i = 0; i < numQubits; i++) {
//             hs0.emplace_back("H");
//         }
//         gates.push_back(std::move(hs0));
//     }
// }

// void GenerateVQCGatesAllSteps(vector<vector<MatrixImp>> &gates, int numQubits) {
//     double** ryTemp = new double*[2];
//     ryTemp[0] = new double[2];
//     ryTemp[1] = new double[2];
//     ryTemp[0][0] = cos(0.2);
//     ryTemp[0][1] = -1 * sin(0.2);
//     ryTemp[1][0] = sin(0.2);
//     ryTemp[1][1] = cos(0.2);

//     // double** cxTemp = new double*[2];
//     // cxTemp[0] = new double[2];
//     // cxTemp[1] = new double[2];
//     // cxTemp[0][0] = 0;
//     // cxTemp[0][1] = 1;
//     // cxTemp[1][0] = 1;
//     // cxTemp[1][1] = 0;

//     for (int stage = 0; stage < 20; stage++) {
//         // geneate RY gates
//         vector<MatrixImp> ry1;
//         for (int i = 0; i < numQubits; i++) ry1.push_back(MatrixImp("RY", false, false, 0, 0, Matrix(ryTemp, 2, 2)));
//         gates.push_back(ry1);

//         ry1.clear();
//         // geneate RY gates
//         for (int i = 0; i < numQubits; i++) ry1.push_back(MatrixImp("RY", false, false, 0, 0, Matrix(ryTemp, 2, 2)));
//         gates.push_back(ry1);

//         // generate all CX gates
//         for (int i = 0; i < numQubits - 1; i++) {
//             ry1.clear();
//             int j;
//             for (j = 0; j < i; j++) {
//                 ry1.emplace_back("IDE");
//             }
//             ry1.push_back(MatrixImp("X", true, false, j, j + 1));
//             ry1.push_back(MatrixImp("X", true, true, j, j + 1));
//             for (j = j + 2; j < numQubits; j++) {
//                 ry1.emplace_back("IDE");
//             }
//             gates.push_back(ry1);
//         }


//     }
//     delete [] ryTemp[0];
//     delete [] ryTemp[1];
//     delete [] ryTemp;

//     // int expectedSize = 20 * (2 + numQubits - 1);
//     // if (gatesForAllSteps.size() != expectedSize) {
//     //     cout << "[ERROR] generateGatesOfAllStepsForVqc, the size of generated gates was wrong: " << gatesForAllSteps.size() << ", which should be " << expectedSize << endl;
//     //     exit(1);
//     // }
// }
