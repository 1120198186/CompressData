//
// Low-order Indexing
//

#include "indexing.h"

/**
 * @brief [Util] Local indexing for 2x2 gates
 * 
 * @param g 2x2 gate matrix
 * @param a amplitude 0
 * @param b amplitude 1
 */
void mutiplyWithGate2x2(Matrix &g, double &a, double &b) {
    double temp0 = a*g.data[0][0] + b*g.data[0][1];
    double temp1 = a*g.data[1][0] + b*g.data[1][1];
    a = temp0;
    b = temp1;
}


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
        MatrixImp &gate, int qid, int lowQubit, int myRank) {
    // cout << "[INFO] indexing for 2x2 gates and CU gates" << endl;
    
    bool isAccessed[localVLen];
    memset(isAccessed, 0, localVLen*sizeof(bool));

    long long stride = (1 << qid); // stride = 2^qid for gate applied on q[qid]

    for (long long j = 0; j < localVLen; ++ j) {
        if (isAccessed[j]) continue;

        if (j + stride >= localVLen) {
            cout << "[ERROR] LocalComputing exceeds the length of state vector." << endl;
            exit(1);
        }

        isAccessed[j] = isAccessed[j + stride] = true;

        // deal with controlled gates
        if (gate.is_target_gate_) {
            int ctrl = gate.control_qubit_; // the ctrl bit whose range is [0, n-1]

            // check if the control qubit is 1
            if (ctrl < lowQubit) { // mask with j
                // a local control qubit
                long long ctrlmask = (1 << ctrl); // ctrl qubit = 1, else 0
                if ((j & ctrlmask) == 0) { // j is the binary index formed by low-order qubits
                    continue; // the control qubit is 0, do nothing
                }
            } else { // ctrl >= lowQubit, should mask with myRank
                // a high-order control qubit
                long long ctrlmask = (1 << (ctrl - lowQubit));
                if ((myRank & ctrlmask) == 0) { // myRank is the binary index formed by high-order qubits
                    continue; // the control qubit is 0, do nothing
                }
            }
        }

        // indexing
        mutiplyWithGate2x2(gate.gate_, localV.data[j][0], localV.data[j + stride][0]);
    }
}


/**
 * @brief Local indexing for 4x4 gates
 * 
 * @param g 4x4 gate matrix
 * @param a amplitude 0
 * @param b amplitude 1
 * @param c amplitude 2
 * @param d amplitude 3
 */
