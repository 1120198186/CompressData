// 
// Generate quantum circuits
// 

#ifndef QCIRCUIT_H
#define QCIRCUIT_H

#include <bits/stdc++.h>
#include "matrix_wrap.h"

using namespace std;

#define random(x) (rand()%x)

class QCircuit {
public:
    int numQubits;
    int numDepths;
    vector<vector<MatrixImp>> gates;

    QCircuit() {};

    /**
     * @brief Construct an n-qubit T-level quantum circuit object
     * 
     * @param numQubits_ #Qubits
     * @param numDepths_ #Depths
     */
    QCircuit(int numQubits_): numQubits(numQubits_){
        numDepths = 0;
        add_level();
    }

    // 
    // Single-qubit gates
    // TODO: Other single-qubit gates: RX, RY, ...
    // 

    /**
     * @brief Apply an X gate to qubit[qid]
     * 
     * @param qid   qubit id
     */
    void x(int qid) {
        if (gates[numDepths-1][qid].gate_name_ != "IDE") {
            add_level();
        }
        gates[numDepths-1][qid] = MatrixImp("X");
    }

    /**
     * @brief Apply an H gate to qubit[qid]
     * 
     * @param qid   qubit id
     */
    void h(int qid) {
        if (gates[numDepths-1][qid].gate_name_ != "IDE") {
            add_level();
        }
        gates[numDepths-1][qid] = MatrixImp("H");
    }

    /**
     * @brief Apply a Z gate to qubit[qid]
     * 
     * @param qid   qubit id
     */
    void z(int qid) {
        if (gates[numDepths-1][qid].gate_name_ != "IDE") {
            add_level();
        }
        gates[numDepths-1][qid] = MatrixImp("Z");
    }

    /**
     * @brief Apply a RY gate to qubit[qid]
     * 
     * @param param the gate parameter
     * @param qid   qubit id
     */
    void ry(double theta, int qid) {
        if (gates[numDepths-1][qid].gate_name_ != "IDE") {
            add_level();
        }
        Matrix RY;
        Rotation_Y(theta, RY);
        gates[numDepths-1][qid] = MatrixImp("RY", false, false, 0, 0, RY);
    }

    // 
    // 2-qubit controlled gates
    // TODO: Other 2-qubit controlled gates: CY, CZ, CU, ...
    // 

    /**
     * @brief Apply a CX gate to qubit[ctrl] -> qubit[targ]
     * 
     * @param ctrl  control qubit id
     * @param targ  target qubit id
     */
    void cx(int ctrl, int targ) {
        if (gates[numDepths-1][ctrl].gate_name_ != "IDE" or gates[numDepths-1][targ].gate_name_ != "IDE") {
            add_level();
        }
        gates[numDepths-1][ctrl] = MatrixImp("X", true, false, ctrl, targ);
        gates[numDepths-1][targ] = MatrixImp("X", false, true, ctrl, targ);
    }

    //
    // 2-qubit non-controlled gates
    // <e.g.> SWAP gates
    //

    /**
     * @brief Apply a SWAP gate to qubit[ctrl] <-> qubit[targ]
     * 
     * @param ctrl  swap input qubit 1
     * @param targ  swap input qubit 2
     */
    void swap(int ctrl, int targ) {
        if (gates[numDepths-1][ctrl].gate_name_ != "IDE" or gates[numDepths-1][targ].gate_name_ != "IDE") {
            add_level();
        }
        gates[numDepths-1][ctrl] = MatrixImp("SWAP", true, true, ctrl, targ);
        gates[numDepths-1][targ] = MatrixImp("SWAP", true, true, targ, ctrl);
    }

    bool addSwap(int level, int ctrl, int targ) {
        if (ctrl == targ or gates[level][ctrl].gate_name_ != "IDE" or gates[level][targ].gate_name_ != "IDE") {
            // cout << "[DEBUG] add swap failed. " << endl;
            return false;
        }
        // cout << "[DEBUG] add swap success. " << endl;
        gates[level][ctrl] = MatrixImp("SWAP", true, true, ctrl, targ);
        gates[level][targ] = MatrixImp("SWAP", true, true, targ, ctrl);
        return true;
    }

    /**
     * @brief Set the circuit depth to numDepths_
     * 
     * @param numDepths_  the target circuit depth
     */
    void setDepths(int numDepths_) {
        int range = numDepths_ - numDepths;
        for (int i = 0; i < range; ++ i){
            add_level();
        }
    }

    //
    // Other operations on quantum circuits
    //

    /**
     * @brief Add a barrier to the quantum circuit
     */
    void barrier() {
        add_level();
    }

