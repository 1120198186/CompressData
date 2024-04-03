//
// Low-order Indexing
//

#include "matrix_wrap.h"

/**
 * @brief Indexing for 2x2 gates, including CU gates
 * 
 * @param localV    local state-vector
 * @param localVLen length of local state-vector, i.e., 2^l
 * @param gate      the processing gate
 * @param qid       the index of the qubit the gate is applied to
 * @param lowQubit  #low qubits, i.e., l
 * @param myRank    the rank of the current MPI process
 */
void IndexingWithGate2x2(Matrix &localV, long long localVLen,
        MatrixImp &gate, int qid, int lowQubit, int myRank);


/**
 * @brief Local indexing for 4x4 gates, <e.g.> SWAP
 * 
 * @param localV    local state-vector
 * @param localVLen length of local state-vector, i.e., 2^l
 * @param gate      the processing gate
 * @param lowQubit  #low qubits, i.e., l
 * @param myRank    the rank of the current MPI process
 */
void IndexingWithGate4x4(Matrix &localV, long long localVLen,
        MatrixImp &gate, int lowQubit);


/**
 * @brief Processing a level of gates on low-order qubits using indexing algorithm
 * 
 * @param localV    local state-vector
 * @param localVLen length of local state-vector, i.e., 2^l
 * @param GateSeq   a level of gates
 * @param lowQubit  #low qubits, i.e., l
 * @param myRank    the rank of the current MPI process
 */
void LocalComputing(Matrix &localV, long long localVLen,
        vector<MatrixImp> &GateSeq, int lowQubit, int myRank);