// 
// Generate quantum circuits
// 

#include <bits/stdc++.h>
#include "matrix_wrap.h"

using namespace std;

#define random(x) (rand()%x)

class QCircuit {
public:
    int numQubits;
    int numDepths;
    vector<vector<MatrixImp>> gates;
    // Matrix sv = Matrix(1 << numQubits, 1);

    QCircuit() {};

    /**
     * @brief Construct an n-qubit T-level quantum circuit object
     * 
     * @param numQubits_ #Qubits
     * @param numDepths_ #Depths
     */
    QCircuit(int numQubits_): numQubits(numQubits_){
        // sv.data[0][0] = 1;
        numDepths = 0;
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
                    level = random(4);
                    if (level == 0) 
                        gates[j][i] = MatrixImp("H");
                    else if (level == 1) 
                        gates[j][i] = MatrixImp("Z");
                    else if (level == 2)
                        gates[j][i] = MatrixImp("X");
                    else {
                        Matrix RY;
                        Rotation_Y(random(10), RY);
                        gates[j][i] = MatrixImp("RY", false, false, 0, 0, RY);
                    }
                }
            }
        }

        // this->print();
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
 * @brief Generate a random dense circuit
 * 
 * @param numQubits #Qubits
 * 
 * @return a quantum circuit
 */
QCircuit Random(int numQubits, int numDepths, double propt=0.1) {
    QCircuit qc = QCircuit(numQubits);
    qc.setDepths(numDepths);

    int numSwaps = round(propt * numDepths);
    // cout << "[DEBUG] numSwaps: " << numSwaps << endl;
    int level, ctrl, targ;
    bool ret;

    for (int i = 0; i < numSwaps; ++ i) {
        level = random(numDepths);
        ctrl = random(numQubits);
        targ = random(numQubits);

        ret = qc.addSwap(level, ctrl, targ);
        while (! ret) {
            level = random(numDepths);
            ctrl = random(numQubits);
            targ = random(numQubits);
            ret = qc.addSwap(level, ctrl, targ);
        }
    }
    
    qc.print();

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


QCircuit QFT(int numQubits) {
    QCircuit qc = QCircuit(numQubits);

    int ii = 0;
    int jj = numQubits - 1;
    while (ii < jj) {
        qc.swap(ii, jj);
        ii ++;
        jj --;
    }

    qc.h(0);
    qc.cx(1, 0);
    qc.barrier();

    qc.h(1);
    qc.cx(2, 0);
    qc.cx(2, 1);
    qc.barrier();

    int n = numQubits - 1;
    int i = 2; // H gate
    while (i < n) {
        qc.h(i);
        int t = i;
        int tt = 0;

        while (t >= 2) {
            qc.cx(i+1, tt);
            tt += 1;
            t -= 1;
        }
        qc.cx(i+1, i-1);
        qc.cx(i+1, i);
        i += 1;
        qc.barrier();
    }
    qc.h(n);

    // qc.print();

    return qc;
}

/**
 * @brief Generate VQC
 * 
 * @param numQubits #Qubits
 * 
 * @return a quantum circuit
 */
QCircuit VQC(int numQubits) {
    QCircuit qc = QCircuit(numQubits);

    for (int j = 0; j < 20; ++ j) {
        // 2 levels of RY
        for (int k = 0; k < 2; ++ k)
            for (int i = 0; i < numQubits; ++ i)
                qc.ry(0.8, i);
        // levels of CX
        for (int i = 0; i < numQubits-1; ++ i) {
            qc.cx(i, i+1);
            qc.barrier();
        }
        // 2 levels of RY
        for (int k = 0; k < 2; ++ k)
            for (int i = 0; i < numQubits; ++ i)
                qc.ry(0.8, i);
    }

    qc.printInfo();
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


QCircuit test() {
    int numQubits = 6;
    QCircuit qc = QCircuit(numQubits);

    for (int k = 0; k < 2; ++ k)
        for (int i = 0; i < numQubits; ++ i)
            qc.ry(2 * M_PI, i);

    qc.swap(0, 1);
    qc.swap(2, 3);
    qc.swap(4, 5);
    
    qc.swap(0, 1);
    qc.swap(2, 3);
    qc.swap(4, 5);

    for (int k = 0; k < 2; ++ k)
        for (int i = 0; i < numQubits; ++ i)
            qc.ry(2 * M_PI, i);

    return qc;
}