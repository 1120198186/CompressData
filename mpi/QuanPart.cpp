#include <mpi.h>
#include <string>
#include <iostream>
#include <math.h>
#include <semaphore.h>
#include <thread>

#include "matrix_wrap.h"
#include "gen_gate.h" // high-order operation-matrix
#include "indexing.h" // low-order indexing
#include "log.h"
#include "qcircuit.h"


// //////////////////////////////////////////
// Merge
// //////////////////////////////////////////


/**
 * @brief Merge at the end of a subcircuit
 * 
 * @param localV    local state-vector
 * @param localVLen length of local state-vector, i.e., 2^l
 * @param numBlocks  #workers
 * @param mat       the operation-matrix of high-order qubits
 * 
 * @return communication time
 */
double MergeComputing(Matrix &localV, long long localVLen, int numBlocks, Matrix &mat, int myRank) {
    long long group = localVLen / numBlocks; // #communication unit
    double commTime = 0.0; // record communication time
    double startTime, endTime;

    double *sendbuf = new double[localVLen];
    if (sendbuf == nullptr) {
        cout << "[ERROR] Not enough space! " << endl;
        exit(1);
    }

    for (long long k = 0; k < localVLen; ++ k) {
        memcpy(sendbuf + k, localV.data[k], sizeof(double));
    }
    
    double *recvbuf = new double[localVLen];
    if (recvbuf == nullptr) {
        cout << "[ERROR] Not enough space! " << endl;
        exit(1);
    }
    memset(recvbuf, 0, localVLen * sizeof(double));

    //
    // all-to-all communication 1
    //
    startTime = MPI_Wtime();
    int ret = MPI_Alltoall(sendbuf, group, MPI_DOUBLE, recvbuf, group, MPI_DOUBLE, MPI_COMM_WORLD);
    endTime = MPI_Wtime();
    if (ret != MPI_SUCCESS) {
        cout << "[ERROR] Rank " << myRank << " MPI_Alltoall 1 failed. " << endl;
    }
    // MPI_Alltoall(localV.data, group, MPI_DOUBLE, buffer, group, MPI_DOUBLE, MPI_COMM_WORLD);
    for (long long k = 0; k < localVLen; ++ k) {
        memcpy(localV.data[k], recvbuf + k, sizeof(double));
    }
    commTime += endTime - startTime;

    //
    // local computation after communication
    //
    memset(sendbuf, 0, localVLen * sizeof(double));

    if (mat.row != numBlocks || mat.col != numBlocks) {
        cout << "[ERROR] Incorrect operation-matrix size! " << endl;
        exit(1);
    }

    long long iStart, jStart;
    for (int i = 0; i < numBlocks; ++ i) {
        iStart = i * group; // the start point of each group
        for (int j = 0; j < numBlocks; ++ j) {
            jStart = j * group; // the offset within each group
            for (long long k = 0; k < group; ++ k) {
                sendbuf[iStart + k] += mat.data[i][j] * localV.data[jStart + k][0];
            }
        }
    }

    //
    // all-to-all communication 2
    //
    memset(recvbuf, 0, localVLen * sizeof(double));
    startTime = MPI_Wtime();
    ret = MPI_Alltoall(sendbuf, group, MPI_DOUBLE, recvbuf, group, MPI_DOUBLE, MPI_COMM_WORLD);
    endTime = MPI_Wtime();
    if (ret != MPI_SUCCESS) {
        cout << "[ERROR] Rank " << myRank << " MPI_Alltoall 2 failed. " << endl;
    }
    for (long long k = 0; k < localVLen; k++) {
        memcpy(localV.data[k], recvbuf + k, sizeof(double));
    }
    commTime += endTime - startTime;

    delete [] sendbuf;
    delete [] recvbuf;

    return commTime;
}



