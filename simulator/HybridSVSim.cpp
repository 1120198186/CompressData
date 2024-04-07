#include "HybridSVSim.h"

void HybridSVSim(QCircuit &qc, int memQubits) {
    int numQubits = qc.numQubits;
    long long N = (1 << numQubits);

    int highQubits = numQubits - memQubits; // TODO: merge to the file
    int lowQubits  = numQubits - highQubits;

    long long H = (1 << highQubits); // the number of blocks
    long long L = (1 << lowQubits);  // the size of each block
    long long numFiles = H * H;      // the number of files

    // 
    // Initialize the state vector
    //
    string dir = "./output/hybrid/";
    InitStateVectorSSD(N, numFiles, dir);

    //
    // An independent thread for calculating high-order operation matrix
    // 
    Matrix opMat;
    Matrix_Init_IDE(H, opMat);
    BuildHighOrderOpMat(opMat, qc, H, lowQubits);
    // TODO: thread myThread(std::bind(BuildHighOrderOpMat, opMat, qc, H, lowQubits));

    //
    // Local svsim for each block
    //
    Matrix localSv = Matrix(L, 1);
    for (long long blkNo = 0; blkNo < H; ++ blkNo) {
        ReadBlock(localSv, blkNo, H, dir);

        for (int i = 0; i < qc.numDepths; ++ i) {
            LocalComputing(localSv, L, qc.gates[i], lowQubits, blkNo);
        }

        WriteBlock(localSv, blkNo, H, dir);
    }

    // myThread.join();
    cout << "[DEBUG] opMat =====" << endl;
    opMat.print();

    //
    // Merge
    //
    for (long long mergeNo = 0; mergeNo < H; ++ mergeNo) {
        ReadMergeBlock(localSv, mergeNo, H, dir);
        MergeComputing(localSv, opMat, mergeNo, H, dir);
    }

    return;
}


void BuildHighOrderOpMat(Matrix &opMat, QCircuit &qc, long long H, int lowQubits) {
    Matrix matA, matB, matRes;
    int i, j;
    int highQubits = qc.numQubits - lowQubits;
    // calculate the operation-matrix of each level
    for (i = 0; i < qc.numDepths; ++ i) {
        // process all gates at the current level
        for (j = qc.numQubits-1; j >= lowQubits; -- j) {
            // matA := store the tensor product of single qubit gates
            // matRes := store the operation-matrix of 2-qubit gates
            if (! qc.gates[i][j].is_control_gate_) {
                // case 1: single qubit gates
                if (j == qc.numQubits - 1) {
                    matA.copy(qc.gates[i][j].gate_); // init
                } else {
                    Matrix_TensorProduct(matA, qc.gates[i][j].gate_, matRes); // tensor-product
                    matA.copy(matRes);
                }
            } else {
                // case 2: 2-input gates
                // deal with single qubit gates
                if (j == qc.numQubits - 1) {
                    Matrix_Init_IDE(2, matA);
                } else {
                    Matrix_TensorProduct(matA, IDE, matRes);
                    matA.copy(matRes);
                }

                // deal with 2-input gates
                // if the target qubit is in high-order
                // if (gates[i][j].target_qubit_ >= lowQubit) {
                if (qc.gates[i][j].is_target_gate_) {
                    if (qc.gates[i][j].target_qubit_ != j) {
                        // cout << "[INFO] Generate SWAP level [" << i << "] " << gates[i][j].control_qubit_ << " " << gates[i][j].target_qubit_ << endl;
                        // only process the higher swap qubit
                        if (qc.gates[i][j].control_qubit_ > qc.gates[i][j].target_qubit_) {
                            // generate a 2^h x 2^h SWAP operation-matrix
                            GenSwapGate(qc.gates[i][j].control_qubit_-lowQubits, qc.gates[i][j].target_qubit_-lowQubits, highQubits, matB);
                            // save the operation-matrix of 2-input gates
                            Matrix_Multiplication(opMat, matB, matRes);
                            opMat.copy(matRes);
                        }
                    } else {
                        // generate a 2^h x 2^h operation-matrix
                        GenControlGate(qc.gates[i][j].gate_, qc.gates[i][j].control_qubit_-lowQubits, j-lowQubits, highQubits, matB);
                        // save the operation-matrix of 2-input gates
                        Matrix_Multiplication(opMat, matB, matRes);
                        opMat.copy(matRes);
                    }
                }
            }
        }
        // end of a level of gates
        // save the tensor product result of single qubit gates
        if (j != qc.numQubits-1) { // if j == nQubits-1, it is a MERGE level, no stage matrix
            Matrix_Multiplication(opMat, matA, matRes);
            opMat.copy(matRes);
        }
    }
}


void MergeComputing(Matrix &localV, Matrix &opMat, long long mergeNo, long long H, string dir) {
    long long filename;
    stringstream filenameStream;
    ofstream file;

    long long fileSize = localV.row / H; // the number of amplitudes within each file
    double ans;

    for (long long blkNo = 0; blkNo < H; ++ blkNo) { // [blkNo, mergeNo]
        // calculate the filename
        filename = blkNo * H + mergeNo;

        // open the file
        filenameStream.str(""); // clear the stream
        filenameStream << dir << "out" << filename << ".txt";
        file.open(filenameStream.str());
        cout << "[DEBUG] filename = " << filenameStream.str() << endl;

        // write the file
        for (long long i = 0; i < fileSize; ++ i) {
            ans = 0.0;
            for (long long j = 0; j < H; ++ j) {
                ans += opMat.data[blkNo][j] * localV.data[j * fileSize + i][0];
                cout << "opMat[" << blkNo << "][" << j << "] * localV[" << j * fileSize + i << "][0] = " << opMat.data[blkNo][j] << " * " << localV.data[j * fileSize + i][0] << endl;
            }
            file << ans << endl; // write total fileSize amplitudes to file
        }
        file.close();
    }
    return;
}