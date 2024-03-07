//
// Generate gates from file
//     qHiPSTER and SingleQE use <Gate> to record a gate
//     MultiQE, QuanPath and QuanTrans use <MatrixImp> to record a gate
//

#include "matrix_wrap.h"

/**
 * @brief Used for qHiPSTER, SingleQE, where gate structures are record in <Gate>
 * 
 * @param gates save the generated circuit structure
 * @param fname filename
 */
void GenerateGatesFromFile(vector<vector<Gate>> &gates, string fname) {
    ifstream circuitFile(fname);

    int numQubits;

    string gateName;
    int gateParam1, gateParam2; // the control qubit, target qubit
    double gateParam3; // the gate parameter
    int qid;

    vector<Gate> level;

    if (circuitFile.is_open()) {
        circuitFile >> numQubits;

        while (true) {
            circuitFile >> gateName >> gateParam1 >> gateParam2;
            // cout << gateName << " " << gateParam1 << " " << gateParam2 << endl;

            if (gateName == "END") {
                // case 1: end of circuit
                // cout << "End of file. " << endl;
                break;
            } else {
                // case 2: a level of gates
                qid = 0;
                while (true) {
                    if (gateName == "IDE" or gateName == "X" or gateName == "H" or gateName == "Z") {
                        // single qubit gates
                        // level.push_back(MatrixImp(gateName));
                        // gateId, targetBit
                        level.push_back(Gate(gateName, gateParam2));
                    } else if (gateName == "RY") {
                        circuitFile >> gateParam3;
                        Matrix RY;
                        Rotation_Y(gateParam3, RY);
                        // gateId, controlBit, targetBit, matrix
                        level.push_back(Gate("RY", gateParam2, gateParam2, RY));
                    } else if (gateName == "CX") { // the target qubit of CX
                        // gateId, controlBit, targetBit, matrix
                        level.push_back(Gate("CX", gateParam1, gateParam2, X));
                    } else {
                        cout << "[ERROR] Gate undefined: " << gateName << endl;
                        exit(1);
                    }
                    if (qid == numQubits - 1) {
                        // end of a level of gates
                        break;
                    }
                    circuitFile >> gateName >> gateParam1 >> gateParam2;
                    qid ++;
                }
            }
            // add a level
            gates.push_back(std::move(level));
            level.clear();
        }
        circuitFile.close();
    } else {
        cout << "[ERROR] Cannot open the circuit file. " << endl;
        exit(1);
    }
}


/**
 * @brief Used for MultiQE, QuanPath and QuanTrans, where gate structures are record in <MatrixImp>
 * 
 * @param gates save the generated circuit structure
 * @param fname filename
 */
void GenerateMatrixImpsFromFile(vector<vector<MatrixImp>> &gates, string fname) {
    // cout << "Load the circuit from: " << fname << "." << endl;

    ifstream circuitFile(fname);

    int numQubits;

    string gateName;
    int gateParam1, gateParam2; // the control qubit, target qubit
    double gateParam3; // the gate parameter
    int qid;

    vector<MatrixImp> level;

    if (circuitFile.is_open()) {
        circuitFile >> numQubits;

        // cout << "numQubits: " << numQubits << endl;

        while (true) {
            circuitFile >> gateName >> gateParam1 >> gateParam2;
            // cout << gateName << " " << gateParam1 << " " << gateParam2 << endl;

            if (gateName == "MERGE"){
                // case 1: merge
                // cout << "=== merge ===" << endl;
                for (int i = 0; i < numQubits; ++ i) {
                    level.push_back(MatrixImp("MERGE", false, false, gateParam1, gateParam2));
                }
            } else if (gateName == "MSWAP") {
                // case 2: swap after merge
                // cout << "reorder qubits: " << gateParam1 << " " << gateParam2 << endl;
                for (int i = 0; i < numQubits; ++ i) {
                    level.push_back(MatrixImp("IDE"));
                }
                if (gateParam1 > gateParam2) // let ctrl qubit > targ qubit
                    level[0] = MatrixImp("MSWAP", true, true, gateParam1, gateParam2);
                else
                    level[0] = MatrixImp("MSWAP", true, true, gateParam2, gateParam1);
            } else if (gateName == "END") {
                // case 3: end of circuit
                // cout << "End of file. " << endl;
                break;
            } else {
                // case 4: a level of gates
                qid = 0;
                while (true) {
                    if (gateName == "IDE" or gateName == "X" or gateName == "H" or gateName == "Z") {
                        // single qubit gates
                        level.push_back(MatrixImp(gateName));
                    } else if (gateName == "RY") {
                        circuitFile >> gateParam3;
                        Matrix RY;
                        Rotation_Y(gateParam3, RY);
                        level.push_back(MatrixImp("RY", false, false, 0, 0, RY));
                    } else if (gateName == "CX") {
                        if (qid == gateParam1) { // the control qubit
                            level.push_back(MatrixImp("X", true, false, gateParam1, gateParam2));
                        } else { // the target qubit
                            level.push_back(MatrixImp("X", true, true, gateParam1, gateParam2));
                        }
                    } else if (gateName == "SWAP") {
                        level.push_back(MatrixImp("SWAP", true, true, gateParam1, gateParam2));
                    } else {
                        cout << "[ERROR] Gate undefined: " << gateName << endl;
                        exit(1);
                    }
                    if (qid == numQubits - 1) {
                        // end of a level of gates
                        break;
                    }
                    circuitFile >> gateName >> gateParam1 >> gateParam2;
                    qid ++;
                }
            }
            // add a level
            gates.push_back(std::move(level));
            level.clear();
        }
        circuitFile.close();
    } else {
        cout << "[ERROR] Cannot open the circuit file. " << endl;
        exit(1);
    }
}