from math import log2
from qiskit import QuantumCircuit
from qiskit.circuit.library.standard_gates import TGate

'''
The visualization of a 3-qubit QFT circuit with initial period 2:
https://algassert.com/quirk#circuit=%7B%22cols%22%3A%5B%5B1%2C%22H%22%2C%22H%22%5D%2C%5B%22Amps3%22%5D%2C%5B%5D%2C%5B%22Swap%22%2C1%2C%22Swap%22%5D%2C%5B%22H%22%5D%2C%5B%22Z%5E%C2%BD%22%2C%22%E2%80%A2%22%5D%2C%5B1%2C%22H%22%5D%2C%5B%22Z%5E%C2%BC%22%2C%22Z%5E%C2%BD%22%2C%22%E2%80%A2%22%5D%2C%5B1%2C1%2C%22H%22%5D%5D%7D

The visualization of a 5-qubit QFT circuit with initial period N/2:
https://algassert.com/quirk#circuit={%22cols%22:[[1,1,1,1,%22H%22],[%22Chance5%22],[%22%E2%80%A6%22,%22%E2%80%A6%22,%22%E2%80%A6%22,%22%E2%80%A6%22,%22%E2%80%A6%22],[%22Swap%22,1,1,1,%22Swap%22],[1,%22Swap%22,1,%22Swap%22],[%22H%22],[%22Z^%C2%BD%22,%22%E2%80%A2%22],[1,%22H%22],[%22Z^%C2%BC%22,%22Z^%C2%BD%22,%22%E2%80%A2%22],[1,1,%22H%22],[%22Z^%E2%85%9B%22,%22Z^%C2%BC%22,%22Z^%C2%BD%22,%22%E2%80%A2%22],[1,1,1,%22H%22],[%22Z^%E2%85%9F%E2%82%81%E2%82%86%22,%22Z^%E2%85%9B%22,%22Z^%C2%BC%22,%22Z^%C2%BD%22,%22%E2%80%A2%22],[1,1,1,1,%22H%22]]}

The visualization of a 10-qubit QFT circuit with initial period 32:
https://algassert.com/quirk#circuit={%22cols%22:[[1,1,1,1,1,%22H%22,%22H%22,%22H%22,%22H%22,%22H%22],[%22Chance10%22],[%22Swap%22,1,1,1,1,1,1,1,1,%22Swap%22],[1,%22Swap%22,1,1,1,1,1,1,%22Swap%22],[1,1,%22Swap%22,1,1,1,1,%22Swap%22],[1,1,1,%22Swap%22,1,1,%22Swap%22],[1,1,1,1,%22Swap%22,%22Swap%22],[%22H%22],[%22Z^%C2%BD%22,%22%E2%80%A2%22],[1,%22H%22],[%22Z^%C2%BC%22,%22Z^%C2%BD%22,%22%E2%80%A2%22],[1,1,%22H%22],[%22Z^%E2%85%9B%22,%22Z^%C2%BC%22,%22Z^%C2%BD%22,%22%E2%80%A2%22],[1,1,1,%22H%22],[%22Z^%E2%85%9F%E2%82%81%E2%82%86%22,%22Z^%E2%85%9B%22,%22Z^%C2%BC%22,%22Z^%C2%BD%22,%22%E2%80%A2%22],[1,1,1,1,%22H%22],[{%22id%22:%22Z^ft%22,%22arg%22:%221/32%22},{%22id%22:%22Z^ft%22,%22arg%22:%221/16%22},{%22id%22:%22Z^ft%22,%22arg%22:%221/8%22},%22Z^%C2%BC%22,%22Z^%C2%BD%22,%22%E2%80%A2%22],[1,1,1,1,1,%22H%22],[{%22id%22:%22Z^ft%22,%22arg%22:%221/64%22},{%22id%22:%22Z^ft%22,%22arg%22:%221/32%22},{%22id%22:%22Z^ft%22,%22arg%22:%221/16%22},{%22id%22:%22Z^ft%22,%22arg%22:%221/8%22},%22Z^%C2%BC%22,%22Z^%C2%BD%22,%22%E2%80%A2%22],[1,1,1,1,1,1,%22H%22],[{%22id%22:%22Z^ft%22,%22arg%22:%221/128%22},{%22id%22:%22Z^ft%22,%22arg%22:%221/64%22},{%22id%22:%22Z^ft%22,%22arg%22:%221/32%22},{%22id%22:%22Z^ft%22,%22arg%22:%221/16%22},{%22id%22:%22Z^ft%22,%22arg%22:%221/8%22},%22Z^%C2%BC%22,%22Z^%C2%BD%22,%22%E2%80%A2%22],[1,1,1,1,1,1,1,%22H%22],[{%22id%22:%22Z^ft%22,%22arg%22:%221/256%22},{%22id%22:%22Z^ft%22,%22arg%22:%221/128%22},{%22id%22:%22Z^ft%22,%22arg%22:%221/64%22},{%22id%22:%22Z^ft%22,%22arg%22:%221/32%22},{%22id%22:%22Z^ft%22,%22arg%22:%221/16%22},{%22id%22:%22Z^ft%22,%22arg%22:%221/8%22},%22Z^%C2%BC%22,%22Z^%C2%BD%22,%22%E2%80%A2%22],[1,1,1,1,1,1,1,1,%22H%22],[{%22id%22:%22Z^ft%22,%22arg%22:%221/512%22},{%22id%22:%22Z^ft%22,%22arg%22:%221/256%22},{%22id%22:%22Z^ft%22,%22arg%22:%221/128%22},{%22id%22:%22Z^ft%22,%22arg%22:%221/64%22},{%22id%22:%22Z^ft%22,%22arg%22:%221/32%22},{%22id%22:%22Z^ft%22,%22arg%22:%221/16%22},{%22id%22:%22Z^ft%22,%22arg%22:%221/8%22},%22Z^%C2%BC%22,%22Z^%C2%BD%22,%22%E2%80%A2%22],[1,1,1,1,1,1,1,1,1,%22H%22]],%22gates%22:[{%22id%22:%22~7idu%22,%22matrix%22:%22{{0.9999994-0.0010908i,0},{0,0.9999994+0.0010908i}}%22}]}
'''

