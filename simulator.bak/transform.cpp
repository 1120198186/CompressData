// 
// The circuit transformation algorithm for QuanTrans
// 

#include "qcircuit.h"

//
// Build Q-graph
//

/**
 * @brief Build Q-graph for C[0:T-1]
 * 
 * @param qc    a quantum circuit
 * 
 * @return Q-graph
 */
vector<vector<int>> BuildQGraph(const QCircuit& qc) {
    vector<vector<int>> Q(qc.numQubits, vector<int>(qc.numQubits, 0));
    for (int j = 0; j < qc.numDepths; ++ j) {
        for (int i = 0; i < qc.numQubits; ++ i) {
            if (qc.gates[j][i].is_target_gate_) {
                ++ Q[qc.gates[j][i].control_qubit_][qc.gates[j][i].target_qubit_];
            }
        }
    }
    return Q;
}

void Print2DVector(const vector<vector<int>>& P) {
    for (const auto& vec1 : P) {
        for (const auto& value : vec1) {
            cout << value << " ";
        }
        cout << endl;
    }
}

void Print3DVector(const vector<vector<vector<int>>>& P){
    for (const auto& vec1 : P) {
        for (const auto& vec2 : vec1) {
            for (const auto& value : vec2) {
                cout << value << " ";
            }
            cout << "\t\t";
        }
        cout << endl;
    }
}

/**
 * @brief Remove a level of gates from qc
 * 
 * @param Q         Q-graph
 * @param qc        a quantum circuit
 * @param level     the level to be removed
 * 
 * @return true if an edge is removed from Q-graph, else false
 */
bool RemoveEdge(vector<vector<int>>& Q, const QCircuit& qc, int level) {
    int ret;
    bool UGraphChanged = false;
    for (int i = 0; i < qc.numQubits; ++ i) {
        if (qc.gates[level][i].is_target_gate_) {
            ret = (-- Q[qc.gates[level][i].control_qubit_][qc.gates[level][i].target_qubit_]);
            if (ret == 0) {
                UGraphChanged = true;
            }
        }
    }
    return UGraphChanged;
}

//
// Build U-graph
//

// 
// Strongly connected component, the 2-pass-DFS implementation
// 

void dfs1(const vector<vector<int>>& graph, vector<int>& visited, stack<int>& topo, int u) {
    visited[u] = 1;
    for (vector<int>::size_type v = 0; v < graph[u].size(); ++ v) { // numQubits
        if (graph[u][v] > 0 && ! visited[v]) {
            dfs1(graph, visited, topo, v);
        }
    }
    // if all vertices starting from u have been visited
    // stack up and return; topological order: from top to bottom of stack
    topo.push(u);
}

void dfs2(const vector<vector<int>>& graphT, vector<int>& visited, int u, vector<int>& uc) {
    uc.push_back(u);
    visited[u] = 1;
    for (vector<int>::size_type v = 0; v < graphT[u].size(); ++ v) {
        if (graphT[u][v] > 0 && ! visited[v]) {
            dfs2(graphT, visited, v, uc);
        }
    }
}

vector<vector<int>> SCC(const vector<vector<int>>& graph) {
    int numQubits = graph.size();
    vector<vector<int>> graphT(numQubits, vector<int>(numQubits, 0));

    // Transpose the graph
    for (int i = 0; i < numQubits; ++i) {
        for (int j = 0; j < numQubits; ++j) {
            graphT[i][j] = graph[j][i];
        }
    }

    vector<int> visited(numQubits, 0);
    stack<int> topo;
    vector<vector<int>> UCs;

    // Run the first DFS on the graph to construct topological order
    for (int u = 0; u < numQubits; ++ u) {
        if (! visited[u]) {
            dfs1(graph, visited, topo, u);
        }
    }

    visited.assign(numQubits, 0);

    // Run the second DFS on the transposed graph to construct strongly connected components
    while (! topo.empty()) {
        int u = topo.top();
        topo.pop();
        if (! visited[u]) {
            vector<int> uc;
            dfs2(graphT, visited, u, uc);
            UCs.push_back(uc);
        }
    }

    return UCs;
}

