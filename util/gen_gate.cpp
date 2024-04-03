//
// High-order operation-matrix constructed by tensor product
//

#include "gen_gate.h"

/**
 * @brief Generate the gate matrix of a control gate
 * 
 * @param gate      the gate matrix without the control qubit
 * @param ctrl      the control qubit index, from 0 to highQubit-1
 * @param targ      the target qubit index, from 0 to highQubit-1
 * @param highQubit #high qubits
 * @param ctl_gate  return 2^h x 2^h operation-matrix
*/
void GenControlGate(Matrix &gate, int ctrl, int targ, int highQubit, Matrix &ctl_gate) {
    Matrix highI, midCtrl, lowI;
    Matrix basisMat, temp;

    long long mask = 0; // the mask of the control qubit

    if (ctrl > targ) {  // a higher qubit controls a lower qubit
        // part 1: high-order I gates
        Matrix_Init_IDE((1 << (highQubit-1-ctrl)), highI);

        // part 2: the CU gate in the middle
        Matrix_Init_ZERO((1 << (ctrl-targ+1)), midCtrl);
        mask |= (1 << (ctrl-targ-1)); // mask the control qubit from qubit[targ+1] to qubit[ctrl]

        // |ctrl><ctrl| \otimes U + | >< | \otimes I
        for (auto i = 0; i < (1 << (ctrl-targ)); ++ i) { // all bases
            // basis |i> has (ctrl-targ) qubits and length (1 << (ctrl-targ))
            // calculate | i >< i |
            Matrix_Init_ZERO((1 << (ctrl-targ)), basisMat);
            basisMat.data[i][i] = 1;  // basisMat = | i >< i |

            if ((i & mask) == mask) { // control qubit = 1
                // midCtrl += | i >< i | \otimes gate
                Matrix_TensorProduct(basisMat, gate, temp); // temp = basisMat \otimes gate
                Matrix_Add(midCtrl, temp);  // midCtrl += temp
            } else {
                // midCtrl += | i >< i | \otimes I
                Matrix_TensorProduct(basisMat, IDE, temp); // temp = basisMat \otimes I
                Matrix_Add(midCtrl, temp);  // midCtrl += temp
            }
        }

        // part 3: low-order I gates
        Matrix_Init_IDE((1 << targ), lowI);
    } else {            // a lower qubit controls a higher qubit
        // part 1: high-order I gates
        Matrix_Init_IDE((1 << (highQubit-1-targ)), highI);

        // part 2: the CU gate in the middle
        Matrix_Init_ZERO((1 << (targ-ctrl+1)), midCtrl);
        mask |= (1 << (targ-ctrl-1)); // mask the control qubit from qubit[targ+1] to qubit[ctrl]

        // U \otimes |ctrl><ctrl| + I \otimes | >< |
        for (auto i = 0; i < (1 << (targ-ctrl)); ++ i) { // all bases
            // basis |i> has (targ-ctrl) qubits and length (1 << (targ-ctrl))
            // calculate | i >< i |
            Matrix_Init_ZERO((1 << (targ-ctrl)), basisMat);
            basisMat.data[i][i] = 1;  // basisMat = | i >< i |

            if ((i & mask) == mask) { // control qubit = 1
                // midCtrl += gate \otimes | i >< i |
                Matrix_TensorProduct(gate, basisMat, temp);
                Matrix_Add(midCtrl, temp);
            } else {
                // midCtrl += I \otimes | i >< i |
                Matrix_TensorProduct(IDE, basisMat, temp); // temp = basisMat \otimes I
                Matrix_Add(midCtrl, temp);  // midCtrl += temp
            }
        }

        // part 3: low-order I gates
        Matrix_Init_IDE((1 << ctrl), lowI);
    }
    // tensor product of 3 parts: high x mid x ctrl
    Matrix_TensorProduct(highI, midCtrl, ctl_gate);
    midCtrl.copy(ctl_gate);
    Matrix_TensorProduct(midCtrl, lowI, ctl_gate);
}


/**
 * @brief [Util] Swap two rows of a gate matrix
 * 
 * @param r1   row index 1
 * @param r2   row index 2
 * @param gate return value
 */
void swapRow(long long r1, long long r2, Matrix &gate) {
    double tmp[gate.row];
    memcpy(tmp, gate.data[r1], gate.row * sizeof(double));
    memcpy(gate.data[r1], gate.data[r2], gate.row * sizeof(double));
    memcpy(gate.data[r2], tmp, gate.row * sizeof(double));
}


/**
 * @brief Generate the gate matrix of a swap gate
 * 
 * @param ctrl      the control qubit index, from 0 to highQubit-1
 * @param targ      the target qubit index, from 0 to highQubit-1
 * @param highQubit #high qubits
 * @param swap      return 2^h x 2^h operation-matrix
 */
void GenSwapGate(int ctrl, int targ, int highQubit, Matrix &swap) {
    Matrix_Init_IDE((1 << highQubit), swap);

    long long ctrlmask = (1 << ctrl); // ctrl qubit = 1, else 0
    long long targmask = (1 << targ); // targ qubit = 1, else 0
    long long row;

    for (auto i = 0; i < (1 << highQubit); ++ i) {
        if ((i & ctrlmask) == 0 && (i & targmask) == targmask) {
            // suppose ctrl > targ
            // i   := |..0..1..>
            // row := |..1..0..>
            row = i ^ ctrlmask ^ targmask;
            // bitset<sizeof(int) * 8> ib(i);
            // bitset<sizeof(int) * 8> rowb(row);
            // cout << "i:\t" << ib << endl;
            // cout << "row:\t" << rowb << endl;

            swapRow(i, row, swap);
        }
    }
    // cout << "GENERATE SWAP GATES\n";
    // swap.print();
}