def QFT(numQubits, period=0):
    '''
    One implementation of the Quantum Fourier Transform (QFT) circuit
        numQubits: the number of qubits
        period: the period of the initial state
    '''
    qc = QuantumCircuit(numQubits)
    ct = TGate().control(1)
    svCnt = 0

    # [NOTE] QFT transforms a state with period p to N / p
    # Since we want to use RepeatCounter, the periods of both initial and final states should be as big as possible
    # i.e., p = N / p, p = (1 << (n / 2))
    if period == 0:
        period = (1 << (numQubits // 2))

    # 1. Set the period of the initial state to 'period'
    # the period can be set by applying H gates to high-order qubits
    N = (1 << numQubits)
    numHGates = int(log2(N / period))
    print(N, numHGates)
    for i in range(numQubits-1, numQubits-1-numHGates, -1):
        qc.h(i)
        qc.save_statevector(label = str(svCnt))
        svCnt += 1

    # 2. Reorder all qubits
    # from 0, 1, ..., numQubits-1 to numQubits-1, numQubits-2, ..., 0
    ii = 0
    jj = numQubits - 1
    while (ii < jj):
        qc.swap(ii, jj)
        qc.save_statevector(label = str(svCnt))
        svCnt += 1
        ii += 1
        jj -= 1

    # 3. Apply the controlled gates
    # 3.1. The first group with H and CS
    qc.h(0)
    qc.save_statevector(label = str(svCnt))
    svCnt += 1
    
    qc.cs(1, 0)
    qc.save_statevector(label = str(svCnt))
    svCnt += 1

    # 3.2. The second group with H, CS, and CT
    qc.h(1)
    qc.save_statevector(label = str(svCnt))
    svCnt += 1

    qc.cs(2, 1)
    qc.save_statevector(label = str(svCnt))
    svCnt += 1

    qc.append(ct, [2, 0])
    qc.save_statevector(label = str(svCnt))
    svCnt += 1

    # 3.3. The remaining groups with H, CS, CT, and CRZ
    n = numQubits - 1
    i = 2
    while (i < n):
        qc.h(i)
        qc.save_statevector(label = str(svCnt))
        svCnt += 1

        qc.cs(i+1, i)
        qc.save_statevector(label = str(svCnt))
        svCnt += 1

        qc.append(ct, [i+1, i-1])
        qc.save_statevector(label = str(svCnt))
        svCnt += 1

        t = i
        tt = 0
        while (t >= 2):
            qc.crz(1/(2**(t+1)), i+1, tt)
            qc.save_statevector(label = str(svCnt))
            svCnt += 1
            tt += 1
            t -= 1

        i += 1
    
    # 4. Apply the final H gate
    qc.h(n)
    qc.save_statevector(label = str(svCnt))
    svCnt += 1

    print(f'[QFT] #Qubits: [{numQubits}] #Depths: [{qc.depth()}] #Saved Statevectors: [{svCnt}] Init period: [{period}] Final period: [{N / period}]')

    return qc