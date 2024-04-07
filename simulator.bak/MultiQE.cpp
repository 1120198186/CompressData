#include <mpi.h>
#include <string>
#include <iostream>
#include <thread>
#include <math.h>

#include "global_notation.h"
#include "matrix_wrap.h"
#include "log.h"

using namespace std;

const int kLocalVTag = 1;
const int kLocalTaskDoneTag = 2;
const int kDistributionTag = 3;
const int kPeerTag = 4;
const int kGateDoneTag = 5;

void mutipleWithGateFor2x2(Matrix &g, double &a, double &b) {
    double temp0 = a*g.data[0][0] + b*g.data[0][1];
    double temp1 = a*g.data[1][0] + b*g.data[1][1];
    a = temp0;
    b = temp1;
}


void localComputing(Matrix &localV, long long localVLen,
        vector<MatrixImp> &GateSeq, int highQubit, int lowQubit, int myRank) {
    if ((localVLen & (localVLen - 1)) != 0) {
        cout << "[ERROR] the length of state vector is not a power of 2." << endl;
        exit(1);
    }
    bool *isAccessed = new bool[localVLen];
    int gate_num = GateSeq.size();

    for (int i = gate_num - 1; i >= highQubit; i--) { // for each lower gate
        int d = pow(2, (gate_num - 1) - i); // pair distance

        if (GateSeq[i].gate_name_ == "IDE") {
            continue;
        }

        Matrix &g = GateSeq[i].gate_;
        if (g.row != 2 || g.col != 2) {
            cout << "[ERROR] gate " << i << " is not 2x2 matrix" << endl;
            exit(1);
        }
        memset(isAccessed, 0, localVLen);

        for (long long j = 0; j < localVLen; j++) { // find pair in local state vector
            if (isAccessed[j]) continue;
            if (j + d >= localVLen) {
                cout << "[ERROR] localComputing exceed the length of state vector." << endl;
                exit(1);
            }

            isAccessed[j] = isAccessed[j+d] = true;
            if (GateSeq[i].is_control_gate_ && GateSeq[i].is_target_gate_) { // for control gate
                auto control_bit = GateSeq[i].control_qubit_;
                if (control_bit >= highQubit) {
                    long long mask = (1 << (gate_num - 1 - control_bit));
                    if ((j & mask) == 0) {
                        continue;
                    }
                } else {
                    long long mask = (1 << (gate_num - 1 - control_bit - lowQubit));
                    if ((myRank & mask) == 0) {
                        continue;
                    }
                 }
            }
            mutipleWithGateFor2x2(g, localV.data[j][0], localV.data[j + d][0]);
        }
        if (!GateSeq[i].is_control_gate_ && !GateSeq[i].is_target_gate_) {
            continue;
        } else if(GateSeq[i].is_control_gate_ && GateSeq[i].is_target_gate_) {
            i = GateSeq[i].control_qubit_ - 1;
        }
    }
    delete [] isAccessed;
}

