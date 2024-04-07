#include "SVSim.h"

void SVSim(QCircuit &qc) {
    try {
        // Initialize the state vector
        long long N = (1 << qc.numQubits);
        Matrix localSv = Matrix(N, 1);
        localSv.data[0][0] = 1;

        for (int i = 0; i < qc.numDepths; ++ i) {
            // Conduct one level of indexing
            LocalComputing(localSv, N, qc.gates[i], qc.numQubits, 0);
        }
        // Write the state vector to the output file
        WriteBlock(localSv, 0, 1, "./output/svsim/");
    } catch (const bad_alloc& e) {
        cout << "[ERROR] Memory allocation failed: " << e.what() << endl;
    }
    return;
}