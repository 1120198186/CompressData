#include <chrono>
#include <iostream>

using namespace std;

class Timer {
public:
    void Start() {
        startTime = chrono::high_resolution_clock::now();
    }

    void End() {
        endTime = chrono::high_resolution_clock::now();
    }

    void ElapsedTime() {
        double duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count();
        cout << "[INFO] Elapsed time: " << duration / 1000.0 << " sec\n";
    }

private:
    chrono::time_point<chrono::high_resolution_clock> startTime;
    chrono::time_point<chrono::high_resolution_clock> endTime;
};
