#include "qcircuit.h"
#include "timer.h"

#include "SVSim.h"
#include "BlockSim.h"
#include "HybridSim.h"
#include "DicCounter.h"

int main(int argc, char** argv) {
    if (argc < 4) {
        cout << "[ERROR] usage: cmd <numQubits> <memQubits> <numDepths>" << endl;
        exit(1);
    }

    srand((int)time(0));

    int numQubits = atoi(argv[1]); // the number of qubits of the circuit
    int memQubits = atoi(argv[2]); // the maximum number of qubits in the memory <= numQubits
    int numDepths = atoi(argv[3]); // the number of depths of the circuit
    int calBlocks = atoi(argv[4]); // the number of blocks to be calculated (for HybridSim)

    cout << "[INFO] numQubits: " << numQubits << " memQubits: " << memQubits << " numDepths: " << numDepths << " calBlocks: " << calBlocks << endl;
    cout << "[INFO] #Blocks for BlockSim:\t" << (1 << (numQubits - memQubits + 2)) << endl;
    cout << "[INFO] #Blocks for HybridSim:\t" << (1 << (numQubits - memQubits)) << endl;
    // 
    // Generate a quantum circuit
    // 
    // QCircuit qc = Grover(numQubits);
    // QCircuit qc = RandomRegular(numQubits, numDepths);
    // QCircuit qc = RandomMedium(numQubits, numDepths);
    QCircuit qc = RandomRandom(numQubits, numDepths);
    // QCircuit qc = test(numQubits, numDepths, memQubits);

    //
    // Call different simulators
    //
    // Method 1: Local SVSim
    // Timer timer;
    // double ioTime = 0.0, simTime = 0.0;
    // timer.Start();
    // ioTime = SVSim(qc) / 1e6;
    // timer.End();
    // simTime = timer.ElapsedTime() / 1e6;
    // cout << "[INFO] [SVSim] Sim time:\t" << simTime << " (sec);\tComp time: " << simTime - ioTime << " (sec)\n";

    // Method 2: BlockSim
    if (calBlocks == 0)
        BlockSim(qc, memQubits);

    // Method 3: HybridSim
    HybridSim(qc, memQubits, calBlocks);

    // TODO: Method 4: Repeat Counter

    // Method 5: Dictionary + Counter
    // DicCounter(qc, memQubits);

    return 0;
}