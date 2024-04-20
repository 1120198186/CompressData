import numpy as np
from qiskit import transpile
from qiskit_aer import AerSimulator
from Grover import Grover

'''
The versions of qiskit and qiskit-aer used in this script:
>>> qiskit.__version__
'1.0.2'
>>> qiskit_aer.__version__
'0.14.0.1'
'''

def qiskitSim(qc):
    ''' Run qc using the qiskit statevector simulator '''
    simulator = AerSimulator(method='statevector')
    qc = transpile(qc, simulator) # transpile the CC...Z gate
    svDict = simulator.run(qc).result().data() # get multiple saved state vectors
    # print(svDict)
    processSvDict(svDict)
    return

def processSvDict(svDict):
    '''
    Check if the amplitudes in all state vectors are real numbers,
    only save the real parts (currently), and calculate the compression ratio
    '''
    for key, sv in svDict.items():
        sv = np.asarray(sv)

        # check if sv is a real state vector
        realParts = np.real(sv)
        imagParts = np.imag(sv)
        if not np.all(np.abs(imagParts) < 1e-15):
            print(f'[WARNING] Imaginary parts are not all zero for state vector {key}!')
        else:
            cratio = compressionRatio(realParts)
            print(f'sv[{key}]: compression ratio = {cratio}')
            print(f'sv[{key}]: {realParts}')
    return

def compressionRatio(sv):
    ''' Calculate the compression ratio of a given state vector '''
    cratio = 0
    # TODO: implement the compression ratio calculation
    print('[TODO] Implement the compression ratio calculation')
    return cratio

if __name__ == '__main__':
    qc = Grover(3)
    qiskitSim(qc)
    # qc.draw('mpl') # draw the circuit