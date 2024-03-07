// 
// Generate quantum circuits
// 

#include <bits/stdc++.h>
#include "matrix_wrap.h"

using namespace std;

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
        add_level();
    }

    // 
    // Single-qubit gates
    // TODO: Other single-qubit gates: Z, RX, RY, ...
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
        gates[numDepths-1][targ] = MatrixImp("SWAP", true, true, ctrl, targ);
    }

    bool addSwap(int level, int ctrl, int targ) {
        if (ctrl == targ or gates[level][ctrl].gate_name_ != "IDE" or gates[level][targ].gate_name_ != "IDE") {
            return false;
        }
        gates[level][ctrl] = MatrixImp("SWAP", true, true, ctrl, targ);
        gates[level][targ] = MatrixImp("SWAP", true, true, ctrl, targ);
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

    /**
     * @brief Print the structure of the quantum circuit
     */
    void print() {
        cout << "numQubits: " << numQubits << " numDepths: " << numDepths << endl;
        for (int i = 0; i < numQubits; ++ i) {
            cout << "q[" << i << "]\t";
            for (int j = 0; j < numDepths; ++ j) {
                cout << gates[j][i].gate_name_ << "\t"; 
            }
            cout << endl;
        }
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
 * @brief Generate a random dense circuit
 * 
 * @param numQubits #Qubits
 * 
 * @return a quantum circuit
 */
QCircuit random(int numQubits, int numDepths, double propt=0.1) {
    QCircuit qc = QCircuit(numQubits);
    qc.setDepths(numDepths);

    int numSwaps = round(propt * numDepths);
    cout << "[DEBUG] numSwaps: " << numSwaps << endl;
    int level, ctrl, targ;
    bool ret;

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> levelDist(0, numDepths - 1);
    uniform_int_distribution<int> qubitDist(0, numQubits - 1);

    for (int i = 0; i < numSwaps; ++ i) {
        level = levelDist(gen);
        ctrl = qubitDist(gen);
        targ = qubitDist(gen);

        // cout << "[DEBUG] level, ctrl, targ: " << level << ", " << ctrl << ", " << targ << endl;

        ret = qc.addSwap(level, ctrl, targ);
        while (! ret) {
            level = levelDist(gen);
            ctrl = qubitDist(gen);
            targ = qubitDist(gen);
            // cout << "[DEBUG] level, ctrl, targ: " << level << ", " << ctrl << ", " << targ << endl;
            ret = qc.addSwap(level, ctrl, targ);
        }
    }

    return qc;
}

/**
 * @brief Generate a Grover's circuit
 * 
 * @param numQubits #Qubits
 * 
 * @return a quantum circuit
 */
QCircuit Grover(int numQubits) {
    QCircuit qc = QCircuit(numQubits);

    for (int i = 0; i < numQubits; ++ i) {
        qc.h(i);
    }

    int numIterations = static_cast<int>(round(M_PI / 4 * sqrt(pow(2, numQubits))));

    for (int itr = 0; itr < numIterations; ++ itr) {
        qc.z(0);
        for (int i = 0; i < numQubits; ++ i) {
            qc.h(i);
        }
        for (int i = 0; i < numQubits; ++ i) {
            qc.x(i);
        }
        qc.z(0);
        for (int i = 0; i < numQubits; ++ i) {
            qc.x(i);
        }
        qc.ry(2 * M_PI, numQubits-1);
        for (int i = 0; i < numQubits; ++ i) {
            qc.h(i);
        }
    }
    return qc;
}

/**
 * @brief Generate VQC1 (a small number of inseparable levels)
 * 
 * @param numQubits #Qubits
 * 
 * @return a quantum circuit
 */
QCircuit VQC1(int numQubits) {
    QCircuit qc = QCircuit(numQubits);

    for (int j = 0; j < 8; ++ j) {
        for (int k = 0; k < 2; ++ k)
            for (int i = 0; i < numQubits; ++ i)
                qc.h(i);
        for (int i = 0; i < numQubits-1; ++ i)
            qc.cx(i, i+1);
        for (int k = 0; k < 2; ++ k)
            for (int i = 0; i < numQubits; ++ i)
                qc.h(i);
    }

    for (int j = 0; j < 8; ++ j) {
        for (int k = 0; k < 2; ++ k)
            for (int i = 0; i < numQubits; ++ i)
                qc.h(i);
        for (int i = 1; i < numQubits; ++ i)
            qc.cx(i, i-1);
        for (int k = 0; k < 2; ++ k)
            for (int i = 0; i < numQubits; ++ i)
                qc.h(i);
    }

    for (int j = 0; j < 8; ++ j) {
        for (int k = 0; k < 2; ++ k)
            for (int i = 0; i < numQubits; ++ i)
                qc.h(i);
        for (int i = 0; i < numQubits-1; ++ i)
            qc.cx(i, i+1);
        for (int k = 0; k < 2; ++ k)
            for (int i = 0; i < numQubits; ++ i)
                qc.h(i);
    }

    return qc;
}

/**
 * @brief Generate VQC2 (a larger number of inseparable levels)
 * 
 * @param numQubits #Qubits
 * 
 * @return a quantum circuit
 */
QCircuit VQC2(int numQubits) {
    QCircuit qc = QCircuit(numQubits);

    for (int k = 0; k < 2; ++ k)
        for (int i = 0; i < numQubits; ++ i)
            qc.ry(2 * M_PI, i);
    
    for (int i = 0; i < numQubits; ++ i) {
        for (int k = 0; k < 5; ++ k)
            for (int ii = 0; ii < numQubits; ++ ii)
                qc.ry(2 * M_PI, ii);
        
        for (int j = 0; j < numQubits; ++ j) {
            if (i == j)
                continue;
            qc.barrier();
            qc.cx(i, j);
        }
    }

    for (int k = 0; k < 2; ++ k)
        for (int i = 0; i < numQubits; ++ i)
            qc.ry(2 * M_PI, i);

    return qc;
}
