import random
from math import pi
from qiskit import QuantumCircuit

def RandomRegular(numQubits, numGates):
    ''' Generate a random regular circuit using H, Z, X, and CX with a given number of qubits and gates '''
    qc = QuantumCircuit(numQubits)
    svCnt = 0

    while svCnt <= numGates:
        for i in range(3):
            for j in range(numQubits):
                gTyp = random.randint(0, 2)
                if gTyp == 0:
                    qc.h(j)
                elif gTyp == 1:
                    qc.z(j)
                else:
                    qc.x(j)
                qc.save_statevector(label = str(svCnt))
                svCnt += 1
        for j in range(numQubits-1, 0, -2):
            qc.cx(j, j-1)
            qc.save_statevector(label = str(svCnt))
            svCnt += 1
        for j in range(numQubits-2, 0, -2):
            qc.cx(j, j-1)
            qc.save_statevector(label = str(svCnt))
            svCnt += 1

    print(f'[RandomRegular] #Qubits: [{numQubits}] #Depths: [{qc.depth()}] #Saved Statevectors: [{svCnt}]')

    return qc

def RandomMedium(numQubits, numGates):
    ''' Generate a random regular circuit using H, Z, X, RY, and CX with a given number of qubits and gates '''
    qc = QuantumCircuit(numQubits)
    svCnt = 0
    
    while svCnt <= numGates:
        for i in range(3):
            for j in range(numQubits):
                gTyp = random.randint(0, 3)
                if gTyp == 0:
                    qc.h(j)
                elif gTyp == 1:
                    qc.z(j)
                elif gTyp == 2:
                    qc.x(j)
                else:
                    qc.ry(random.uniform(0, 2 * pi), j)
                qc.save_statevector(label = str(svCnt))
                svCnt += 1
        for j in range(numQubits-1, 0, -2):
            qc.cx(j, j-1)
            qc.save_statevector(label = str(svCnt))
            svCnt += 1
        for j in range(numQubits-2, 0, -2):
            qc.cx(j, j-1)
            qc.save_statevector(label = str(svCnt))
            svCnt += 1

    print(f'[RandomMedium] #Qubits: [{numQubits}] #Depths: [{qc.depth()}] #Saved Statevectors: [{svCnt}]')
    
    return qc

def RandomRandom(numQubits, numGates):
    ''' Generate a random regular circuit using RY and CX with a given number of qubits and gates '''
    qc = QuantumCircuit(numQubits)
    svCnt = 0

    while svCnt <= numGates:
        for i in range(3):
            for j in range(numQubits):
                qc.ry(random.uniform(0, 2 * pi), j)
                qc.save_statevector(label = str(svCnt))
                svCnt += 1
        for j in range(numQubits-1, 0, -2):
            qc.cx(j, j-1)
            qc.save_statevector(label = str(svCnt))
            svCnt += 1
        for j in range(numQubits-2, 0, -2):
            qc.cx(j, j-1)
            qc.save_statevector(label = str(svCnt))
            svCnt += 1

    print(f'[RandomRandom] #Qubits: [{numQubits}] #Depths: [{qc.depth()}] #Saved Statevectors: [{svCnt}]')
    
    return qc