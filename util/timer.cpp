#include "timer.h"

void Timer::Start() {
    startTime = chrono::high_resolution_clock::now();
}

void Timer::End() {
    endTime = chrono::high_resolution_clock::now();
}

double Timer::ElapsedTime() {
    double duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count();
    // cout << "[INFO] Elapsed time: " << duration / 1000.0 << " sec\n";
    return duration;
}