#include "indexing.h"
#include "qcircuit.h"

int main(int argc, char** argv) {
    // 
    // Generate a quantum circuit
    // 
    if (argc != 3) {
        cout << "[ERROR] usage: cmd <numQubits> <numDepths>" << endl;
        exit(1);
    }

    srand((int)time(0));

    // double sum = 0.0;
    int numQubits = atoi(argv[1]); 
    int numDepths = atoi(argv[2]);
    int h = 0; // H = 2^h := the number of blocks
    // int epoch = atoi(argv[4]);

    // Random /////
    QCircuit qc = QCircuit(numQubits);
    qc.setDepths(numDepths);
    qc.fill(0);

    // VQC /////
    // QCircuit qc = VQC(numQubits);

    // QFT /////
    // QCircuit qc = QFT(numQubits);

    //
    // Initialize the state vector
    //
    Matrix localSv = Matrix(1 << (numQubits-h), 1);

    // TODO 1: block-based-BDD + indexing //////////////////////
    // Start compressing this localSv
    // Replace the code here with compressed initial state
    // Add necessary compression code in ../util/compress.h
    localSv.data[0][0] = 1;
    //
    // TODO 2: block-based indexing without compression ////////
    // Write H-1 blocks to files, i.e., SSD
    // Leave it for xls
    //

    // 
    // Indexing simulations
    // 
    auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < qc.numDepths; ++ i) {
        // LocalComputing does one level of indexing for the localSv of size 2^(n-h)
        // TODO: 这里不涉及到多块参与的计算，多块参与的indexing需要另外实现
        LocalComputing(localSv, 1 << numQubits, qc.gates[i], numQubits - h, 0);
    }
    auto end = chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    // sv.print();
    for (int i = 0; i < 10; ++ i) {
        cout << localSv.data[i][0] << " ";
    }
    cout << endl;

    cout << "[INFO] Indexing time: " << duration << " (ms)" << endl;

    return 0;
}