int main(int argc, char** argv) {
    if (argc != 4) {
        cout << "[ERROR] usage: cmd <numQubits> <lowQubits> <numDepths>" << endl;
        exit(1);
    }

    srand((int)time(0));

    int numQubits = atoi(argv[1]);
    int lowQubits = atoi(argv[2]); // the number of low-order qubits
    int numDepths = atoi(argv[3]);
    // double sum = 0.0;
    // int epoch = atoi(argv[4]);

    int highQubits = numQubits - lowQubits; // the number of high-order qubits
    long long localVLen = (1 << lowQubits);
    long long numBlocks = (1 << highQubits); // the number of blocks

    // 
    // Generate a quantum circuit
    // 
    // Random /////
    QCircuit qc = QCircuit(numQubits);
    qc.setDepths(numDepths);
    qc.fill(0);

    // VQC /////
    // QCircuit qc = VQC(numQubits);

    // QFT /////
    // QCircuit qc = QFT(numQubits);

    //
    // MPI initialization
    //
    int myRank; // my process rank
    MPI_Init(nullptr, nullptr);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    MPI_Barrier(MPI_COMM_WORLD);
    double wholeStartTime = MPI_Wtime(); // record the start time

    //
    // Initialize the operation-matrix
    // TODO: only one process build the operation matrix
    //
    Matrix opMat, matA, matB, matRes;
    Matrix_Init_IDE(numBlocks, opMat);
    // TODO: calculate the opMat for the high-order qubits
    // 
    // an independent thread for higher qubits to compute the big matrix
    // 
    thread t1([&]() {
        // cout << "RANK [" << myRank << "] START THREAD" << endl;
        int i, j;
        // calculate the operation-matrix of each level
        for (i = 0; i < qc.numDepths; ++ i) {
            // process all gates at the current level
            for (j = numQubits-1; j >= lowQubits; -- j) {
                // matA := store the tensor product of single qubit gates
                // matRes := store the operation-matrix of 2-qubit gates
                if (! qc.gates[i][j].is_control_gate_) {
                    // case 1: single qubit gates
                    if (j == numQubits - 1) {
                        matA.copy(qc.gates[i][j].gate_); // init
                    } else {
                        Matrix_TensorProduct(matA, qc.gates[i][j].gate_, matRes); // tensor-product
                        matA.copy(matRes);
                    }
                } else {
                    // case 2: 2-input gates
                    // deal with single qubit gates
                    if (j == numQubits - 1) {
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
                            // opMat.push_back(matRes);
                            // save the operation-matrix of 2-input gates
                            Matrix_Multiplication(opMat, matB, matRes);
                            opMat.copy(matRes);
                        }
                    }
                }
            }
            // end of a level of gates
            // save the tensor product result of single qubit gates
            if (j != numQubits-1) { // if j == nQubits-1, it is a MERGE level, no stage matrix
                Matrix_Multiplication(opMat, matA, matRes);
                opMat.copy(matRes);
                // opMat.push_back(matA);
            }
        }
        // cout << "RANK [" << myRank << "] FINISH THREAD" << endl;
    });

    //
    // Initialize the state vector
    //
    // TODO: block-based-compression + indexing
    // Start compressing this localSv
    // Replace the code here with compressed initial state
    // Add necessary compression code in ../util/compress.h
    //
    // Block-based indexing without compression
    // Write H-1 blocks to files, i.e., SSD
    //
    Matrix localSv = Matrix(localVLen, 1);
    if (myRank == 0) {
        localSv.data[0][0] = 1;
    }
    // cout << "[DEBUG] Initialize the state vector. " << endl;
    // TODO: load initial state vector from file

    // 
    // Indexing simulations
    // 
    // auto start = chrono::high_resolution_clock::now();
    // TODO: check if some of the blocked-state-vectors are zero or the same
    for (int i = 0; i < qc.numDepths; ++ i) {
        // LocalComputing does one level of indexing for the localSv of size 2^(n-h)
        // TODO: 这里不涉及到多块参与的计算，多块参与的indexing需要另外实现
        LocalComputing(localSv, localVLen, qc.gates[i], lowQubits, myRank);
    }
    // cout << "[DEBUG] Finish indexing. " << endl;
    // auto end = chrono::high_resolution_clock::now();
    // auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    t1.join(); // blocking current process until thread ending
    
    // 
    // Merge
    //
    double commTime = MergeComputing(localSv, localVLen, numBlocks, opMat, myRank);
    cout << "[DEBUG] Finish merge. " << endl;
    
    //
    // Timer
    //
    double wholeEndTime = MPI_Wtime();
    double wholeTime = wholeEndTime - wholeStartTime;

    MPI_Barrier(MPI_COMM_WORLD);

    // record timer for all workers
    double wholeTimer[numBlocks];
    memset(wholeTimer, 0, sizeof(double) * (numBlocks));
    double communicationTimer[numBlocks];
    memset(communicationTimer, 0, sizeof(double) * (numBlocks));

    double buffer[2];
    buffer[0] = wholeTime;
    buffer[1] = commTime;

    MPI_Status status;
    if (myRank == 0) {
        wholeTimer[0] = buffer[0];
        communicationTimer[0] = buffer[1];
        for (int j = 1; j < numBlocks; j++) {
            MPI_Recv(buffer, 2, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            wholeTimer[status.MPI_SOURCE] = buffer[0];
            communicationTimer[status.MPI_SOURCE] = buffer[1];
            // cout << "[DEBUG] Recv timer from " << status.MPI_SOURCE << endl;
        }
    } else {
        MPI_Send(buffer, 2, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }
    
    if (myRank == 0) {
        LogStartInfo("QuanPart", "Hybrid SVSim");
        double minCommunicationTime = INFINITY;
        for (int i = 0; i < numBlocks; i++) {
            if (communicationTimer[i] < minCommunicationTime) {
                minCommunicationTime = communicationTimer[i];
                wholeTime = wholeTimer[i];
            }
        }
        LogTimeInfo(wholeTime - minCommunicationTime, minCommunicationTime, wholeTime);
    }
    
    stringstream filenameStream;
    filenameStream << "./output/" << myRank << ".txt";
    std::string outfile = filenameStream.str();

    freopen(outfile.c_str(), "a", stdout);
    // MPI_Barrier(MPI_COMM_WORLD);
    localSv.print();
    // for (int i = 0; i < 10; ++ i) {
    //     cout << localSv.data[i][0] << " ";
    // }
    // cout << endl;

    MPI_Finalize();
    return 0;
}