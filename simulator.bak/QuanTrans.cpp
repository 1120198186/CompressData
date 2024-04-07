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

using namespace std;

const int kLocalVTag = 1;
const int kLocalTaskDoneTag = 2;
const int kDistributionTag = 3;
const int kPeerTag = 4;
const int kGateDoneTag = 5;


// //////////////////////////////////////////
// Merge
// //////////////////////////////////////////


/**
 * @brief Merge at the end of a subcircuit
 * 
 * @param localV    local state-vector
 * @param localVLen length of local state-vector, i.e., 2^l
 * @param nWorkers  #workers
 * @param mat       the operation-matrix of high-order qubits
 * 
 * @return communication time
 */
double MergeComputing(Matrix &localV, long long localVLen, int nWorkers, Matrix &mat, int myRank) {
    long long group = localVLen / nWorkers; // #communication unit
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

    if (mat.row != nWorkers || mat.col != nWorkers) {
        cout << "[ERROR] Incorrect operation-matrix size! " << endl;
        exit(1);
    }

    long long iStart, jStart;
    for (int i = 0; i < nWorkers; ++ i) {
        iStart = i * group; // the start point of each group
        for (int j = 0; j < nWorkers; ++ j) {
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


/**
 * @brief Processing SWAP gates after merging, communication required
 * 
 * @param localV    local state-vector
 * @param localVLen length of local state-vector, i.e., 2^l
 * @param MSWAP     a MSWAP gate with ctrl and targ qubits
 * @param lowQubit  #low qubits, i.e., l
 * @param myRank    the rank of the current MPI process
 * 
 * @return communication time
 */
double MswapComputing(Matrix &localV, long long localVLen, MatrixImp &MSWAP, int lowQubit, int myRank) {
    //           c  t
    // find   |..0..1..>
    // locate |..1..0..>
    // TODO: try MPI_ISend and MPI_IRecv

    double commTime = 0.0; // record communication time
    double startTime, endTime;

    int ctrl = MSWAP.control_qubit_; // high-order
    int targ = MSWAP.target_qubit_;  // low-order

    if (ctrl < targ) {
        cout << "[ERROR] MSWAP ctrl " << ctrl << " < targ " << targ << endl;
        exit(1);
    }

    long long ctrlmask = (1 << (ctrl - lowQubit)); // high-order: mask with myRank
    long long targmask = (1 << targ); // low-order: mask with local amplitudes

    vector<int> bitIndex; // swapped qubits' indices
    double buffer[localVLen / 2]; // swapped amplitudes
    long long cnt = 0;
    int ret;

    long long pairRank = myRank ^ ctrlmask;   // the communication target node
                                              //    c  t
    if ((myRank & ctrlmask) == 0) {           // |..0..x..>
        // prepare amplitudes
        for (long long j = 0; j < localVLen; ++ j) {
            if ((j & targmask) == targmask) { // |..0..1..>
                bitIndex.push_back(j);
                buffer[cnt ++] = localV.data[j][0];
            }
        }
        if (cnt != localVLen / 2) {
            cout << "[ERROR] MSWAP: comm data is not localVLen / 2: " << cnt << endl;
            exit(1);
        }

        // communication
        startTime = MPI_Wtime(); // start timer
        ret = MPI_Send(buffer, cnt, MPI_DOUBLE, pairRank, 0, MPI_COMM_WORLD);
        if (ret != MPI_SUCCESS) {
            cout << "[ERROR] Rank " << myRank << " MPI_Send failed. " << endl;
        }
        ret = MPI_Recv(buffer, cnt, MPI_DOUBLE, pairRank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (ret != MPI_SUCCESS) {
            cout << "[ERROR] Rank " << myRank << " MPI_Recv failed. " << endl;
        }
        endTime = MPI_Wtime();
        commTime += endTime - startTime;

        // update amplitudes
        cnt = 0;
        for (auto &idx : bitIndex) {
            localV.data[idx][0] = buffer[cnt ++];
        }

    } else {                           // |..1..x..>
        // receive amplitudes
        startTime = MPI_Wtime();
        ret = MPI_Recv(buffer, localVLen / 2, MPI_DOUBLE, pairRank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        endTime = MPI_Wtime();
        commTime += endTime - startTime;

        if (ret != MPI_SUCCESS) {
            cout << "[ERROR] Rank " << myRank << " MPI_Recv failed. " << endl;
        }
        
        // update and prepare amplitudes
        double temp = 0;
        for (long long j = 0; j < localVLen; ++ j) {
            if ((j & targmask) == 0) { // |..1..0..>
                temp = localV.data[j][0];
                localV.data[j][0] = buffer[cnt];
                buffer[cnt ++] = temp;
            }
        }
        if (cnt != localVLen / 2) {
            cout << "[ERROR] MSWAP: comm data is not localVLen / 2: " << cnt << endl;
            exit(1);
        }

        // send amplitudes
        startTime = MPI_Wtime();
        ret = MPI_Send(buffer, cnt, MPI_DOUBLE, pairRank, 0, MPI_COMM_WORLD);
        endTime = MPI_Wtime();
        commTime += endTime - startTime;

        if (ret != MPI_SUCCESS) {
            cout << "[ERROR] Rank " << myRank << " MPI_Send failed. " << endl;
        }
    }
    // endTime = MPI_Wtime();
    // commTime = endTime - startTime;

    return commTime;
}



int main(int argc, char** argv) {
    if (argc != 4) {
        cout << "[ERROR] usage: cmd <filename> <nWorkers> <outputFname>" << endl;
        exit(1);
    }
    string fname = argv[1];         // the circuit file name
    int nWorkers = atoi(argv[2]);   // #workers
    freopen(argv[3], "a", stdout);

    vector<vector<MatrixImp>> gates;
    GenerateMatrixImpsFromFile(gates, fname); // generate a circuit from the file

    int nQubits = gates[0].size();
    int nSteps = gates.size();

    int highQubit = log2(nWorkers);     // #higher qubits
    int lowQubit = nQubits - highQubit; // #lower qubits

    // cout << endl << "nQubits: " << nQubits << " nSteps: " << nSteps << endl;

    // for (long unsigned int i = 0; i < gates.size(); ++ i) {
    //     for (long unsigned int j = 0; j < gates[i].size(); ++ j ) {
    //         cout << gates[i][j].gate_name_ << " ";
    //     }
    //     cout << endl;
    // }
    // cout << endl;

    long long localVLen = (1 << lowQubit);  // local V len = 2^l

    if ((nWorkers & (nWorkers - 1)) != 0) {
        cout << "[ERROR] the number of workers is not a power of 2." << endl;
        exit(1);
    }

    // MPI initialization
    int myRank; // my process rank
    MPI_Init(nullptr, nullptr);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    
    // temp matrix in higher qubits for each step
    sem_t isGend; // if the operation-matrix has been generated
    sem_t isUsed; // if the operation-matrix has been used
    sem_init(&isGend, 0, 0);
    sem_init(&isUsed, 0, 1);
    // vector<Matrix> subcircuit_matrix; // operation-matrics for subcircuits
    Matrix subcircuit_matrix;
    Matrix tmp_stage_matrix; // temp operation-matrics
    Matrix_Init_IDE(nWorkers, tmp_stage_matrix);
    Matrix matA, matB, matRes;

    MPI_Barrier(MPI_COMM_WORLD);
    double wholeStartTime = MPI_Wtime(); // record the start time

    double wholeTime = 0.0, commTime = 0.0; // sum
    
    // //////////////////////////////////////////////////////////////////
    // an independent thread for higher qubits to compute the big matrix
    // //////////////////////////////////////////////////////////////////
    thread t1([&]() {
        // cout << "RANK [" << myRank << "] START THREAD" << endl;
        int i, j;
        // calculate the operation-matrix of each level
        for (i = 0; i < nSteps; ++ i) {
            // process all gates at the current level
            for (j = nQubits-1; j >= lowQubit; -- j) {
                // case 1: end of a subcircuit
                if (gates[i][j].gate_name_ == "MERGE") {
                    
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
                        // add the operation-matrx to <subcircuit_matrix>
                        sem_wait(&isUsed);
                        // subcircuit_matrix.push_back(tmp_stage_matrix);
                        subcircuit_matrix.copy(tmp_stage_matrix);
                        sem_post(&isGend);
                        Matrix_Init_IDE(nWorkers, tmp_stage_matrix);
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
                                GenSwapGate(gates[i][j].control_qubit_-lowQubit, gates[i][j].target_qubit_-lowQubit, highQubit, matB);
                                // save the operation-matrix of 2-input gates
                                // tmp_stage_matrix.push_back(matRes);
                                Matrix_Multiplication(tmp_stage_matrix, matB, matRes);
                                tmp_stage_matrix.copy(matRes);
                            }
                        } else {
                            // generate a 2^h x 2^h operation-matrix
                            GenControlGate(gates[i][j].gate_, gates[i][j].control_qubit_-lowQubit, j-lowQubit, highQubit, matB);
                            // tmp_stage_matrix.push_back(matRes);
                            // save the operation-matrix of 2-input gates
                            Matrix_Multiplication(tmp_stage_matrix, matB, matRes);
                            tmp_stage_matrix.copy(matRes);
                        }
                    }
                }
            }
            // end of a level of gates
            // save the tensor product result of single qubit gates
            if (j != nQubits-1) { // if j == nQubits-1, it is a MERGE level, no stage matrix
                Matrix_Multiplication(tmp_stage_matrix, matA, matRes);
                tmp_stage_matrix.copy(matRes);
                // tmp_stage_matrix.push_back(matA);
            }
        }
        // cout << "RANK [" << myRank << "] FINISH THREAD" << endl;
    });

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
        // cout << "[DEBUG] step: " << i << endl;
        // encounter merge level
        if (gates[i][0].gate_name_ == "MERGE") {

            // MERGE gateParam1: subcircuit index starting from 0
            int subcId = gates[i][0].control_qubit_;

            // MERGE gateParam2: # of following MSWAP
            int nMSWAPs = gates[i][0].target_qubit_;

            if (subcId != 0) { // no operation-matrix before the first MERGE
                // get the high-order operation-matrix from subcircuit_matrix[subcId-1]
                sem_wait(&isGend);
                // MPI_Barrier(MPI_COMM_WORLD);
                // if (subcircuit_matrix.size() < (long unsigned int)subcId) {
                //     cout << "[ERROR] The operation-matrix is not ready yet. " << endl;
                //     exit(1);
                // }
                // commTime += MergeComputing(localV, localVLen, nWorkers, subcircuit_matrix[subcId - 1], myRank);
                commTime += MergeComputing(localV, localVLen, nWorkers, subcircuit_matrix, myRank);
                sem_post(&isUsed);
            }

            for (int j = 0; j < nMSWAPs; ++ j) { // communication time
                ++ i;
                commTime += MswapComputing(localV, localVLen, gates[i][0], lowQubit, myRank);
            }
        } else {
            // indexing on low-order qubits
            LocalComputing(localV, localVLen, gates[i], lowQubit, myRank);
        }
    }
    
    t1.join(); // blocking current process until thread ending

    // MPI_Barrier(MPI_COMM_WORLD);
    sem_destroy(&isGend);
    sem_destroy(&isUsed);
    double wholeEndTime = MPI_Wtime();
    wholeTime = wholeEndTime - wholeStartTime;

    // set timer for all workers ///////////////////////////////////
    double wholeTimer[nWorkers];
    memset(wholeTimer, 0, sizeof(double) * (nWorkers));
    double communicationTimer[nWorkers];
    memset(communicationTimer, 0, sizeof(double) * (nWorkers));

    double buffer[2];
    buffer[0] = wholeTime;
    buffer[1] = commTime;

    MPI_Status status;
    if (myRank == 0) {
        wholeTimer[0] = buffer[0];
        communicationTimer[0] = buffer[1];
        for (int j = 1; j < nWorkers; j++) {
            MPI_Recv(buffer, 2, MPI_DOUBLE, MPI_ANY_SOURCE, kLocalTaskDoneTag, MPI_COMM_WORLD, &status);
            wholeTimer[status.MPI_SOURCE] = buffer[0];
            communicationTimer[status.MPI_SOURCE] = buffer[1];
        }
    } else {
        MPI_Send(buffer, 2, MPI_DOUBLE, 0, kLocalTaskDoneTag, MPI_COMM_WORLD);
    }

    // send communication time and whole time to rank 0
    // double maxWholeTime, maxCommTime;
    // MPI_Reduce(&wholeTime, &maxWholeTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    // MPI_Reduce(&commTime, &maxCommTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (myRank == 0) {
        LogStartInfo("QuanTrans", fname);
        // LogTimeInfo(maxWholeTime - maxCommTime, maxCommTime, maxWholeTime);
        double minCommunicationTime = INFINITY;
        for (int i = 0; i < nWorkers; i++) {
            if (communicationTimer[i] < minCommunicationTime) {
                minCommunicationTime = communicationTimer[i];
                wholeTime = wholeTimer[i];
            }
        }
        LogTimeInfo(wholeTime - minCommunicationTime, minCommunicationTime, wholeTime);
    }

    // for (long long j = 0; j < localVLen; ++ j) {
    //     if (localV.data[j][0] > 0.0001) {
    //         cout << "myRank: " << myRank << " j: " << j << "  " << localV.data[j][0] << endl;
    //     }
    // }
    // if (myRank == 0)
    //     localV.print();

    MPI_Finalize();
    return 0;
}