# State Vector Simulation Using Data Compression

## In-memory Block-based Simulation Using Offline Compressors

### SC19: QPress

```
cd ~/compressed_qcsim/tests
./run_and_get_worst_comprate.sh <numQubits> <numGates>
cd ~/CompressData/comratio
```

实验结果会复制到`~/CompressData/comratio/ratioQPress.txt`中，该文件记录了每个gate执行完毕后的压缩率，并且最后两行是统计的最坏压缩率。

### SZ, ZFP, FPZIP

```
# activate the virtual environment for libpressio
spack env activate compress

./cratio.sh
```

Other useful spack commands. 

```
# check the packages in this environment
spack find

# add new packages to the environment
spack add xx

# deactivate the virtual environment
spack env deactivate
```