int GetUCId(const vector<vector<int>>& UC, int qid) {
    for (vector<int>::size_type i = 0; i < UC.size(); ++i) {
        for (vector<int>::size_type j = 0; j < UC[i].size(); ++j) {
            if (UC[i][j] == qid) {
                return i;
            }
        }
    }
    return -1;
}

/**
 * @brief
 * 
 * @param
 * @param
 * 
 * @return
 */
vector<vector<int>> BuildUCParent(const vector<vector<int>>& Q, const vector<vector<int>>& UCs) {
    int numQubits = Q.size();
    vector<vector<int>> parent(UCs.size());

    for (int i = 0; i < numQubits; ++ i) {
        for (int j = 0; j < numQubits; ++ j) {
            if (Q[i][j] > 0) {
                int uc_i = GetUCId(UCs, i);
                int uc_j = GetUCId(UCs, j);
                if (uc_i != uc_j) {
                    bool already_added = false;
                    for (vector<int>::size_type k = 0; k < parent[uc_j].size(); ++k) {
                        if (parent[uc_j][k] == uc_i) {
                            already_added = true;
                            break;
                        }
                    }
                    if (! already_added) {
                        parent[uc_j].push_back(uc_i);
                    }
                }
            }
        }
    }
    return parent;
}

// 
// Build P-table
// 

/**
 * @brief Get a valid P0 for Q-graph and h
 * 
 * @param
 * @param
 * 
 * @return
 */
vector<int> GetValidP0(const vector<vector<int>>& Q, int h) {
    vector<int> p0;
    vector<int> p0_qubit;
    vector<int> ucSize;
    vector<vector<int>> ucParent;

    // build U-graph by SCC on Q-graph ///////////////////////
    auto UCs = SCC(Q);

    // get the sizes of each UC
    for (vector<int>::size_type i = 0; i < UCs.size(); ++ i) {
        ucSize.push_back(UCs[i].size());
    }

    // get parents of each UC
    ucParent = BuildUCParent(Q, UCs);

    // build dp table ////////////////////////////////////////
    int numUCs = UCs.size();

    vector<vector<vector<vector<int>>>> dp(2, vector<vector<vector<int>>>(h + 1));
    int last = 0, curr = 1;

    for (int i = 1; i <= numUCs; ++ i) {
        dp[curr] = dp[last];

        int uc_id = i - 1;
        if (ucParent[uc_id].empty() && ucSize[uc_id] <= h) {
            dp[curr][ucSize[uc_id]].push_back({uc_id});
        }

        for (int j = ucSize[uc_id] + 1; j <= h; ++ j) {
            for (vector<int>::size_type t = 0; t < dp[last][j - ucSize[uc_id]].size(); ++ t) {
                if (includes(dp[last][j - ucSize[uc_id]][t].begin(), dp[last][j - ucSize[uc_id]][t].end(), ucParent[uc_id].begin(), ucParent[uc_id].end())) {
                    dp[curr][j].push_back(dp[last][j - ucSize[uc_id]][t]);
                    dp[curr][j].back().push_back(uc_id);
                }
            }
        }

        last ^= 1;
        curr ^= 1;
    }

    if (dp[last][h].empty()) { // emptyset
        // cout << "No valid P0" << endl;
        return p0_qubit;
    }

    p0 = dp[last][h][0];

    // extract qubits from P0 ////////////////////////////////

    for (vector<int>::size_type i = 0; i < p0.size(); ++ i){
        // the current uc id: p0[i]
        for (int j = 0; j < ucSize[p0[i]]; ++ j) {
            p0_qubit.push_back(UCs[p0[i]][j]);
        }
    }

    return p0_qubit;
}

/**
 * @brief Build P-table for C[0:T-1]
 * 
 * @param qc    a quantum circuit
 * @param h     #high-order qubits
 * 
 * @return P-table
 */
