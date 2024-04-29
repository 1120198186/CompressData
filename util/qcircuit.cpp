#include "qcircuit.h"

QCircuit RandomRegular(int numQubits, int numDepths) {
    QCircuit qc = QCircuit(numQubits);
    int gTyp;

    while (true) {
        // add two levels of CX gates
        if (qc.numDepths % 10 == 2) {
            for (int j = numQubits - 1; j > 0; j -= 2) {
                qc.cx(j, j-1);
            }
            for (int j = numQubits - 2; j > 0; j -= 2) {
                qc.cx(j, j-1);
            }
        }
        // add one level of single-qubit gates
        else {
            // random single-qubit gates
            for (int j = 0; j < numQubits; ++ j) {
                gTyp = random(3);
                if (gTyp == 0) {
                    qc.h(j);
                }
                else if (gTyp == 1) {
                    qc.z(j);
                }
                else {
                    qc.x(j);
                }
            }
        }

        if (qc.numDepths >= numDepths) {
            break;
        }
    }

    qc.print();
    return qc;
}


QCircuit RandomMedium(int numQubits, int numDepths) {
    QCircuit qc = QCircuit(numQubits);
    int gTyp;

    while (true) {
        // add two levels of CX gates
        if (qc.numDepths % 10 == 2) {
            for (int j = numQubits - 1; j > 0; j -= 2) {
                qc.cx(j, j-1);
            }
            for (int j = numQubits - 2; j > 0; j -= 2) {
                qc.cx(j, j-1);
            }
        }
        // add one level of single-qubit gates
        else {
            // random single-qubit gates
            for (int j = 0; j < numQubits; ++ j) {
                gTyp = random(4);
                if (gTyp == 0) {
                    qc.h(j);
                }
                else if (gTyp == 1) {
                    qc.z(j);
                }
                else if (gTyp == 2) {
                    qc.x(j);
                } else {
                    qc.ry((double)rand() / RAND_MAX * 2 * acos(-1.0), j);
                }
            }
        }

        if (qc.numDepths >= numDepths) {
            break;
        }
    }

    qc.print();
    return qc;
}


QCircuit RandomRandom(int numQubits, int numDepths) {
    QCircuit qc = QCircuit(numQubits);

    int numGates = 0;

    while (true) {
        // add one level of CX gates
        if (qc.numDepths % 10 == 2) {
            for (int j = numQubits - 1; j > 0; j -= 2) {
                qc.cx(j, j-1);
                ++ numGates;
            }
            // for (int j = numQubits - 2; j > 0; j -= 2) {
            //     qc.cx(j, j-1);
            // }
        }
        // add one level of single-qubit gates
        else {
            // random single-qubit gates
            for (int j = 0; j < numQubits; ++ j) {
                qc.ry((double)rand() / RAND_MAX * 2 * acos(-1.0), j);
                ++ numGates;
            }
        }

        if (qc.numDepths >= numDepths) {
            break;
        }
    }

    // qc.print();

    cout << "[INFO] [RandomRandom] #Qubits: [" << qc.numQubits << "] #Gates: [" << numGates << "]" << endl;
    return qc;
}

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
        qc.barrier();
    }

    // int numIterations = static_cast<int>(round(std::acos(-1.0) / 4 * sqrt(pow(2, numQubits))));
    int numIterations = 10;

    for (int itr = 0; itr < numIterations; ++ itr) {
        qc.phaseInv((1 << numQubits) - 1);
        qc.barrier();
        for (int i = 0; i < numQubits; ++ i) {
            qc.h(i);
            qc.barrier();
        }
        for (int i = 0; i < numQubits; ++ i) {
            qc.x(i);
            qc.barrier();
        }
        qc.phaseInv((1 << numQubits) - 1);
        qc.barrier();
        for (int i = 0; i < numQubits; ++ i) {
            qc.x(i);
            qc.barrier();
        }
        // qc.ry(2 * std::acos(-1.0), numQubits-1);
        for (int i = 0; i < numQubits; ++ i) {
            qc.h(i);
            qc.barrier();
        }
    }

    qc.print();
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
            qc.ry(2 * std::acos(-1.0), i);
    
    for (int i = 0; i < numQubits; ++ i) {
        for (int k = 0; k < 5; ++ k)
            for (int ii = 0; ii < numQubits; ++ ii)
                qc.ry(2 * std::acos(-1.0), ii);
        
        for (int j = 0; j < numQubits; ++ j) {
            if (i == j)
                continue;
            qc.barrier();
            qc.cx(i, j);
        }
    }

    for (int k = 0; k < 2; ++ k)
        for (int i = 0; i < numQubits; ++ i)
            qc.ry(2 * std::acos(-1.0), i);

    return qc;
}


QCircuit test(int numQubits, int numDepths, int memQubits) {
    QCircuit qc = QCircuit(numQubits);

    // for (int i = 0; i < 2; ++ i) {
    //     for (int j = 0; j < numQubits; ++ j) {
    //         qc.ry(0.1 * j, j);
    //     }
    // }
    for (int j = 0; j < numQubits; ++ j) {
        qc.h(j);
        qc.z(j);
    }

    for (int i = 0; i < 1; ++ i) {
        // for (int k = 0; k < 5; ++ k) {
        //     for (int j = 0; j < numQubits; ++ j) {
        //         qc.ry(0.1 * j, j);
        //     }
        // }
        for (int j = 0; j < numQubits; ++ j) {
            if (i == j)
                continue;
            qc.cx(i, j);
            qc.barrier();
        }
    }

    for (int j = numQubits - 1; j >= 0; -- j) {
        qc.ry((double)rand() / RAND_MAX * 2 * acos(-1.0), j);
    }
    // for (int i = 0; i < 2; ++ i) {
    //     for (int j = 0; j < numQubits; ++ j) {
    //         qc.ry(0.1 * j, j);
    //     }
    // }
    // for (int j = 0; j < numQubits; ++ j) {
    //     qc.h(j);
    //     qc.z(j);
    // }
    qc.print();
    return qc;
}