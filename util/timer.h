#pragma once

#include <chrono>
#include <iostream>

using namespace std;

class Timer {
public:
    void Start();

    void End();

    double ElapsedTime();

private:
    chrono::time_point<chrono::high_resolution_clock> startTime;
    chrono::time_point<chrono::high_resolution_clock> endTime;
};
