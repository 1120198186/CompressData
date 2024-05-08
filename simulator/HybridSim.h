#include <thread>
#include "gen_gate.h" // high-order operation-matrix
#include "indexing.h"
#include "qcircuit.h"
#include "block.h"

/**
 * @brief Conduct the hybrid simulation
 * 
 * @param qc        the quantum circuit
 * @param memQubits the number of qubits storable in the memory
 */
void HybridSim(QCircuit &qc, int memQubits, long long calBlocks = 0);

/**
 * @brief Build high-order operation matrix
 * 
 * @param opMat     the high-order operation matrix
 * @param qc        the quantum circuit
 * @param H         the size of the opMat
 * @param lowQubits the number of low-order qubits
 */
void BuildHighOrderOpMat(Matrix &opMat, QCircuit &qc, long long H, int lowQubits);

/**
 * @brief Conduct the merge operation for one block
 * 
 * @param localV  the local state vector
 * @param opMat   the high-order operation matrix
 * @param mergeNo the No. of the merge operation
 * @param H       the size of the opMat
 * @param dir     the directory of the output files
 * 
 * @return the I/O time (us)
 */
double MergeComputing(Matrix &localV, Matrix &opMat, long long mergeNo, long long H, string dir);

/**
 * @brief Merge for one block (|0>^n)
 * 
 * @param localV 
 * @param opMat 
 * @param H 
 * @param dir 
 * 
 * @return double the I/O time (us)
 */
double MergeComputingForOneBlock(Matrix &localV, Matrix &opMat, long long H, string dir);