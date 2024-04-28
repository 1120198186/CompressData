#pragma once

#include <cmath>

#include "block.h"
#include "qcircuit.h"
#include "indexing.h"

vector<int> DicCounterEncoder(
    Matrix &m,
    vector<double> &amp,
    vector<pair<int,int> > &ampCnt,
    vector<vector<int> > &seq,
    double &compRate);

/**
 * @brief 
 * 
 * @param n 
 * @param v 
 * @param amp 
 * @param ampCnt 
 * @param seq 
 * @return Matrix 
 */
Matrix DicCounterDecoder(
    int n,
    vector<int> &v,
    vector<double> &amp,
    vector<pair<int,int> > &ampCnt,
    vector<vector<int> > &seq);

/**
 * @brief Call the dictionary counter compressor
 * 
 * @param qc a quantum circuit
 */
void DicCounter(QCircuit &qc, int memQubits);

// /**
//  * @brief 
//  * 
//  * @param [in] state vector 
//  * @param [out] pair id -> {p,cnt}
//  * @param [out] lzw id -> pair id sequence
//  */
// vector<int> DicCounterEncoder(Matrix &m,vector<pair<double,int> > &pCnt,vector<vector<int> > &seq);

// Matrix DicCounterDecoder(int n,vector<int> &v,vector<pair<double,int> > &pCnt,vector<vector<int> > &seq);