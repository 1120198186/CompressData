#include <mpi.h>
#include <string>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <queue>
#include <iostream>

#include "global_notation.h"
#include "matrix_wrap.h"
#include "log.h"

using namespace std;

const int klocalSvTag = 1;
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

void localComputing(Matrix &localSv, long long localSvLen, vector<Gate> gates, int numLocalGates) {
    if ((localSvLen & (localSvLen - 1)) != 0) {
        cout << "[ERROR] the length of state vector is not a power of 2." << endl;
        exit(1);
    }
    bool *isAccessed = new bool[localSvLen];

    for (auto &gate : gates) {
        if (gate.targetBit_ >= numLocalGates) break;
        int d = pow(2, gate.targetBit_);

        if (gate.gateId_ == "IDE") continue;

        Matrix &g = gate.m_;
        if (g.row != 2 || g.col != 2) {
            cout << "[ERROR] gate " << gate.targetBit_ << " is not 2x2 matrix." << endl;
            exit(1);
        }
        memset(isAccessed, 0, localSvLen);

        for (int j = 0; j < localSvLen; j++) {
            if (isAccessed[j]) continue;
            if (j + d >= localSvLen) {
                cout << "[ERROR] localComputing exceed the length of state vector." << endl;
                exit(1);
            }
            isAccessed[j] = isAccessed[j+d] = true;
            mutipleWithGateFor2x2(g, localSv.data[j][0], localSv.data[j + d][0]);
        }
    }
    delete [] isAccessed;
}

void distributionComputing(int rank, bool isControlGate, int mask, double a, double b, Matrix &firstV, Matrix &secondV, long long localSvLen, bool isFirst) {
    if (isFirst) {
        for (int i = 0, index = (rank - 1) * localSvLen; i < localSvLen; i++, index++) {
            if (isControlGate && ((index & mask) != mask)) continue;
            firstV.data[i][0] = a*firstV.data[i][0] + b*secondV.data[i][0];
        }
    } else {
        for (int i = 0, index = (rank - 1) * localSvLen; i < localSvLen; i++, index++) {
            if (isControlGate && ((index & mask) != mask)) continue; // TODO
            secondV.data[i][0] = a*firstV.data[i][0] + b*secondV.data[i][0];
        }
    }
}

void masterService(int numWorkers, int numQubits, int numGates, int numDistrGates, long long localSvLen, int numSteps, vector<vector<Gate>> gatesForAllSteps) {
    // cout << "--- qHiPSTER_with_VQC" << endl;
    // send local state vector to each worker
    auto *buffer = new double[localSvLen];
    memset(buffer, 0, sizeof(double) * localSvLen);
    buffer[0] = 1;
    for (int i = 1; i <= numWorkers; i++) {
        if (i != 1) {
            buffer[0] = 0;
        }
        MPI_Send(buffer, localSvLen, MPI_DOUBLE, i, klocalSvTag, MPI_COMM_WORLD);
    }
    delete [] buffer;

    // set timer for all workers
    double computingTimer[numWorkers + 1];
    memset(computingTimer, 0, sizeof(double) * (numWorkers+1));

    // steps
    for (int step = 0; step < numSteps; step++) {
        // receive worker's local computing task done
        MPI_Status status;
        double localComputingTime;
        for (int j = 1; j <= numWorkers; j++) {
            MPI_Recv(&localComputingTime, 1, MPI_DOUBLE, MPI_ANY_SOURCE, kLocalTaskDoneTag, MPI_COMM_WORLD, &status);
            computingTimer[status.MPI_SOURCE] += localComputingTime;
        }

        // starting distributed computing
        bool *isAccessed = new bool[numWorkers + 1];
        int numSolvedGates = numGates - numDistrGates;
        for (auto &gate : gatesForAllSteps[step]) {
            if (gate.targetBit_ < numSolvedGates) continue;
            if (gate.gateId_ == "IDE") continue; // skip IDE

            int d = pow(2, gate.targetBit_ - numSolvedGates);
            memset(isAccessed, 0, numWorkers + 1);

            for (int w = 1; w <= numWorkers; w++) {
                if (isAccessed[w]) continue;
                if (w + d > numWorkers) {
                    cout << "[ERROR] compute exceed the number of workers." << endl;
                    exit(1);
                }
                int peer = w + d;
                isAccessed[w] = isAccessed[peer] = true;
                MPI_Send(&w, 1, MPI_INT, peer, kDistributionTag, MPI_COMM_WORLD);
                MPI_Send(&peer, 1, MPI_INT, w, kDistributionTag, MPI_COMM_WORLD);
            }

            // receive distribution task done
            double distributionTime;
            for (int w = 1; w <= numWorkers; w++) {
                MPI_Recv(&distributionTime, 1, MPI_DOUBLE, w, kGateDoneTag, MPI_COMM_WORLD, &status);
                computingTimer[status.MPI_SOURCE] += distributionTime;
            }
        }
        delete [] isAccessed;
    }

    double maxComputingTime = 0;
    for (int i = 1; i <= numWorkers; i++) {
        if (computingTimer[i] > maxComputingTime) {
            maxComputingTime = computingTimer[i];
        }
    }
    cout << "\tcomputing time:  " << maxComputingTime << endl;
}