void multiplyWithGate4x4(Matrix &g, double &a, double &b, double &c, double &d) {
    double temp0 = a*g.data[0][0] + b*g.data[0][1] + c*g.data[0][2] + d*g.data[0][3];
    double temp1 = a*g.data[1][0] + b*g.data[1][1] + c*g.data[1][2] + d*g.data[1][3];
    double temp2 = a*g.data[2][0] + b*g.data[2][1] + c*g.data[2][2] + d*g.data[2][3];
    double temp3 = a*g.data[3][0] + b*g.data[3][1] + c*g.data[3][2] + d*g.data[3][3];
    a = temp0;
    b = temp1;
    c = temp2;
    d = temp3;
}


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
        MatrixImp &gate, int lowQubit) {
    // cout << "[INFO] Indexing for 4x4 gates" << endl;

    int ctrl = gate.control_qubit_;
    int targ = gate.target_qubit_;

    if (ctrl >= lowQubit || targ >= lowQubit) {
        cout << "[ERROR] The SWAP gate exceeds local qubits. " << endl;
        exit(1);
    }

    if (ctrl <= targ) {
        // cout << "[INFO] Skip low SWAP" << endl;
        return;
    }

    bool isAccessed[localVLen];
    memset(isAccessed, 0, localVLen*sizeof(bool));

    /*  Since 2 input qubits are local, the amplitude starts from |0...0>
           c  t
        |..0..0..> -+------------------+
                    | strideLow = 2^t  |
        |..0..1..> -+                  |
                                       | strideHigh = 2^c
        |..1..0..> -+------------------+
                    | strideLow = 2^t
        |..1..1..> -+
    */
    long long strideLow = (1 << targ);
    long long strideHigh = (1 << ctrl);

    for (long long j = 0; j < localVLen; ++ j) {
        if (isAccessed[j]) continue;

        if (j + strideHigh + strideLow >= localVLen) {
            cout << "[ERROR] LocalComputing exceeds the length of state vector." << endl;
            exit(1);
        }

        isAccessed[j] = isAccessed[j + strideLow] = isAccessed[j + strideHigh] = isAccessed[j + strideHigh + strideLow] = true;

        multiplyWithGate4x4(gate.gate_, localV.data[j][0], localV.data[j + strideLow][0], localV.data[j + strideHigh][0], localV.data[j + strideHigh + strideLow][0]);
    }
}


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
        vector<MatrixImp> &GateSeq, int lowQubit, int myRank) {
    if ((localVLen & (localVLen - 1)) != 0) {
        cout << "[ERROR] the length of state vector is not a power of 2." << endl;
        exit(1);
    }

    for (int i = lowQubit - 1; i >= 0; -- i) {
        if (GateSeq[i].gate_name_ == "IDE") {
            continue;
        }

        if (GateSeq[i].is_control_gate_ && ! GateSeq[i].is_target_gate_) {
            continue; // the control qubit of a controlled gate
        }

        if (GateSeq[i].gate_.row == 2 && GateSeq[i].gate_.col == 2) {
            // indexing with gate 2x2
            IndexingWithGate2x2(localV, localVLen, GateSeq[i], i, lowQubit, myRank);

        } else if (GateSeq[i].gate_.row == 4 && GateSeq[i].gate_.col == 4) {
            // indexing with gate 4x4
            IndexingWithGate4x4(localV, localVLen, GateSeq[i], lowQubit);
        } else {
            cout << "[ERROR] Unimplemented gates " << GateSeq[i].gate_.row << "x" << GateSeq[i].gate_.col << endl;
            exit(1);
        }
    }

    // if (myRank == 0) {
    //     cout << "LOCAL V\n";
    //     localV.print();
    // }
}

/*
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
    long long localVLen = (1 << lowQubit);  // local V len = 2^l

    if ((nWorkers & (nWorkers - 1)) != 0) {
        cout << "[ERROR] the number of workers is not a power of 2." << endl;
        exit(1);
    }

    int myRank = 0;

    // //////////////////////////////////////////////////////////////////
    // local computing for gates on low-order qubits and merge
    // //////////////////////////////////////////////////////////////////
    Matrix localV = Matrix(localVLen, 1); // 2^l x 2^l

    // initialization
    if (myRank == 0) {
        localV.data[0][0] = 1;
    }

    // local computing
    for (int i = 0; i < nSteps; ++ i) {
        cout << "\n\n========== RANK [" << myRank << "] LEVEL [" << i << "] ==========" << endl; 
        // encounter merge level
        if (gates[i][0].gate_name_ == "MERGE") {
            
            // MERGE gateParam1: subcircuit index starting from 0
            int subcId = gates[i][0].control_qubit_;

            // MERGE gateParam2: # of following MSWAP
            int nMSWAPs = gates[i][0].target_qubit_;

            if (subcId != 0) { // no operation-matrix before the first MERGE
                // get the high-order operation-matrix from subcircuit_matrix[subcId-1]
                cout << "===== MERGE AT SUBC[" << subcId << "] =====\n";
                // sem_wait(&sem);
                // MergeComputing(localV, localVLen, nWorkers, subcircuit_matrix[subcId - 1]);
            }

            for (int j = 0; j < nMSWAPs; ++ j) {
                ++ i;
                cout << "===== MSWAP AT LEVEL[" << i << "] =====\n";
                // MswapComputing(localV, localVLen, gates[i][0], lowQubit, myRank);
            }
        } else {
            // indexing on low-order qubits
            LocalComputing(localV, localVLen, gates[i], lowQubit, myRank);
            cout << "After indexing" << endl;
            localV.print();
        }
    }

}
*/

// g++ -std=c++11 -Wall -Werror -I./util/ indexing.cpp ../obj/util/*.o -o indexing
// ./indexing ./circuit.txt 4