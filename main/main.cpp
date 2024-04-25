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

    cout << "[DEBUG] numQubits: " << numQubits << " memQubits: " << memQubits << " numDepths: " << numDepths << endl;

    // 
    // Generate a quantum circuit
    // 
    QCircuit qc = Grover(numQubits);
    // QCircuit qc = RandomRegular(numQubits, numDepths);
    // QCircuit qc = RandomMedium(numQubits, numDepths);
    QCircuit qc = RandomRandom(numQubits, numDepths);
    // QCircuit qc = test(numQubits, numDepths, memQubits);

    Timer timer;

    //
    // Call different simulators
    //
    // Method 1: Local SVSim
    // timer.Start();
    // SVSim(qc);
    // timer.End();
    // timer.ElapsedTime();

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
    {
        // double t[][1]={1,1,1,2,2,1,3,3,3,3,2,2,1};
        // int n=sizeof(t)/sizeof(double);
        // Matrix m((double**)t,n,1);
        // vector<double> amp;
        // vector<pair<double,int> > pCnt;
        // vector<vector<int> > seq;
        // double compRate;
        // vector<int> res=DicCounterEncoder(m,amp,pCnt,seq,compRate);
        // cout<<"compression rate: "<<compRate<<endl;
        // Matrix m0;
        // m0=DicCounterDecoder(n,res,amp,pCnt,seq);
        // int f=1;
        // for (int i=0;i<n;i++)
        //     if (m0.data[i][0]!=m.data[i][0]){
        //         f=0;
        //         break;
        //     }
        // cout<<(f?"pass":"fail")<<endl;

        // return 0;

        try {
            // Initialize the state vector
            long long N = (1 << qc.numQubits);
            Matrix localSv = Matrix(N, 1);
            localSv.data[0][0] = 1;

            for (int i = 0; i < qc.numDepths; ++ i) {
                // Conduct one level of indexing
                LocalComputing(localSv, N, qc.gates[i], qc.numQubits, 0);
                vector<double> sA;
                vector<pair<double,int> > pCnt;
                vector<vector<int> > seq;
                double compRate;
                vector<int> res=DicCounterEncoder(localSv,sA,pCnt,seq,compRate);
                // for (int j=0;j<N;j++) {cout<<localSv.data[j][0]<<" ";}
                // cout<<endl;
                cout<<"depth: "<<i<<", compression rate: "<<compRate<<endl;
                int seqSize=0;
                for (auto &x:seq){
                    seqSize+=x.size();
                }
                cout<<"uncompressed data size:"<<sizeof(double)*N<<", compressed data size: "<<sizeof(int)*res.size()<<", lzw dict size: "<<seqSize*sizeof(int)<<endl;
                // Matrix m0;
                // m0=DicCounterDecoder(N,res,sA,pCnt,seq);
                // int f=1;
                // for (int j=0;j<N;j++)
                //     if (m0.data[j][0]!=localSv.data[j][0]){
                //         f=0;
                //         break;
                //     }
                // cout<<(f?"pass":"fail")<<endl;
            }
            // Write the state vector to the output file
            WriteBlock(localSv, 0, 1, "./output/svsim/");
        } catch (const bad_alloc& e) {
            cout << "[ERROR] Memory allocation failed: " << e.what() << endl;
        }
    }

    return 0;
}