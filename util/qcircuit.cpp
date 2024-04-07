#include "qcircuit.h"

/**
 * @brief Generate a random dense circuit
 * 
 * @param numQubits #Qubits
 * 
 * @return a quantum circuit
 */
QCircuit Random(int numQubits, int numDepths, double propt) {
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