vector<vector<vector<int>>> BuildPTable(const QCircuit& qc, int h) {
    vector<vector<vector<int>>> P(qc.numDepths, vector<vector<int>>(qc.numDepths));

    vector<int> p0;

    vector<vector<int>> Q = BuildQGraph(qc);

    // cout << "[DEBUG] Q-graph" << endl;
    // Print2DVector(Q);

    bool success = false;
    bool UGraphChanged = true;

    int cnt = 0;

    // construct each cell of the P-table from top to bottom and from right to left
    for (int i = 0; i < qc.numDepths; ++ i) {
        // ===== P[i][numDepths-1] =====
        // rebuild Q-graph
        if (i != 0) { // about to calculate P[i:], remove the (i-1)-th level
            UGraphChanged = RemoveEdge(Q, qc, i-1);
        }

        // inherit from the upper grid
        if (P[i][qc.numDepths-1].size() > 0) {
            success = true; // for leftward propagation
            if (i + 1 < qc.numDepths) {
                P[i+1][qc.numDepths-1] = P[i][qc.numDepths-1]; // downward propagation
            }
        } else {
            success = false;
            if (UGraphChanged) { // rerun p0-finding
                p0 = GetValidP0(Q, h);
                cnt ++;

                P[i][qc.numDepths-1] = p0;
                if (p0.size() > 0) {
                    success = true; // for leftward propagation
                    if (i + 1 < qc.numDepths) {
                        P[i+1][qc.numDepths-1] = p0; // downward propagation
                    }
                }
            }
            // else: U[i-1:] = U[i:] = emptyset
        }

        // ===== P[i][numDepths-2 ~ i] =====
        auto Q_tmp = Q; // from C[i:]

        for (int j = qc.numDepths-2; j >= i; -- j) { // P[i][j]

            // inherit from the right grid
            if (success) {
                P[i][j] = P[i][j+1];
            // inherit from the upper grid
            } else if (P[i][j].size() > 0) {
                success = true; // for leftward propagation
                if (i + 1 < qc.numDepths) { // downward propagation
                    P[i+1][j] = P[i][j];
                }
            } else {
                // rebuild Q-graph, remove the last level
                UGraphChanged = RemoveEdge(Q_tmp, qc, j+1);
                if (UGraphChanged) {
                    p0 = GetValidP0(Q_tmp, h);
                    cnt ++;

                    P[i][j] = p0;
                    if (p0.size() > 0) {
                        success = true; // for leftward propagation
                        if (i + 1 < qc.numDepths) {
                            P[i+1][j] = p0; // downward propagation
                        }
                    }
                }
                // else: U[i:j] = U[i:j+1] = emptyset
            }
        }
    }
    // cout << "[DEBUG] P-table calculate times: " << cnt << endl;
    return P;
}

// 
// Build T-table
// 

#include <iostream>
#include <vector>
#include <algorithm>

int numSwap(const vector<int>& p_end, const vector<int>& p_begin) {
    vector<int> diff;
    set_difference(p_end.begin(), p_end.end(), p_begin.begin(), p_begin.end(), back_inserter(diff));
    int numSwap = diff.size();
    return numSwap;
}

