#include "HybridSim.h"

void HybridSim(QCircuit &qc, int memQubits, long long calBlocks) {
    int numQubits = qc.numQubits;
    long long N = (1 << numQubits);

    int highQubits = numQubits - memQubits;
    int lowQubits  = numQubits - highQubits;

    long long H = (1 << highQubits); // the number of blocks
    long long L = (1 << lowQubits);  // the size of each block
    long long numFiles = H * H;      // the number of files

    double simTime = 0.0;
    double ioTimeHigh = 0.0;
    double ioTimeLow = 0.0;

    // 
    // Initialize the state vector
    //
    string dir = "./output/hybrid/";
    if (calBlocks != 1) {
        InitStateVectorSSD(N, numFiles, dir);
    }
    
    Timer timer;
    timer.Start();

    //
    // Calculating high-order operation matrix
    // 
    Matrix opMat;
    Matrix_Init_IDE(H, opMat);
    BuildHighOrderOpMat(opMat, qc, H, lowQubits);

    if (calBlocks == 0) {
        calBlocks = H;
    }

    Matrix localSv = Matrix(L, 1);

    if (calBlocks == 1) {
        localSv.data[0][0] = 1.0;

        for (int i = 0; i < qc.numDepths; ++ i) {
            LocalComputing(localSv, L, qc.gates[i], lowQubits, 0);
        }

        ioTimeHigh += MergeComputingForOneBlock(localSv, opMat, H, dir);
    } else {
        //
        // IndexSim
        //
        for (long long blkNo = 0; blkNo < calBlocks; ++ blkNo) {
            ioTimeLow += ReadBlock(localSv, blkNo, H, dir);
            for (int i = 0; i < qc.numDepths; ++ i) {
                LocalComputing(localSv, L, qc.gates[i], lowQubits, blkNo);
            }
            ioTimeLow += WriteBlock(localSv, blkNo, H, dir);
        }
        // 
        // MatrixSim
        // 
        double ioTimeHighRead = 0.0;
        double ioTimeHighWrite = 0.0;
        for (long long mergeNo = 0; mergeNo < H; ++ mergeNo) {
            ioTimeHighRead += ReadMergeBlock(localSv, mergeNo, H, dir);
            ioTimeHighWrite += MergeComputing(localSv, opMat, mergeNo, H, dir);
        }
        // for (long long mergeNo = 0; mergeNo < calBlocks; ++ mergeNo) {
        //     ioTimeHighRead += ReadMergeBlock(localSv, mergeNo, H, dir);
        // }
        // for (long long mergeNo = 0; mergeNo < H; ++ mergeNo) {
        //     ioTimeHighWrite += MergeComputing(localSv, opMat, mergeNo, H, dir);
        // }
        // cout << "[DEBUG] ioTimeHighRead: " << ioTimeHighRead / 1e6 << " ioTimeHighWrite: " << ioTimeHighWrite / 1e6 << endl;

        ioTimeHigh = ioTimeHighRead + ioTimeHighWrite;
    }

    timer.End();
    simTime = timer.ElapsedTime();

    cout << "[INFO] [HybridSim] simTime:\t" << simTime / 1e6;
    cout << " ioTimeHigh:\t" << ioTimeHigh / 1e6; 
    cout << " ioTimeLow:\t" << ioTimeLow / 1e6;
    cout << " compTime:\t" << (simTime - ioTimeHigh - ioTimeLow) / 1e6 << endl;

    return;
}


void BuildHighOrderOpMat(Matrix &opMat, QCircuit &qc, long long H, int lowQubits) {
    Matrix matA, matB, matRes;
    int i, j;
    int highQubits = qc.numQubits - lowQubits;
    // calculate the operation-matrix of each level
    for (i = 0; i < qc.numDepths; ++ i) {
        // cout << endl << "[DEBUG] Level: " << i << endl;
        // process all gates at the current level
        for (j = qc.numQubits-1; j >= lowQubits; -- j) {
            // matA := store the tensor product of single qubit gates
            // matRes := store the operation-matrix of 2-qubit gates
            if (! qc.gates[i][j].is_control_gate_ && ! qc.gates[i][j].is_target_gate_) {
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
                    // if (qc.gates[i][j].target_qubit_ != j) {
                    if (qc.gates[i][j].is_control_gate_) {
                        // only process the higher swap qubit
                        if (qc.gates[i][j].control_qubit_ > qc.gates[i][j].target_qubit_) {
                            // generate a 2^h x 2^h SWAP operation-matrix
                            GenSwapGate(qc.gates[i][j].control_qubit_-lowQubits, qc.gates[i][j].target_qubit_-lowQubits, highQubits, matB);
                            // save the operation-matrix of 2-input gates
                            Matrix_Multiplication(matB, opMat, matRes);
                            opMat.copy(matRes);
                        }
                    } else {
                        // generate a 2^h x 2^h operation-matrix
                        GenControlGate(qc.gates[i][j].gate_, qc.gates[i][j].control_qubit_-lowQubits, j-lowQubits, highQubits, matB);
                        // cout << "[DEBUG] matB (CX): " << endl;
                        // matB.print();
                        
                        // save the operation-matrix of 2-input gates
                        Matrix_Multiplication(matB, opMat, matRes);
                        opMat.copy(matRes);
                    }
                }
            }
        }
        // end of a level of gates
        // save the tensor product result of single qubit gates
        // cout << "[DEBUG] matA: " << endl;
        // matA.print();
        Matrix_Multiplication(matA, opMat, matRes);
        opMat.copy(matRes);
    }
}


double MergeComputing(Matrix &localV, Matrix &opMat, long long mergeNo, long long H, string dir) {
    long long filename;
    stringstream filenameStream;
    ofstream file;

    long long fileSize = localV.row / H; // the number of amplitudes within each file
    double ans;

    Timer timer;
    double ioTime = 0.0;

    for (long long blkNo = 0; blkNo < H; ++ blkNo) { // [blkNo, mergeNo]
        // calculate the filename
        filename = blkNo * H + mergeNo;

        // open the file
        timer.Start();
        filenameStream.str(""); // clear the stream
        filenameStream << dir << "out" << filename;
        file.open(filenameStream.str());
        timer.End();
        ioTime += timer.ElapsedTime();

        // write the file
        for (long long i = 0; i < fileSize; ++ i) {
            ans = 0.0;
            for (long long j = 0; j < H; ++ j) {
                ans += opMat.data[blkNo][j] * localV.data[j * fileSize + i][0];
            }

            timer.Start();
            file << ans << endl; // write total fileSize amplitudes to file
            timer.End();
            ioTime += timer.ElapsedTime();
        }
        file.close();
    }

    return ioTime;
}


double MergeComputingForOneBlock(Matrix &localV, Matrix &opMat, long long H, string dir) {
    stringstream filenameStream;
    ofstream file;

    long long fileSize = localV.row; // the number of amplitudes within each file
    double ans;

    Timer timer;
    double ioTime = 0.0;

    for (long long blkNo = 0; blkNo < H; ++ blkNo) {
        // open the file
        timer.Start();
        filenameStream.str(""); // clear the stream
        filenameStream << dir << "out" << blkNo;
        file.open(filenameStream.str());
        timer.End();
        ioTime += timer.ElapsedTime();

        // write the file
        for (long long i = 0; i < fileSize; ++ i) {
            ans = opMat.data[blkNo][0] * localV.data[i][0];

            timer.Start();
            file << ans << endl; // write total fileSize amplitudes to file
            timer.End();
            ioTime += timer.ElapsedTime();
        }
        file.close();
    }

    return ioTime;
}