void localComputing_exchanged(Matrix &localV, long long localVLen,
        vector<MatrixImp> &GateSeq, int highQubit, int lowQubit, int myRank, int nWorkers) {
    if ((localVLen & (localVLen - 1)) != 0) {
        cout << "[ERROR] the length of state vector is not a power of 2." << endl;
        exit(1);
    }
    bool *isAccessed = new bool[localVLen];
    // int gate_num = GateSeq.size();

    for (int i = highQubit - 1; i >= 0; i--) { // for each higher qubits after communication
        int d = pow(2, (highQubit - 1) - i);

        if (GateSeq[i].gate_name_ == "IDE") {
            continue;
        }

        Matrix &g = GateSeq[i].gate_;
        if (g.row != 2 || g.col != 2) {
            cout << "[ERROR] gate " << i << " is not 2x2 matrix" << endl;
            exit(1);
        }
        memset(isAccessed, 0, localVLen);

        for (long long j = 0; j < localVLen; j++) {
            if (isAccessed[j]) continue;
            if (j + d >= localVLen) {
                cout << "[ERROR] localComputing_exchange exceed the length of state vector." << endl;
                exit(1);
            }
            isAccessed[j] = isAccessed[j+d] = true;
            if (GateSeq[i].is_control_gate_ && GateSeq[i].is_target_gate_) {
                auto control_bit = GateSeq[i].control_qubit_;
                if (control_bit < 0 && control_bit >= highQubit) {
                    cout << "[ERROR] localComputing_exchange control gate illegal." << endl;
                    exit(1);
                } else {
                    long long mask = (1 << (highQubit - 1 - control_bit));
                    if (((j % nWorkers) & mask) == 0) {
                        continue;
                    }
                }
            } else if (GateSeq[i].is_control_gate_ && !GateSeq[i].is_target_gate_) {
                continue;
            }
            mutipleWithGateFor2x2(g, localV.data[j][0], localV.data[j + d][0]);
        }
        if (!GateSeq[i].is_control_gate_ && !GateSeq[i].is_target_gate_) {
            continue;
        } else if(GateSeq[i].is_control_gate_ && GateSeq[i].is_target_gate_) {
            i = GateSeq[i].control_qubit_ - 1;
        }
    }
    delete [] isAccessed;
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

    // int nWorkers = atoi(argv[1]); // #workers(process)
    // int nQubits = atoi(argv[2]); // #total qubits
    // int nGates = nQubits;
    // int nSteps = atoi(argv[3]); // #time step
    // int nSteps;

    int highQubit = log2(nWorkers); // #higher qubits
    int lowQubit = nQubits - highQubit; // #lower qubits
    // vector<vector<MatrixImp>> gates;
    // GenerateQFTGatesAllSteps(gates, nQubits); 
    // nSteps = gates.size();

    auto vlen = (long long) pow(2, nQubits); // total V len
    long long localVLen = vlen / nWorkers; // local V len

    if ((nWorkers & (nWorkers - 1)) != 0) {
        cout << "[ERROR] the number of workers is not a power of 2." << endl;
        exit(1);
    }

    // set timer for all workers
    double wholeTimer[nWorkers];
    memset(wholeTimer, 0, sizeof(double) * (nWorkers));
    double communicationTimer[nWorkers];
    memset(communicationTimer, 0, sizeof(double) * (nWorkers));
    
    // MPI initialization
    int myRank; // my process rank
    MPI_Init(nullptr, nullptr);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    // if (myRank == 0) {
    //     for (long unsigned int i = 0; i < gates.size(); ++ i) {
    //         for (long unsigned int j = 0; j < gates[i].size(); ++ j ) {
    //             cout << gates[i][j].gate_name_ << " ";
    //         }
    //         cout << endl;
    //     }
    //     cout << endl;
    // }

    // cout << "Start\n";
    
    double startTime = MPI_Wtime(); // whole time start
    
    Matrix localV = Matrix(localVLen, 1);
    for (long long j = 0; j < localVLen; j++) { // initialize state vector
        if (myRank == 0 && j == 0) {
            localV.data[j][0] = 1;
        } else {
            localV.data[j][0] = 0;
        }
    }

    long long group = localVLen / nWorkers; // #send data once
    auto *buffer = new double[group];

    // record time
    // double localtime_start = 0.0, localtime_end = 0.0;
    double commtime_start = 0.0, commtime_end = 0.0;
    // double exgtime_start = 0.0, exgtime_end = 0.0;

    // for each time step
    for (int i = 0; i < nSteps; i++) {
        // local computing for lower qubits
        localComputing(localV, localVLen, gates[i], highQubit, lowQubit, myRank);
        
        // if (i < int(nSteps-highQubit*(highQubit+1)/2)) {// non-communication step
        //     continue;
        // }
        // if no gate applied to high-order qubits
        bool needComm = false;
        for (int j = 0; j < highQubit; ++ j) {
            if (gates[i][j].gate_name_ != "IDE") {
                needComm = true;
                break;
            }
        }
        if (! needComm) {
            continue;
        }
        
        // communication among all nodes
        commtime_start += MPI_Wtime();
        memset(buffer, 0, sizeof(double) * group);
        // 一次性和其前面的node i交换数据，接收来自i的数据，发送给i自己的数据
        for (int j = 0; j < myRank; j++) {
            MPI_Recv(buffer, group, MPI_DOUBLE, j, kDistributionTag,
                MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for(long long k = 0; k < group; k++) {
                swap(buffer[k], localV.data[j + k * nWorkers][0]);
            }
            MPI_Send(buffer, group, MPI_DOUBLE, j, kDistributionTag, MPI_COMM_WORLD);
        }
        // 发送数据给后面的node j
        for (int j = myRank + 1; j < nWorkers; j++) {
            for(long long k = 0; k < group; k++) {
                buffer[k] = localV.data[j + k * nWorkers][0];
            }
            MPI_Send(buffer, group, MPI_DOUBLE, j, kDistributionTag, MPI_COMM_WORLD);
            MPI_Recv(buffer, group, MPI_DOUBLE, j, kDistributionTag, MPI_COMM_WORLD,
                    MPI_STATUS_IGNORE);
            for (long long k = 0; k < group; k++) {
                localV.data[j + k * nWorkers][0] = buffer[k];
            }
        }
        commtime_end += MPI_Wtime();

        // local computing after communication
        localComputing_exchanged(localV, localVLen, gates[i], highQubit, lowQubit, myRank, nWorkers);
    }

    delete [] buffer;

    MPI_Barrier(MPI_COMM_WORLD);
    double endTime = MPI_Wtime(); // whole end time
    double whole_time = endTime - startTime;
    double communication_time = commtime_end - commtime_start;
    
    // send computation time and buffer time to rank 0
    // double maxWholeTime, maxCommTime;
    // MPI_Reduce(&whole_time, &maxWholeTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    // MPI_Reduce(&communication_time, &maxCommTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    buffer = new double[2];
    buffer[0] = whole_time;
    buffer[1] = communication_time;

    MPI_Status status;
    if (myRank == 0) {
        wholeTimer[0] = buffer[0];
        communicationTimer[0] = buffer[1];
        for (int j = 1; j < nWorkers; j++) {
            MPI_Recv(buffer, 2, MPI_DOUBLE, MPI_ANY_SOURCE, kLocalTaskDoneTag, MPI_COMM_WORLD, &status);
            wholeTimer[status.MPI_SOURCE] = buffer[0];
            communicationTimer[status.MPI_SOURCE] = buffer[1];
        }
    }
    if (myRank != 0) {
        MPI_Send(buffer, 2, MPI_DOUBLE, 0, kLocalTaskDoneTag, MPI_COMM_WORLD);
    }

    // compare and output the whole time and computation time
    if (myRank == 0) {
        LogStartInfo("MultiQE", fname);
        // LogTimeInfo(maxWholeTime - maxCommTime, maxCommTime, maxWholeTime);
        double minCommunicationTime = INFINITY, wholeTime = 0.0;
        for (int i = 0; i < nWorkers; i++) {
            if (communicationTimer[i] < minCommunicationTime) {
                minCommunicationTime = communicationTimer[i];
                wholeTime = wholeTimer[i];
            }
        }
        LogTimeInfo(wholeTime - minCommunicationTime, minCommunicationTime, wholeTime);
    }

    delete [] buffer;

    MPI_Finalize();
    return 0;
}