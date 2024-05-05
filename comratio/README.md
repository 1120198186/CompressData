# State Vector Simulation Using Data Compression

## In-memory Block-based Simulation Using Offline Compressors

### SC19: QPress

SC19提出的压缩器代码在`~/compressed_qcsim`文件夹下，目前文章中命名为`QPress`。可以执行以下命令收集压缩率，使用的线路是`RandomRandom`。

```
cd ~/compressed_qcsim/tests
./run_and_get_worst_comprate.sh <numQubits=20> <numGates=20>
cd ~/CompressData/comratio
```

实验结果会复制到`~/CompressData/comratio/ratioQPress.txt`中，
该文件记录了每个gate执行完毕后的压缩率，最后两行是统计的最坏压缩率。
如果压缩率中出现了科学计数法，最坏压缩率的统计会出问题。
目前记录的是`<numQubits=20> <numGates=20>`的压缩情况，如需要可以调整参数。

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