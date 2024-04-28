#include "DicCounter.h"

// int findPos(double v,vector<double> &sortedAmp){
//     int l=0,r=sortedAmp.size()-1;
//     while (l<=r){
//         int mid=(l+r)/2;
//         if (sortedAmp[mid]==v) return mid;
//         if (sortedAmp[mid]<v)
//             l=mid+1;
//         else r=mid-1;
//     }
//     return -1;
// }

/**
 * @brief
 *
 * @param x         the hash code of current element
 * @param sid       the dictionary of LZW
 * @param t
 * @param s1        the compressed data
 * @param size      the size of the dictionary
 * @param maxSize   the size limit of the dictionary
 */
void lzw(int x, map<vector<int>, int> &sid, vector<int> &t, vector<int> &s1, int &size, int maxSize)
{
    if (!sid.count({x})) // if {x} is not a recorded pattern
    {
        if (size + 1 < maxSize)
        {
            sid[{x}] = sid.size(); // insert {x} to the LZW dictionary
            size++;
        }
        else
        {
            s1.push_back(-x - 1);
            t.clear();
            return;
        }
    }
    t.push_back(x); // t := t + {x}
    // for (auto y:t) {cout<<y<<" ";} cout<<endl;
    if (!sid.count(t)) // if t is not a recorded pattern
    {
        if (size + (int)t.size() < maxSize) // record a new pattern t
        {
            sid[t] = sid.size();
            size += t.size();
        }
        // for (auto y:t) {cout<<y<<" ";}
        // cout<<"="<<sid[t]<<endl;
        t.pop_back(); // t - x is a recorded pattern, but t is not
        if (!t.empty())
            s1.push_back(sid[t]), t.clear(); // record t - x
        t.push_back(x);
    }
}

vector<int> DicCounterEncoder(
    Matrix &m,
    vector<double> &amp,
    vector<pair<int, int>> &ampCnt,
    vector<vector<int>> &seq,
    double &compRate)
{

    int n = m.row;
    // set<double> tmpSet;
    // for (int i=0;i<n;i++) tmpSet.insert(m.data[i][0]);
    // for (auto it=tmpSet.begin();it!=tmpSet.end();it++) sortedAmp.push_back(*it);

    int prec = 5;

    double v = m.data[0][0];
    v = round(v * pow(10, prec)) / pow(10, prec);
    int cnt = 1;
    map<double, int> aid;            // map a new amplitude to integer
    map<pair<double, int>, int> pid; // map (amp, counter) to integer
    vector<int> s0, s1;              //
    map<vector<int>, int> sid;
    int size = 0;
    int maxSize = 10000; // n / 10;
    for (int i = 1; i < n; i++)
    {
        // cout<<m.data[i][0]<<endl;
        // if (m.data[i][0] == v) // repeat counter
        if (fabs(m.data[i][0] - v) < pow(10.0, -prec))
        {
            cnt++;
            // cout << "i: " << i << " cnt: " << cnt << " v: " << v << endl;
        }
        else
        { // add to dictionary
            // int p=findPos(v,sortedAmp);
            int p;
            // 1. each amplitude should be given a integer index
            // 1.1. check if v is in aid
            if (aid.count(v))
            {
                p = aid[v]; // p := v's index in aid
            }
            // 1.2. if not, insert v to aid, give v a new index
            else
            {
                p = aid.size();
                aid[v] = aid.size();
            }
            // 2. LZW
            // 2.1. if cnt=1, use the index of v, i.e., p
            if (cnt == 1)
            {
                // s0.push_back(p);
                lzw(p, sid, s0, s1, size, maxSize);
            }
            // 2.2. if cnt>1, use the index of (v,cnt), i.e., pid[{v,cnt}]
            else
            {
                if (!pid.count({p, cnt}))
                    pid[{p, cnt}] = pid.size() + n;
                // s0.push_back(pid[{p,cnt}]);
                lzw(pid[{p, cnt}], sid, s0, s1, size, maxSize);
            }
            v = m.data[i][0];
            v = round(v * pow(10, prec)) / pow(10, prec);
            cnt = 1;
        }
    }
    int p;
    if (aid.count(v))
    {
        p = aid[v];
    }
    else
    {
        p = aid.size();
        aid[v] = aid.size();
    }
    if (cnt == 1)
    {
        // s0.push_back(p);
        lzw(p, sid, s0, s1, size, maxSize);
    }
    else
    {
        if (!pid.count({p, cnt}))
            pid[{p, cnt}] = pid.size() + n;
        // s0.push_back(pid[{p,cnt}]);
        lzw(pid[{p, cnt}], sid, s0, s1, size, maxSize);
    }
    if (!sid.count(s0))
        sid[s0] = sid.size();
    s1.push_back(sid[s0]);

    int nSingle = aid.size();
    amp.resize(nSingle);
    for (auto it = aid.begin(); it != aid.end(); it++)
        amp[it->second] = it->first;

    ampCnt.resize(pid.size());
    for (auto it = pid.begin(); it != pid.end(); it++) // pid: (amp, cnt) -> int
        ampCnt[it->second - n] = it->first;

    seq.resize(sid.size());
    for (auto it = sid.begin(); it != sid.end(); it++)
        seq[it->second] = it->first;

    compRate = 1.0 * s1.size() * sizeof(int) / (n * sizeof(double));

    // output the dictionary
    // cout << "Map contents aid:" << endl;
    // for (auto it = aid.begin(); it != aid.end(); ++it) {
    //     cout << "Key: " << it->first << ", Value: " << it->second << endl;
    // }

    return s1;
}

