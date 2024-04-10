#include "DicCounter.h"

int findPos(double v,vector<double> &sortedAmp){
    int l=0,r=sortedAmp.size()-1;
    while (l<=r){
        int mid=(l+r)/2;
        if (sortedAmp[mid]==v) return mid;
        if (sortedAmp[mid]<v)
            l=mid+1;
        else r=mid-1;
    }
    return -1;
}

void lzw(int x,map<vector<int>,int> &sid,vector<int> &t,vector<int> &s1){
    if (!sid.count({x})) sid[{x}]=sid.size();
    t.push_back(x);
    // for (auto y:t) {cout<<y<<" ";} cout<<endl;
    if (!sid.count(t)){
        sid[t]=sid.size();
        // for (auto y:t) {cout<<y<<" ";}
        // cout<<"="<<sid[t]<<endl;
        t.pop_back();
        if (!t.empty()) s1.push_back(sid[t]),t.clear();
        t.push_back(x);
    }
}

vector<int> DicCounterEncoder(
    Matrix &m,
    vector<double> &sortedAmp,
    vector<pair<double,int> > &ampCnt,
    vector<vector<int> > &seq,
    double &compRate){
    
    int n=m.row;
    set<double> tmpSet;
    for (int i=0;i<n;i++) tmpSet.insert(m.data[i][0]);
    for (auto it=tmpSet.begin();it!=tmpSet.end();it++) sortedAmp.push_back(*it);

    double v=m.data[0][0];
    int cnt=1;
    map<pair<double,int>,int> pid;
    vector<int> s0,s1;
    map<vector<int>,int> sid;
    for (int i=1;i<n;i++){
        // cout<<m.data[i][0]<<endl;
        if (m.data[i][0]==v)
            cnt++;
        else{
            int p=findPos(v,sortedAmp);
            if (cnt==1){
                // s0.push_back(p);
                lzw(p,sid,s0,s1);
            }
            else{
                if (!pid.count({p,cnt})) pid[{p,cnt}]=pid.size()+sortedAmp.size();
                // s0.push_back(pid[{p,cnt}]);
                lzw(pid[{p,cnt}],sid,s0,s1);
            }
            v=m.data[i][0];
            cnt=1;
        }
    }
    int p=findPos(v,sortedAmp);
    if (cnt==1){
        // s0.push_back(p);
        lzw(p,sid,s0,s1);
    }
    else{
        if (!pid.count({p,cnt})) pid[{p,cnt}]=pid.size()+sortedAmp.size();
        // s0.push_back(pid[{p,cnt}]);
        lzw(pid[{p,cnt}],sid,s0,s1);
    }
    if (!sid.count(s0)) sid[s0]=sid.size();
    s1.push_back(sid[s0]);

    int nSingle=sortedAmp.size();
    ampCnt.resize(pid.size());
    for (auto it=pid.begin();it!=pid.end();it++) ampCnt[it->second-nSingle]=it->first;
    seq.resize(sid.size());
    for (auto it=sid.begin();it!=sid.end();it++) seq[it->second]=it->first;

    compRate=1.0*s1.size()*sizeof(int)/(n*sizeof(double));

    return s1;
}

Matrix DicCounterDecoder(
    int n,
    vector<int> &v,
    vector<double> &sortedAmp,
    vector<pair<double,int> > &ampCnt,
    vector<vector<int> > &seq){
    Matrix m(n,1);
    int i=0;
    int nSingle=sortedAmp.size();
    for (auto x:v){
        vector<int> s=seq[x];
        // cout<<"*********\n";
        for (auto y:s){
            // cout<<y<<endl;
            double val;
            int cnt;
            if (y<nSingle) 
                val=sortedAmp[y],cnt=1;
            else val=sortedAmp[ampCnt[y-nSingle].first],cnt=ampCnt[y-nSingle].second;
            for (int j=0;j<cnt;j++) m.data[i++][0]=val;
        }
    }
    return m;
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