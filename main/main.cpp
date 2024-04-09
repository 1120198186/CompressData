#include <bits/stdc++.h>

#include "qcircuit.h"
#include "block.h"
#include "timer.h"

#include "SVSim.h"
#include "BlockSVSim.h"
#include "HybridSVSim.h"

int main(int argc, char** argv) {
    if (argc != 4) {
        cout << "[ERROR] usage: cmd <numQubits> <memQubits> <numDepths>" << endl;
        exit(1);
    }

    srand((int)time(0));

    int numQubits = atoi(argv[1]); // the number of qubits of the circuit
    int memQubits = atoi(argv[2]); // the maximum number of qubits in the memory <= numQubits
    int numDepths = atoi(argv[3]); // the number of depths of the circuit

    // long long N = (1 << numQubits);
    // long long M = (1 << memQubits);

    // 
    // Generate a quantum circuit
    // 
    // Random /////
    // QCircuit qc = RandomRegular(numQubits, numDepths);
    QCircuit qc = RandomRandom(numQubits, numDepths);

    Timer timer;

    //
    // Call different simulators
    //
    // Method 1: Local SVSim
    timer.Start();
    SVSim(qc);
    timer.End();
    timer.ElapsedTime();

    // TODO: Method 2: Block-based SVSim
    timer.Start();
    BlockSVSim(qc, memQubits);
    timer.End();
    timer.ElapsedTime();

    // Method 3: Hybrid SVSim
    timer.Start();
    HybridSVSim(qc, memQubits);
    timer.End();
    timer.ElapsedTime();

    // TODO: Method 4: Repeat Counter

    // TODO: Method 5: Dictionary + Counter

    return 0;
}