Matrix DicCounterDecoder(
    int n,
    vector<int> &v,
    vector<double> &amp,
    vector<pair<int, int>> &ampCnt,
    vector<vector<int>> &seq)
{
    Matrix m(n, 1);
    int i = 0;
    double val;
    int cnt;
    for (auto x : v)
    {
        if (x < 0)
        {
            x = -x - 1;
            if (x < n)
                val = amp[x], cnt = 1;
            else
                val = amp[ampCnt[x - n].first], cnt = ampCnt[x - n].second;
            for (int j = 0; j < cnt; j++)
                m.data[i++][0] = val;
            continue;
        }
        vector<int> s = seq[x];
        // cout<<"*********\n";
        for (auto y : s)
        {
            // cout<<y<<endl;
            if (y < n)
                val = amp[y], cnt = 1;
            else
                val = amp[ampCnt[y - n].first], cnt = ampCnt[y - n].second;
            for (int j = 0; j < cnt; j++)
                m.data[i++][0] = val;
        }
    }
    return m;
}

void DicCounter(QCircuit &qc, int memQubits)
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
    try
    {
        // Initialize the state vector
        long long N = (1 << qc.numQubits);
        long long memSize = (1 << memQubits) * sizeof(double);

        Matrix localSv = Matrix(N, 1);
        localSv.data[0][0] = 1;
        
        long long compSizeLast = 0;
        long long compSizeCurr = 0;

        // Print the compressed result
        ofstream outputFile("./output/dic_cratio.txt");
        if (!outputFile.is_open()) {
            cerr << "[ERROR] Failed to open the file!" << endl;
            exit(1);
        }

        for (int i = 0; i < qc.numDepths; ++i)
        {
            if (i % 200 == 0) {
                cout << "[DEBUG] << Level " << i << endl;
            }
            // Conduct one level of indexing
            LocalComputing(localSv, N, qc.gates[i], qc.numQubits, 0);

            if (i < 330) {
                continue;
            }

            // Compress the state vector
            vector<double> sA;
            vector<pair<int, int>> pCnt;
            vector<vector<int>> seq;
            double compRate;
            vector<int> res = DicCounterEncoder(localSv, sA, pCnt, seq, compRate);

            // Calculate the memory footprint
            long long seqSize = 0;
            for (auto &x:seq){
                seqSize += x.size();
            }
            seqSize *= sizeof(int);
            long long ampSize = sA.size() * sizeof(double);
            long long ampCntSize = pCnt.size() * (sizeof(int) + sizeof(int));

            if (i == 0) {
                compSizeLast = res.size() * sizeof(int) + ampSize + ampCntSize + seqSize;
            } else {
                compSizeCurr = res.size() * sizeof(int) + ampSize + ampCntSize + seqSize;
                outputFile << compSizeLast + compSizeCurr << endl;

                if (compSizeLast + compSizeCurr > memSize) {
                    outputFile.close();
                    cout << "[ERROR] Level [" << i << "] memory overflow: " << compSizeLast + compSizeCurr << " > " << memSize << endl;
                    exit(1);
                }

                compSizeLast = compSizeCurr;
            }

            // cout << "depth: " << i << ", compression rate: " << compRate << endl; // <<", lzw dict size: "<<seqSize*sizeof(int)<<endl;
            // cout<<"uncompressed data size:"<<sizeof(double)*N<<", compressed data size: "<<sizeof(int)*res.size()<<", lzw dict size: "<<seqSize*sizeof(int)<<endl;
            
            // Check decompression results
            // Matrix m0;
            // m0 = DicCounterDecoder(N, res, sA, pCnt, seq);
            // int f = 1;
            // for (int j = 0; j < N; j++)
            //     if (fabs(m0.data[j][0] - localSv.data[j][0]) > 1e-5)
            //     {
            //         // cout << "[ERROR] DicCounter decompress failed!" << endl;
            //         // exit(1);
            //         f = 0;
            //         // WriteBlock(localSv, 0, 1, "./output/dic/");
            //         // WriteBlock(m0, 1, 1, "./output/dic/");
            //         break;
            //     }
            // cout << "Level [" << i << "] " << (f ? "pass" : "fail") << endl;
        }
        // Write the state vector to the output file
        // WriteBlock(localSv, 0, 1, "./output/svsim/");

        outputFile.close();
        cout << "Memory footprints have been written to ./output/dic_cratio.txt" << endl;
    }
    catch (const bad_alloc &e)
    {
        cout << "[ERROR] Memory allocation failed: " << e.what() << endl;
    }
}

