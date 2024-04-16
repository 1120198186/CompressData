from qiskit import *
from mqt import ddsim
from mqt.ddsim import CircuitSimulator
import time
from math import pi
from qiskit_aer import AerSimulator
import qiskit.quantum_info as qi

from qcircuit import *

def qiskitSim(circ) :
    simulator = AerSimulator(method='statevector')
#     circ = transpile(circ, simulator)
    result = simulator.run(circ).result()
#     statevector = result.get_statevector(circ)
#     print(statevector)
    # print(result.data(0))

def ddSim(circ) :
    sim = CircuitSimulator(circ)
    result = sim.simulate(shots = 1)
#     sv = sim.get_vector()
#     object_size = sys.getsizeof(sv[0])
#     print(object_size)

if __name__ == '__main__':
    circ = RandomRegular(numQubits=20, numDepths=20)

    # circ = VQC(10)
    # circ = QFT(400)
    # circ.draw('mpl')

    start_time = time.time()

    qiskitSim(circ)
    # ddSim(circ)

    end_time = time.time()

    execution_time = end_time - start_time
    print(execution_time)