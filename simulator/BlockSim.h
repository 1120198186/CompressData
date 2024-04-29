#include "indexing.h"
#include "qcircuit.h"
#include "block.h"

/**
 * @brief Conduct the block-based state vector simulation
 * 
 * @param qc 
 * @param memQubits 
 * 
 * @return the I/O time (sec)
 */
double BlockSim(QCircuit &qc, int memQubits);