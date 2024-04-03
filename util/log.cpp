#include "log.h"

/**
 * @brief Log start information
 * 
 * @param method the method used for simulation, i.e., qHiPSTER, SingleQE, MultiQE, QuanPath, QuanTrans
 * @param task the task to run, e.g., Grover, VQC
 */
void LogStartInfo(string method, string task) {
    // get current time
    time_t currentTime = time(nullptr);
    // transform to local time
    tm* localTime = localtime(&currentTime);
    
    // log method, task, time
    cout << "[INFO] [" << method << "] [" << task << "] " << asctime(localTime);
    
    return;
}

/**
 * @brief Output time information
 * 
 * @param compTime  computing time
 * @param commTime  communication time
 * @param wholeTime whole time
 */
void LogTimeInfo(double compTime, double commTime, double wholeTime) {
    cout << "\tcomputing time:  " << compTime << endl;
    // cout << "\tcommunication time:  " << commTime << endl;
    cout << "\twhole time:  " << wholeTime << endl << endl;
    return;
}