    void fill(double propt = 0.1) {
        // clear
        for (int j = 0; j < numDepths; ++ j) {
            for (int i = 0; i < numQubits; ++ i) {
                gates[j][i] = MatrixImp("IDE");
            }
        }
        
        // random swap gates
        int numSwaps = round(propt * numDepths);
        
        int level, ctrl, targ;
        bool ret;

        for (int i = 0; i < numSwaps; ++ i) {
            level = random(numDepths);
            ctrl = random(numQubits);
            targ = random(numQubits);

            ret = addSwap(level, ctrl, targ);
            while (! ret) {
                level = random(numDepths);
                ctrl = random(numQubits);
                targ = random(numQubits);
                ret = addSwap(level, ctrl, targ);
            }
        }

        for (int j = 0; j < numDepths; ++ j) {
            for (int i = 0; i < numQubits; ++ i) {
                if (gates[j][i].gate_name_ == "IDE") {
                    level = random(3);
                    if (level == 0) 
                        gates[j][i] = MatrixImp("H");
                    else if (level == 1) 
                        gates[j][i] = MatrixImp("Z");
                    else if (level == 2)
                        gates[j][i] = MatrixImp("X");
                    // else {
                    //     Matrix RY;
                    //     Rotation_Y(random(10), RY);
                    //     gates[j][i] = MatrixImp("RY", false, false, 0, 0, RY);
                    // }
                }
            }
        }

        this->print();
    }

    /**
     * @brief Print the structure of the quantum circuit
     */
    void print() {
        cout << "numQubits: " << numQubits << " numDepths: " << numDepths << endl;
        int start = 0;
        if (numQubits > 5) {
            start = numQubits - 5;
        }
        for (int i = numQubits - 1; i >= start; -- i) {
        // for (int i = start; i < numQubits; ++ i) {
            // if (i > 5) {
            //     cout << "..." << endl;
            //     break;
            // }
            cout << "q[" << i << "]\t";
            for (int j = 0; j < numDepths; ++ j) {
                if (j > 10) {
                    cout << "...";
                    break;
                }
                if (gates[j][i].is_control_gate_) {
                    cout << "C";
                } else if (gates[j][i].is_target_gate_) {
                    cout << "T";
                }
                cout << gates[j][i].gate_name_ << "\t"; 
            }
            cout << endl;
        }
    }

    /**
     * @brief Print the information of the quantum circuit
     */
    void printInfo() {
        cout << "numQubits: " << numQubits << " numDepths: " << numDepths << endl;
    }

private:
    /**
     * @brief Add a new level full of IDE gates to the circuit
     */
    void add_level() {
        vector<MatrixImp> level;
        for (int i = 0; i < numQubits; ++ i) {
            level.push_back(MatrixImp("IDE"));
        }
        gates.push_back(level);
        numDepths ++;
    }
};


/**
 * @brief Random a circuit with regular state vector using H, Z, X, CX gates
 * 
 * @param numQubits 
 * @param numDepths
 * @return QCircuit 
 */
QCircuit RandomRegular(int numQubits, int numDepths);


/**
 * @brief Random a circuit with relatively random state vector using H, Z, X, RY, CX gates
 * 
 * @param numQubits 
 * @param numDepths 
 * @return QCircuit 
 */
QCircuit RandomMedium(int numQubits, int numDepths);


/**
 * @brief Random a circuit with random state vector using RY, CX gates
 * 
 * @param numQubits 
 * @param numDepths 
 * @return QCircuit 
 */
QCircuit RandomRandom(int numQubits, int numDepths);


/**
 * @brief Generate a random dense circuit
 * 
 * @param numQubits #Qubits
 * 
 * @return a quantum circuit
 */
QCircuit Random(int numQubits, int numDepths, double propt=0.1);


/**
 * @brief Generate a Grover's circuit
 * 
 * @param numQubits #Qubits
 * 
 * @return a quantum circuit
 */
QCircuit Grover(int numQubits);


QCircuit QFT(int numQubits);


/**
 * @brief Generate VQC
 * 
 * @param numQubits #Qubits
 * 
 * @return a quantum circuit
 */
QCircuit VQC(int numQubits);


/**
 * @brief Generate VQC1 (a small number of inseparable levels)
 * 
 * @param numQubits #Qubits
 * 
 * @return a quantum circuit
 */
QCircuit VQC1(int numQubits);


/**
 * @brief Generate VQC2 (a larger number of inseparable levels)
 * 
 * @param numQubits #Qubits
 * 
 * @return a quantum circuit
 */
QCircuit VQC2(int numQubits);


QCircuit test(int numQubits, int numDepths, int memQubits);

#endif // QCIRCUIT_H