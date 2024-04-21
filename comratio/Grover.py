from qiskit import QuantumCircuit
from math import pi, sqrt
from qiskit.circuit.library.standard_gates import ZGate

'''
The visualization of a 3-qubit Grover's circuit with target |111>:
https://algassert.com/quirk#circuit={%22cols%22:[[%22H%22,%22H%22,%22H%22],[%22%E2%80%A2%22,%22%E2%80%A2%22,%22Z%22],[%22H%22,%22H%22,%22H%22],[%22X%22,%22X%22,%22X%22],[%22%E2%80%A2%22,%22%E2%80%A2%22,%22Z%22],[%22X%22,%22X%22,%22X%22],[%22H%22,%22H%22,%22H%22],[%22%E2%80%A2%22,%22%E2%80%A2%22,%22Z%22],[%22H%22,%22H%22,%22H%22],[%22X%22,%22X%22,%22X%22],[%22%E2%80%A2%22,%22%E2%80%A2%22,%22Z%22],[%22X%22,%22X%22,%22X%22],[%22H%22,%22H%22,%22H%22]]}
'''

def Grover(numQubits):
    ''' One implementation of Grover\'s algorithm '''
    qc = QuantumCircuit(numQubits)
    cz = ZGate().control(numQubits-1)
    qubits = [i for i in range(numQubits)]

    svCnt = 0

    # 1. Apply Hadamard gates to all qubits
    for j in range(numQubits): # |00...0> => |u>
        qc.h(j)
        qc.save_statevector(label = str(svCnt))
        svCnt += 1

    # 2. Start the Grover iteration
    numIterations = int(round(pi / 4 * sqrt(pow(2, numQubits)), 0))

    for _ in range(numIterations):
        # 2.1. Apply the phase inversion use the controlled-Z gate
        # i.e., transform the target state |x*> into -|x*>

        # ######################################################
        # [OPTIONAL] Add X gates to transform |x*> => |11...1>
        # for j in range(numQubits): # <e.g.> |x*> = |00...0> => |11...1>
        #     qc.x(j)
        # ######################################################
        # [DEFAULT] |x*> = |11...1>
        qc.append(cz, qubits)  # CC...Z gate: |11...1> => -|11...1>
        qc.save_statevector(label = str(svCnt))
        svCnt += 1
        # ######################################################
        # [OPTIONAL] Add X gates to recover |11...1> => |x*>
        # for j in range(numQubits): # <e.g.> |11...1> => |x*> = |00...0>
        #     qc.x(j)
        # ######################################################

        # 2.2. Apply the diffusion operator
        # 2.2.1. Apply H, X to all qubits
        for j in range(numQubits):
            qc.h(j) # |u> => |00...0>
            qc.save_statevector(label = str(svCnt))
            svCnt += 1
            qc.x(j) # |00...0> => |11...1>
            qc.save_statevector(label = str(svCnt))
            svCnt += 1

        # 2.2.2. Apply the controlled-Z gate
        qc.append(cz, qubits) # |11...1> => -|11...1>
        qc.save_statevector(label = str(svCnt))
        svCnt += 1

        # 2.2.3. Recover X, H
        for j in range(numQubits):
            qc.x(j) # |11...1> => |00...0>
            qc.save_statevector(label = str(svCnt))
            svCnt += 1
            qc.h(j) # |00...0> => |u>
            qc.save_statevector(label = str(svCnt))
            svCnt += 1
    
    print(f'[Grover] #Qubits: [{numQubits}] #Depths: [{qc.depth()}] #Saved Statevectors: [{svCnt}]')

    return qc
