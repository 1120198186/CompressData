#include <iostream>
#include <ctime>
using namespace std;


/**
 * @brief Log start information
 * 
 * @param method the method used for simulation, i.e., qHiPSTER, SingleQE, MultiQE, QuanPath, QuanTrans
 * @param task the task to run, e.g., Grover, VQC
 */
void LogStartInfo(string method, string task);


/**
 * @brief Output time information
 * 
 * @param compTime  computing time
 * @param commTime  communication time
 * @param wholeTime whole time
 */
void LogTimeInfo(double compTime, double commTime, double wholeTime);