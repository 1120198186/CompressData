#include "block.h"

double InitStateVectorSSD(long long N, long long numFiles, string dir) {
    Timer timer;
    timer.Start();
    stringstream filenameStream;
    long long fileSize = N / numFiles;

    for (long long i = 0; i < numFiles; ++ i) {
        filenameStream.str(""); // clear the stream
        filenameStream << dir << i;
        ofstream file(filenameStream.str()); // open the file

        for (long long j = 0; j < fileSize; ++ j) {
            if (i == 0 && j == 0) {
                file << 1 << endl;
            } else {
                file << 0 << endl;
            }
        }
    }
    timer.End();
    return timer.ElapsedTime(); // I/O time (sec)
}


double InitRandomStateVectorSSD(long long N, long long numFiles, string dir) {
    Timer timer;
    timer.Start();
    stringstream filenameStream;
    long long fileSize = N / numFiles;

    for (long long i = 0; i < numFiles; ++ i) {
        filenameStream.str(""); // clear the stream
        filenameStream << dir << i;
        ofstream file(filenameStream.str()); // open the file

        for (long long j = 0; j < fileSize; ++ j) {
            file << (double)rand() / RAND_MAX << endl;
        }
    }
    timer.End();
    return timer.ElapsedTime(); // I/O time
}


double ReadBlock(Matrix &localSv, long long blkNo, long long fileCnt, string dir) {
    Timer timer;
    timer.Start();
    long long filename;
    stringstream filenameStream;
    ifstream file;
    long long fileSize = localSv.row / fileCnt; // the number of amplitudes within each file

    for (long long fileNo = 0; fileNo < fileCnt; ++ fileNo) {
        // calculate the filename
        filename = blkNo * fileCnt + fileNo;

        // open the file
        filenameStream.str(""); // clear the stream
        filenameStream << dir << filename; // << ".txt";
        file.open(filenameStream.str());

        // read the file
        for (long long i = 0; i < fileSize; ++ i) {
            file >> localSv.data[fileNo * fileSize + i][0];
        }
        file.close();
    }
    timer.End();
    return timer.ElapsedTime(); // I/O time
}


double ReadMergeBlock(Matrix &localSv, long long mergeNo, long long H, string dir) {
    Timer timer;
    timer.Start();
    long long filename;
    stringstream filenameStream;
    ifstream file;
    long long fileSize = localSv.row / H; // the number of amplitudes within each file

    for (long long blkNo = 0; blkNo < H; ++ blkNo) {
        // calculate the filename
        // 0 * H + 0, 1 * H + 0, 2 * H + 0, ..., (H-1) * H + 0
        filename = blkNo * H + mergeNo;

        // open the file
        filenameStream.str(""); // clear the stream
        filenameStream << dir << filename; // << ".txt";
        file.open(filenameStream.str());

        // read the file
        for (long long i = 0; i < fileSize; ++ i) {
            file >> localSv.data[blkNo * fileSize + i][0];
        }
        file.close();

        // remove the file
        remove(filenameStream.str().c_str());
    }
    timer.End();
    return timer.ElapsedTime(); // I/O time
}


double WriteBlock(Matrix &localSv, long long blkNo, long long fileCnt, string dir) {
    Timer timer;
    timer.Start();
    long long filename;
    stringstream filenameStream;
    ofstream file;
    long long fileSize = localSv.row / fileCnt; // the number of amplitudes within each file

    for (long long fileNo = 0; fileNo < fileCnt; ++ fileNo) {
        // calculate the filename
        filename = blkNo * fileCnt + fileNo;

        // open the file
        filenameStream.str(""); // clear the stream
        filenameStream << dir << filename; // << ".txt";
        file.open(filenameStream.str());

        // write the file
        for (long long i = 0; i < fileSize; ++ i) {
            file << localSv.data[fileNo * fileSize + i][0] << endl;
        }
        file.close();
    }
    timer.End();
    return timer.ElapsedTime(); // I/O time
}