vector<vector<int>> BuildTTable(const vector<vector<vector<int>>>& P) {
    int numDepths = P.size();
    vector<vector<pair<int, int>>> T(numDepths, vector<pair<int, int>>(numDepths, make_pair(0, 0)));
    vector<vector<int>> S(numDepths, vector<int>(numDepths, -1));
    vector<vector<vector<int>>> P_begin(numDepths, vector<vector<int>>(numDepths));
    vector<vector<vector<int>>> P_end(numDepths, vector<vector<int>>(numDepths));

    if (P[0][numDepths-1].size() > 0) {
        // cout << "[DEBUG] T[0][w-1]: (0, 0)" << endl;
        return S;
    }

    for (int i = 0; i < numDepths; ++ i) { // sub-circuit of depth 1
        P_begin[i][i] = P_end[i][i] = P[i][i];
    }

    for (int depth = 2; depth <= numDepths; ++ depth) {
        for (int i = 0; i < numDepths - depth + 1; ++ i) { // sub-circuit C[i:j]
            int j = i + depth - 1;
            if (P[i][j].size() > 0) {
                P_begin[i][j] = P_end[i][j] = P[i][j];
            } else {
                T[i][j] = make_pair(99999, 99999);
                for (int k = i; k < j; ++ k) {
                    pair<int, int> tmp = make_pair(T[i][k].first + T[k + 1][j].first + 1, T[i][k].second + T[k + 1][j].second + numSwap(P_end[i][k], P_begin[k + 1][j]));
                    if (tmp < T[i][j]) {
                        T[i][j] = tmp;
                        S[i][j] = k;
                        P_begin[i][j] = P_begin[i][k];
                        P_end[i][j] = P_end[k + 1][j];
                    }
                }
            }
        }
    }

    // cout << "[DEBUG] T[0][w-1]: (" << T[0][numDepths-1].first << ", " << T[0][numDepths-1].second << ")" << endl;

    // cout << "[DEBUG] T-table" << endl;
    // for (const auto& vec1 : T) {
    //     for (const auto& value : vec1) {
    //         cout << "(" << value.first << ", " << value.second << ")\t\t";
    //     }
    //     cout << endl;
    // }

    return S;
}

void GetOptimalSplit(vector<string>& res, const vector<vector<int>>& S, int i, int j) {
    if (S[i][j] == -1) {
        res.push_back(to_string(i));
        res.push_back("-");
        res.push_back(to_string(j));
    } else {
        GetOptimalSplit(res, S, i, S[i][j]);
        res.push_back("|");
        GetOptimalSplit(res, S, S[i][j] + 1, j);
    }
}

// 
// Main function
// 
void Transform(const QCircuit& qc, int h) {
    // Build P-table for the entire circuit
    auto P = BuildPTable(qc, h);

    // cout << "[DEBUG] P-table" << endl;
    // for (const auto& vec1 : P) {
    //     for (const auto& vec2 : vec1) {
    //         for (const auto& value : vec2) {
    //             cout << value << " ";
    //         }
    //         cout << "\t\t";
    //     }
    //     cout << endl;
    // }

    // Build T-table for the entire circuit, get optimal slicing position table S
    auto S = BuildTTable(P);

    // Recover the optimal slicing policy from S
    vector<string> res;
    GetOptimalSplit(res, S, 0, qc.numDepths-1);

    cout << "[DEBUG] optimal slicing policy: ";
    for (auto & e : res) {
        cout << e << " ";
    }
    cout << endl;
}

int main(int argc, char** argv) {
    if (argc != 5) {
        cout << "[ERROR] usage: cmd <numQubits> <numDepths> <h> <epoch>" << endl;
        exit(1);
    }

    srand((int)time(0));

    double sum = 0.0;
    int numQubits = atoi(argv[1]); 
    int numDepths = atoi(argv[2]);
    int h = atoi(argv[3]);
    int epoch = atoi(argv[4]);

    QCircuit qc = QCircuit(numQubits);
    qc.setDepths(numDepths);

    for (int eph = 0; eph < epoch; ++ eph) {
        qc.fill(0.1);
        // auto qc = Random(numQubits, numDepths);
        // auto qc = test();
        // qc.print();

        auto start = chrono::high_resolution_clock::now();
        Transform(qc, h);
        auto end = chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
        sum += duration;

        cout << "[INFO] QuanTrans preprocessing time: " << duration << " (sec)" << endl;
    }

    cout << "[INFO] numQubits: " << numQubits << " numDepths: " << numDepths << endl;
    cout << "[INFO] Average QuanTrans preprocessing time: " << sum / epoch << " (sec)" << endl;

    return 0;
}