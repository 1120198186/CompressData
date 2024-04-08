#include "BlockSVSim.h"

void BlockSVSim(QCircuit &qc, int memQubits) {
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
    string dir = "./output/block/";
    InitStateVectorSSD(N, numFiles, dir);

    
}