#include "qcircuit.h"
#include "timer.h"

#include "SVSim.h"
#include "BlockSVSim.h"
#include "HybridSVSim.h"

#include "DicCounter.h"

int main(int argc, char** argv) {
    if (argc != 4) {
        cout << "[ERROR] usage: cmd <numQubits> <memQubits> <numDepths>" << endl;
        exit(1);
    }

    srand((int)time(0));

    int numQubits = atoi(argv[1]); // the number of qubits of the circuit
    int memQubits = atoi(argv[2]); // the maximum number of qubits in the memory <= numQubits
    int numDepths = atoi(argv[3]); // the number of depths of the circuit

    // 
    // Generate a quantum circuit
    // 
    // QCircuit qc = RandomRegular(numQubits, numDepths);
    // QCircuit qc = RandomMedium(numQubits, numDepths);
    // QCircuit qc = RandomRandom(numQubits, numDepths);
    QCircuit qc = test(numQubits, numDepths, memQubits);

    Timer timer;

    //
    // Call different simulators
    //
    // Method 1: Local SVSim
    timer.Start();
    SVSim(qc);
    timer.End();
    timer.ElapsedTime();

    // Method 2: Block-based SVSim
    // timer.Start();
    // BlockSVSim(qc, memQubits);
    // timer.End();
    // timer.ElapsedTime();

    // Method 3: Hybrid SVSim
    // timer.Start();
    // HybridSVSim(qc, memQubits);
    // timer.End();
    // timer.ElapsedTime();

    // TODO: Method 4: Repeat Counter

    // TODO: Method 5: Dictionary + Counter
    // {
    //     double t[][1]={1,1,1,2,2,1,3,3,3,3,2,2,1};
    //     int n=sizeof(t)/sizeof(double);
    //     Matrix m((double**)t,n,1);
    //     vector<double> sA;
    //     vector<pair<double,int> > pCnt;
    //     vector<vector<int> > seq;
    //     double compRate;
    //     vector<int> res=DicCounterEncoder(m,sA,pCnt,seq,compRate);
    //     cout<<"compression rate: "<<compRate<<endl;
    //     Matrix m0;
    //     m0=DicCounterDecoder(n,res,sA,pCnt,seq);
    //     int f=1;
    //     for (int i=0;i<n;i++)
    //         if (m0.data[i][0]!=m.data[i][0]){
    //             f=0;
    //             break;
    //         }
    //     cout<<(f?"pass":"fail")<<endl;
    // }

    return 0;
}