// vector<int> DicCounterEncoder(
//     Matrix &m,
//     vector<double> &sortedAmp,
//     vector<pair<double,int> > &ampCnt,
//     vector<vector<int> > &seq){

//     int n=m.row;
//     set<double> tmpSet;
//     for (int i=0;i<n;i++) tmpSet.insert(m.data[i][0]);
//     for (auto it=tmpSet.begin();it!=tmpSet.end();it++) sortedAmp.push_back(*it);

//     double v=m.data[0][0];
//     int cnt=1;
//     map<pair<double,int>,int> pid;
//     vector<int> s0;
//     for (int i=1;i<n;i++){
//         // cout<<m.data[i][0]<<endl;
//         if (m.data[i][0]==v)
//             cnt++;
//         else{
//             int p=findPos(v,sortedAmp);
//             if (cnt==1){
//                 s0.push_back(p);
//             }
//             else{
//                 if (!pid.count({p,cnt})) pid[{p,cnt}]=pid.size()+sortedAmp.size();
//                 s0.push_back(pid[{p,cnt}]);
//             }
//             v=m.data[i][0];
//             cnt=1;
//         }
//     }
//     if (cnt==1){
//         s0.push_back(findPos(v,sortedAmp));
//     }
//     else{
//         if (!pid.count({v,cnt})) pid[{v,cnt}]=pid.size()+sortedAmp.size();
//         s0.push_back(pid[{v,cnt}]);
//     }

//     // for (auto x:s0) {cout<<x<<" "; }
//     // cout<<endl;
//     vector<int> s1;
//     vector<int> t;
//     map<vector<int>,int> sid;
//     for (auto x:s0){
//         if (!sid.count({x})) sid[{x}]=sid.size();
//         t.push_back(x);
//         // for (auto y:t) {cout<<y<<" ";} cout<<endl;
//         if (!sid.count(t)){
//             sid[t]=sid.size();
//             // for (auto y:t) {cout<<y<<" ";}
//             // cout<<"="<<sid[t]<<endl;
//             t.pop_back();
//             if (!t.empty()) s1.push_back(sid[t]),t.clear();
//             t.push_back(x);
//         }
//     }
//     if (!sid.count(t)) sid[t]=sid.size();
//     s1.push_back(sid[t]);

//     int nSingle=sortedAmp.size();
//     ampCnt.resize(pid.size());
//     for (auto it=pid.begin();it!=pid.end();it++) ampCnt[it->second-nSingle]=it->first;
//     seq.resize(sid.size());
//     for (auto it=sid.begin();it!=sid.end();it++) seq[it->second]=it->first;
//     return s1;
// }

// vector<int> DicCounterEncoder(Matrix &m,vector<pair<double,int> > &pCnt,vector<vector<int> > &seq){
//     int n=m.row;
//     double v=m.data[0][0];
//     int cnt=1;
//     map<pair<double,int>,int> pid;
//     vector<int> s0;
//     for (int i=1;i<n;i++)
//         if (m.data[i][0]==v)
//             cnt++;
//         else{
//             if (!pid.count({v,cnt})) pid[{v,cnt}]=pid.size();
//             s0.push_back(pid[{v,cnt}]);
//             v=m.data[i][0];
//             cnt=1;
//         }
//     if (!pid.count({v,cnt})) pid[{v,cnt}]=pid.size();
//     s0.push_back(pid[{v,cnt}]);

//     vector<int> s1;
//     vector<int> t;
//     map<vector<int>,int> sid;
//     for (int i=0;i<(int)pid.size();i++) sid[{i}]=sid.size();
//     for (auto x:s0){
//         t.push_back(x);
//         // for (auto y:t) {cout<<y<<" ";} cout<<endl;
//         if (!sid.count(t)){
//             sid[t]=sid.size();
//             // for (auto y:t) {cout<<y<<" ";}
//             // cout<<"="<<sid[t]<<endl;
//             t.pop_back();
//             if (!t.empty()) s1.push_back(sid[t]),t.clear();
//             t.push_back(x);
//         }
//     }
//     if (!sid.count(t)) sid[t]=sid.size();
//     s1.push_back(sid[t]);

//     pCnt.resize(pid.size());
//     for (auto it=pid.begin();it!=pid.end();it++) pCnt[it->second]=it->first;
//     seq.resize(sid.size());
//     for (auto it=sid.begin();it!=sid.end();it++) seq[it->second]=it->first;
//     return s1;
// }

// Matrix DicCounterDecoder(int n,vector<int> &v,vector<pair<double,int> > &pCnt,vector<vector<int> > &seq){
//     Matrix m(n,1);
//     int i=0;
//     for (auto x:v){
//         vector<int> s=seq[x];
//         for (auto y:s){
//             auto p=pCnt[y];
//             for (int j=0;j<p.second;j++) m.data[i++][0]=p.first;
//         }
//     }
//     return m;
// }