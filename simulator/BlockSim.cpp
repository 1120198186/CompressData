#include "BlockSim.h"

double BlockSim(QCircuit &qc, int memQubits) {
    int numQubits = qc.numQubits;
    long long N = (1 << numQubits);

    // For a 2-qubit non-controlled gate, 4 blocks should be read to the memory
    int highQubits = numQubits - memQubits + 2;
    int lowQubits  = numQubits - highQubits;

    long long H = (1 << highQubits); // the number of blocks
    long long L = (1 << lowQubits);  // the size of each block

    double ioTime = 0.0;

    // 
    // Initialize the state vector
    //
    string dir = "./output/block/";
    ioTime += InitStateVectorSSD(N, H, dir);

    Matrix localSv0 = Matrix(L, 1);
    Matrix localSv1 = Matrix(L, 1);
    Matrix localSv2 = Matrix(L, 1);
    Matrix localSv3 = Matrix(L, 1);

    //
    // Simulate the circuit level by level
    //
    int ctrl, targ;
    long long blkStride, blkStride1, ctrlmask;
    bool isAccessed[H]; // mark if the block is accessed
    for (int i = 0; i < qc.numDepths; ++ i) {

        // low-order gates
        for (long long blkNo = 0; blkNo < H; ++ blkNo) {
            ioTime += ReadBlock(localSv0, blkNo, 1, dir);
            LocalComputing(localSv0, L, qc.gates[i], lowQubits, blkNo);
            ioTime += WriteBlock(localSv0, blkNo, 1, dir);
        }

        // high-order gates
        for (int j = qc.numQubits - 1; j >= lowQubits; -- j) {
            if (qc.gates[i][j].gate_name_ == "IDE") {
                continue;
            }

            else if (qc.gates[i][j].is_control_gate_ && ! qc.gates[i][j].is_target_gate_) {
                continue; // the control qubit of a controlled gate
            }

            // single-qubit gates and 2-qubit controlled gates: a high-order target gate
            else if (qc.gates[i][j].gate_.row == 2) {
                memset(isAccessed, 0, H*sizeof(bool));
                blkStride = (1 << (j - lowQubits));

                for (long long blkNo = 0; blkNo < H; ++ blkNo) {
                    if (isAccessed[blkNo]) continue;

                    // read two blocks
                    ioTime += ReadBlock(localSv0, blkNo, 1, dir);
                    ioTime += ReadBlock(localSv1, blkNo + blkStride, 1, dir);

                    isAccessed[blkNo] = isAccessed[blkNo + blkStride] = true;

                    // update the state vector
                    for (long long idx = 0; idx < L; ++ idx) {

                        if (qc.gates[i][j].is_target_gate_) {
                            // skip amplitude pairs whose ctrl qubits are 0
                            ctrl = qc.gates[i][j].control_qubit_;

                            if (ctrl < lowQubits) {
                                ctrlmask = (1 << ctrl);
                                if ((idx & ctrlmask) == 0) {
                                    continue;
                                }
                            } else {
                                ctrlmask = (1 << (ctrl - lowQubits));
                                if ((blkNo & ctrlmask) == 0) {
                                    continue;
                                }
                            }
                        }

                        // indexing
                        multiplyWithGate2x2(qc.gates[i][j].gate_, localSv0.data[idx][0], localSv1.data[idx][0]);
                    }

                    // write back
                    ioTime += WriteBlock(localSv0, blkNo, 1, dir);
                    ioTime += WriteBlock(localSv1, blkNo + blkStride, 1, dir);
                }
            }

            // 2-qubit non-controlled gates
            else if (qc.gates[i][j].gate_.row == 4) {

                ctrl = qc.gates[i][j].control_qubit_;
                targ = qc.gates[i][j].target_qubit_;

                if (ctrl > targ) { // the high-order gate
                    memset(isAccessed, 0, H*sizeof(bool));

                    if (targ < lowQubits) { // one high, one low, inseparable
                        cout << "[ERROR] Inseparable circuit!" << endl;
                        exit(1);
                    }

                    blkStride = (1 << (targ - lowQubits)); // low stride
                    blkStride1 = (1 << (ctrl - lowQubits)); // high stride

                    for (long long blkNo = 0; blkNo < H; ++ blkNo) {
                        if (isAccessed[blkNo]) continue;

                        // read four blocks
                        ioTime += ReadBlock(localSv0, blkNo, 1, dir);
                        ioTime += ReadBlock(localSv1, blkNo + blkStride, 1, dir);
                        ioTime += ReadBlock(localSv2, blkNo + blkStride1, 1, dir);
                        ioTime += ReadBlock(localSv3, blkNo + blkStride + blkStride1, 1, dir);

                        isAccessed[blkNo] = isAccessed[blkNo + blkStride] = isAccessed[blkNo + blkStride1] = isAccessed[blkNo + blkStride + blkStride1] = true;

                        // update the state vector
                        for (long long idx = 0; idx < L; ++ idx) {
                            multiplyWithGate4x4(qc.gates[i][j].gate_, localSv0.data[idx][0], localSv1.data[idx][0], localSv2.data[idx][0], localSv3.data[idx][0]);
                        }

                        // write back
                        ioTime += WriteBlock(localSv0, blkNo, 1, dir);
                        ioTime += WriteBlock(localSv1, blkNo + blkStride, 1, dir);
                        ioTime += WriteBlock(localSv2, blkNo + blkStride1, 1, dir);
                        ioTime += WriteBlock(localSv3, blkNo + blkStride + blkStride1, 1, dir);
                    }
                }
            }
            
            else {
                cout << "[ERROR] Unimplemented gates " << qc.gates[i][j].gate_.row << "x" << qc.gates[i][j].gate_.col << endl;
                exit(1);
            }
        }
    }

    return ioTime;
}