/* For debug only
int main(int argc, char** argv) {
    if (argc != 3) {
        cout << "[ERROR] usage: cmd <filename> <nWorkers>" << endl;
        exit(1);
    }
    string fname = argv[1];         // the circuit file name
    int nWorkers = atoi(argv[2]);   // #workers

    vector<vector<MatrixImp>> gates;
    GenerateGatesFromFile(gates, fname); // generate a circuit from the file

    int nQubits = gates[0].size();
    int nSteps = gates.size();

    int highQubit = log2(nWorkers);     // #higher qubits
    int lowQubit = nQubits - highQubit; // #lower qubits

    if ((nWorkers & (nWorkers - 1)) != 0) {
        cout << "[ERROR] the number of workers is not a power of 2." << endl;
        exit(1);
    }

    vector<Matrix> subcircuit_matrix; // operation-matrics for subcircuits
    vector<Matrix> tmp_stage_matrix; // temp operation-matrics
    
    Matrix matA, matRes;

    int i, j;
    // calculate the operation-matrix of each level
    for (i = 0; i < nSteps; ++ i) {
        // process all gates at the current level
        for (j = nQubits-1; j >= lowQubit; -- j) {
            // cout << "===================" << endl;
            // cout << "i = " << i << " j = " << j << endl;
            // cout << "===================" << endl;
            // case 1: end of a subcircuit
            if (gates[i][j].gate_name_ == "MERGE") {
                cout << "===== MERGE =====\n";

                // MERGE gateParam1: subcircuit index starting from 0
                int subcId = gates[i][j].control_qubit_;

                // MERGE gateParam2: # of following MSWAP
                int nMSWAPs = gates[i][j].target_qubit_;

                if (j != nQubits - 1) {
                    cout << "[ERROR] Repeatedly processing MERGE!" << endl;
                    exit(1);
                }
                
                // no operation-matrix before the first MERGE
                if (subcId != 0) {
                    // multiplication of temp operation-matrics in <tmp_stage_matrix>
                    matA.copy(tmp_stage_matrix[0]);
                    for (long unsigned int t = 1; t < tmp_stage_matrix.size(); ++ t) {
                        Matrix_Multiplication(matA, tmp_stage_matrix[t], matRes);
                        matA.copy(matRes);
                    }
                    // cout << "MERGE total " << tmp_stage_matrix.size() << " tmp_stage_matrix" << endl;
                    // matA.print();

                    // add the operation-matrx to <subcircuit_matrix>
                    subcircuit_matrix.push_back(matA);

                    // clear <tmp_stage_matrix>
                    tmp_stage_matrix.clear();
                }

                // skip the following MSWAP levels since no operation-matrix needed
                i += nMSWAPs;

                // jump to next level
                break;
            }
            // case 2: middle of a subcircuit
            // matA := store the tensor product of single qubit gates
            // matRes := store the operation-matrix of 2-qubit gates
            if (! gates[i][j].is_control_gate_) {
                // case 2.1: single qubit gates
                if (j == nQubits - 1) {
                    matA.copy(gates[i][j].gate_); // init
                } else {
                    Matrix_TensorProduct(matA, gates[i][j].gate_, matRes); // tensor-product
                    matA.copy(matRes);
                }
            } else {
                // case 2.2: 2-input gates
                // deal with single qubit gates
                if (j == nQubits - 1) {
                    Matrix_Init_IDE(2, matA);
                } else {
                    Matrix_TensorProduct(matA, IDE, matRes);
                    matA.copy(matRes);
                }

                // deal with 2-input gates
                // if the target qubit is in high-order
                // if (gates[i][j].target_qubit_ >= lowQubit) {
                if (gates[i][j].is_target_gate_) {
                    if (gates[i][j].target_qubit_ != j) {
                        // cout << "[INFO] Generate SWAP level [" << i << "] " << gates[i][j].control_qubit_ << " " << gates[i][j].target_qubit_ << endl;
                        // only process the higher swap qubit
                        if (gates[i][j].control_qubit_ > gates[i][j].target_qubit_) {
                            // generate a 2^h x 2^h SWAP operation-matrix
                            GenSwapGate(gates[i][j].control_qubit_-lowQubit, gates[i][j].target_qubit_-lowQubit, highQubit, matRes);
                            tmp_stage_matrix.push_back(matRes); // save the operation-matrix of 2-input gates
                        }
                    } else {
                        // generate a 2^h x 2^h operation-matrix
                        GenControlGate(gates[i][j].gate_, gates[i][j].control_qubit_-lowQubit, j-lowQubit, highQubit, matRes);
                        tmp_stage_matrix.push_back(matRes); // save the operation-matrix of 2-input gates
                    }
                }
            }
        }
        // end of a level of gates
        // save the tensor product result of single qubit gates
        if (j != nQubits-1) { // if j == nQubits-1, it is a MERGE level, no stage matrix
            tmp_stage_matrix.push_back(matA);
        }
    }

    /////////////////////
    // cout << "===== SUBCIRCUIT_MATRIX =====" << endl;
    // for (auto &mat : subcircuit_matrix) {
    //     mat.print();
    // }
}
*/

// g++ -std=c++11 -Wall -Werror -I./util/ gen_gate.cpp ../obj/util/*.o -o gen_gate
// ./gen_gate ./circuit.txt 4