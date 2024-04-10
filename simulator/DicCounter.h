#pragma once

#include "global_notation.h"

vector<int> DicCounterEncoder(
    Matrix &m,
    vector<double> &sortedAmp,
    vector<pair<double,int> > &ampCnt,
    vector<vector<int> > &seq,
    double &compRate);

Matrix DicCounterDecoder(
    int n,
    vector<int> &v,
    vector<double> &sortedAmp,
    vector<pair<double,int> > &ampCnt,
    vector<vector<int> > &seq);

// /**
//  * @brief 
//  * 
//  * @param [in] state vector 
//  * @param [out] pair id -> {p,cnt}
//  * @param [out] lzw id -> pair id sequence
//  */
// vector<int> DicCounterEncoder(Matrix &m,vector<pair<double,int> > &pCnt,vector<vector<int> > &seq);

// Matrix DicCounterDecoder(int n,vector<int> &v,vector<pair<double,int> > &pCnt,vector<vector<int> > &seq);