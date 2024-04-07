//
// Partition the state vector and SSD IO
//

#ifndef BLOCK_H
#define BLOCK_H

#include "global_notation.h"

/**
 * @brief Initilize the state vector
 * 
 * @param N        the size of the state vector
 * @param numFiles the number of blocks
 * @param dir      the directory of the state vector
 */
void InitStateVectorSSD(long long N, long long numFiles, string dir="./output/");

/**
 * @brief Read a block from the SSD
 * 
 * @param localSv the local state vector
 * @param blkNo   the block number
 * @param fileCnt the number of files within each block
 * @param dir     the directory of the state vector
 */
void ReadBlock(Matrix &localSv, long long blkNo, long long fileCnt, string dir="./output/");

/**
 * @brief Read a block for merge operation from SSD
 * 
 * @param localSv the local state vector
 * @param mergeNo the merge block number
 * @param H       the number of blocks
 * @param dir     the directory of the state vector
 */
void ReadMergeBlock(Matrix &localSv, long long mergeNo, long long H, string dir="./output/");

/**
 * @brief Write a block from the SSD
 * 
 * @param localSv the local state vector
 * @param blkNo   the block number
 * @param fileCnt the number of files within each block
 * @param dir     the directory of the state vector
 */
void WriteBlock(Matrix &localSv, long long blkNo, long long fileCnt, string dir="./output/");

#endif // BLOCK_H