void workerService(int rank, int numGates, int numDistrGates, long long localSvLen, int numSteps, vector<vector<Gate>> gatesForAllSteps) {
    // worker receive local state vector from master
    auto *buffer = new double [localSvLen];
    MPI_Recv(buffer, localSvLen, MPI_DOUBLE, 0, klocalSvTag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    // start step computation
    Matrix localSv = Matrix(localSvLen, 1);
    Matrix peerV = Matrix(localSvLen, 1);
    for (long long j = 0; j < localSvLen; j++) {
        localSv.data[j][0] = buffer[j];
    }
    for (int step = 0; step < numSteps; step++) {
        // do local computation task
        double localComputingStartTime = MPI_Wtime();
        localComputing(localSv, localSvLen, gatesForAllSteps[step], numGates - numDistrGates);
        double localComputingEndTime = MPI_Wtime();
        double localComputingTime = localComputingEndTime - localComputingStartTime; // send local task done to master
        MPI_Send(&localComputingTime, 1, MPI_DOUBLE, 0, kLocalTaskDoneTag, MPI_COMM_WORLD);

        // starting distributed computing
        int numSolvedGates = numGates - numDistrGates; // gate number starts from 0.
        for (auto &gate : gatesForAllSteps[step]) {
            if (gate.targetBit_ < numSolvedGates) continue;
            if (gate.gateId_ == "IDE") continue; // skip IDE
            int peer;
            // receive distribution task from master
            MPI_Recv(&peer, 1, MPI_INT, 0, kDistributionTag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // if ((step % (numGates+1)) >= (numDistrGates + 2)) { // non-communication step
            //     double distributionTime = 0.0;
            //     MPI_Send(&distributionTime, 1, MPI_DOUBLE, 0, kGateDoneTag, MPI_COMM_WORLD);
            //     continue;
            // }

            if (rank < peer) {
                for (int k = 0; k < localSvLen; k++) buffer[k] = localSv.data[k][0];
                MPI_Send(buffer, localSvLen, MPI_DOUBLE, peer, kPeerTag, MPI_COMM_WORLD);
                MPI_Recv(buffer, localSvLen, MPI_DOUBLE, peer, kPeerTag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                for (int k = 0; k < localSvLen; k++) peerV.data[k][0] = buffer[k];
            } else {
                MPI_Recv(buffer, localSvLen, MPI_DOUBLE, peer, kPeerTag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                for (int k = 0; k < localSvLen; k++) peerV.data[k][0] = buffer[k];
                for (int k = 0; k < localSvLen; k++) buffer[k] = localSv.data[k][0];
                MPI_Send(buffer, localSvLen, MPI_DOUBLE, peer, kPeerTag, MPI_COMM_WORLD);
            }

            // compute after communication
            double distributionStartTime = MPI_Wtime();
            Matrix &g = gate.m_;
            bool isControlGate = gate.controlBit_ != gate.targetBit_;
            int mask = 1 << gate.controlBit_;
            for (auto& b : gate.extraControlBits_) mask |= 1 << b;
            if (rank < peer) {
                distributionComputing(rank, isControlGate, mask, g.data[0][0], g.data[0][1], localSv, peerV, localSvLen, true);
            } else {
                distributionComputing(rank, isControlGate, mask, g.data[1][0], g.data[1][1], peerV, localSv, localSvLen, false);
            }
            double distributionEndTime = MPI_Wtime();
            double distributionTime = distributionEndTime - distributionStartTime;
            MPI_Send(&distributionTime, 1, MPI_DOUBLE, 0, kGateDoneTag, MPI_COMM_WORLD);
        }
    }
    delete [] buffer;
}

int main(int argc, char **argv) {
    if (argc != 4) {
        cout << "[ERROR] usage: cmd <filename> <nWorkers> <outputFname>" << endl;
        exit(1);
    }
    string fname = argv[1];         // the circuit file name
    int numWorkers = atoi(argv[2]); // #workers
    freopen(argv[3], "a", stdout);

    // initialize gates
    vector<vector<Gate>> gatesForAllSteps;
    GenerateGatesFromFile(gatesForAllSteps, fname);
    // generateGatesOfAllStepsForVqcWithI(numQubits, gatesForAllSteps);

    int numQubits = gatesForAllSteps[0].size(); // number of qubits
    int numGates = numQubits; // number of gates
    int numSteps = gatesForAllSteps.size(); // number of steps

    // cout << endl << "nQubits: " << numQubits << " nSteps: " << numSteps << endl;
    
    // the length of state vector and localSv
    auto svLen = (long long) pow(2, numQubits);
    long long localSvLen = svLen / numWorkers;
    // the number of distributing matrix
    int numDistrGates = log2(numWorkers);

    if ((numWorkers & (numWorkers - 1)) != 0) {
        cout << "[ERROR] the number of workers is not a power of 2." << endl;
        exit(1);
    }

    // MPI initialization
    int myRank; // my process rank
    MPI_Init(nullptr, nullptr);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank); // MPI communication world (ordered), rank is the unique ID

    // if (myRank == 0) {
    //     for (long unsigned int i = 0; i < gatesForAllSteps.size(); ++ i) {
    //         for (long unsigned int j = 0; j < gatesForAllSteps[i].size(); ++ j ) {
    //             cout << gatesForAllSteps[i][j].gateId_ << " " << gatesForAllSteps[i][j].targetBit_ << endl;
    //             gatesForAllSteps[i][j].m_.print();
    //         }
    //         cout << endl;
    //     }
    //     cout << endl;
    // }

    if (myRank == 0) {
        LogStartInfo("qHiPSTER", fname);
        double startTime = MPI_Wtime();
        masterService(numWorkers, numQubits, numGates, numDistrGates, localSvLen, numSteps, gatesForAllSteps);
        double endTime = MPI_Wtime();
        cout << "\twhole time:  " << endTime - startTime << endl << endl;
    } else {
        // worker's rank range: 1 ~ numWorkers
        workerService(myRank, numGates, numDistrGates, localSvLen, numSteps, gatesForAllSteps);
    }

    MPI_Finalize();
    // cout << "Success" << endl;
    return 0;
}
