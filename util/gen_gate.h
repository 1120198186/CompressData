//
// High-order operation-matrix constructed by tensor product
//

#include "matrix_wrap.h"

/**
 * @brief Generate the gate matrix of a control gate
 * 
 * @param gate      the gate matrix without the control qubit
 * @param ctrl      the control qubit index, from 0 to highQubit-1
 * @param targ      the target qubit index, from 0 to highQubit-1
 * @param highQubit #high qubits
 * @param ctl_gate  return 2^h x 2^h operation-matrix
*/
void GenControlGate(Matrix &gate, int ctrl, int targ, int highQubit, Matrix &ctl_gate);


/**
 * @brief Generate the gate matrix of a swap gate
 * 
 * @param ctrl      the control qubit index, from 0 to highQubit-1
 * @param targ      the target qubit index, from 0 to highQubit-1
 * @param highQubit #high qubits
 * @param swap      return 2^h x 2^h operation-matrix
 */
void GenSwapGate(int ctrl, int targ, int highQubit, Matrix &swap);