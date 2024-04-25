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

void lzw(int x,map<vector<int>,int> &sid,vector<int> &t,vector<int> &s1,int &size,int maxSize){
    if (!sid.count({x})){
        if (size+1<maxSize){
            sid[{x}]=sid.size();    
            size++;
        }
        else{
            s1.push_back(-x-1);
            t.clear();
            return;
        }
    }
    t.push_back(x);
    // for (auto y:t) {cout<<y<<" ";} cout<<endl;
    if (!sid.count(t)){
        if (size+(int)t.size()<maxSize) {
            sid[t]=sid.size();
            size+=t.size();
        }
        // for (auto y:t) {cout<<y<<" ";}
        // cout<<"="<<sid[t]<<endl;
        t.pop_back();
        if (!t.empty()) s1.push_back(sid[t]),t.clear();
        t.push_back(x);
    }
}

vector<int> DicCounterEncoder(
    Matrix &m,
    vector<double> &amp,
    vector<pair<double,int> > &ampCnt,
    vector<vector<int> > &seq,
    double &compRate){
    
    int n=m.row;
    // set<double> tmpSet;
    // for (int i=0;i<n;i++) tmpSet.insert(m.data[i][0]);
    // for (auto it=tmpSet.begin();it!=tmpSet.end();it++) sortedAmp.push_back(*it);

    double v=m.data[0][0];
    int cnt=1;
    map<double,int> aid;
    map<pair<double,int>,int> pid;
    vector<int> s0,s1;
    map<vector<int>,int> sid;
    int size=0;
    int maxSize=n/10;
    for (int i=1;i<n;i++){
        // cout<<m.data[i][0]<<endl;
        if (m.data[i][0]==v)
            cnt++;
        else{
            // int p=findPos(v,sortedAmp);
            int p;
            if (aid.count(v)){
                p=aid[v];
            }
            else{
                p=aid.size();
                aid[v]=aid.size();
            }
            if (cnt==1){
                // s0.push_back(p);
                lzw(p,sid,s0,s1,size,maxSize);
            }
            else{
                if (!pid.count({p,cnt})) pid[{p,cnt}]=pid.size()+n;
                // s0.push_back(pid[{p,cnt}]);
                lzw(pid[{p,cnt}],sid,s0,s1,size,maxSize);
            }
            v=m.data[i][0];
            cnt=1;
        }
    }
    int p;
    if (aid.count(v)){
        p=aid[v];
    }
    else{
        p=aid.size();
        aid[v]=aid.size();
    }
    if (cnt==1){
        // s0.push_back(p);
        lzw(p,sid,s0,s1,size,maxSize);
    }
    else{
        if (!pid.count({p,cnt})) pid[{p,cnt}]=pid.size()+n;
        // s0.push_back(pid[{p,cnt}]);
        lzw(pid[{p,cnt}],sid,s0,s1,size,maxSize);
    }
    if (!sid.count(s0)) sid[s0]=sid.size();
    s1.push_back(sid[s0]);

    int nSingle=aid.size();
    amp.resize(nSingle);
    for (auto it=aid.begin();it!=aid.end();it++) amp[it->second]=it->first;
    ampCnt.resize(pid.size());
    for (auto it=pid.begin();it!=pid.end();it++) ampCnt[it->second-n]=it->first;
    seq.resize(sid.size());
    for (auto it=sid.begin();it!=sid.end();it++) seq[it->second]=it->first;

    compRate=1.0*s1.size()*sizeof(int)/(n*sizeof(double));

    return s1;
}

Matrix DicCounterDecoder(
    int n,
    vector<int> &v,
    vector<double> &amp,
    vector<pair<double,int> > &ampCnt,
    vector<vector<int> > &seq){
    Matrix m(n,1);
    int i=0;
    double val;
    int cnt;
    for (auto x:v){
        if (x<0){
            x=-x-1;
            if (x<n) 
                val=amp[x],cnt=1;
            else val=amp[ampCnt[x-n].first],cnt=ampCnt[x-n].second;
            for (int j=0;j<cnt;j++) m.data[i++][0]=val;
            continue;
        }
        vector<int> s=seq[x];
        // cout<<"*********\n";
        for (auto y:s){
            // cout<<y<<endl;
            if (y<n) 
                val=amp[y],cnt=1;
            else val=amp[ampCnt[y-n].first],cnt=ampCnt[y-n].second;
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