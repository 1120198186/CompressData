#include <bits/stdc++.h>

#include "qcircuit.h"
#include "block.h"

#include "SVSim.h"
#include "HybridSVSim.h"

int main(int argc, char** argv) {
    if (argc != 4) {
        cout << "[ERROR] usage: cmd <numQubits> <memQubits> <numDepths>" << endl;
        exit(1);
    }

    srand((int)time(0));

    int numQubits = atoi(argv[1]); // the number of qubits of the circuit
    int memQubits = atoi(argv[2]); // the maximum number of qubits in the memory
    int numDepths = atoi(argv[3]); // the number of depths of the circuit

    // long long N = (1 << numQubits);
    // long long M = (1 << memQubits);

    // 
    // Generate a quantum circuit
    // 
    // Random /////
    QCircuit qc = QCircuit(numQubits);
    qc.setDepths(numDepths);
    qc.fill(0);

    //
    // Call different simulators
    //
    // Method 1: Local SVSim
    SVSim(qc);

    // TODO: Method 2: Block-based SVSim

    // Method 3: Hybrid SVSim
    HybridSVSim(qc, memQubits);

    // Method 4: Repeat Counter

    return 0;
}