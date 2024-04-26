# CompressData
## 完成进度（240409）

目前主函数在`main/main.cpp`里，生成了一个随机量子线路`qc`，然后调用不同的模拟方法去模拟这个线路​​。模拟方法包括：

1. 最基础的indexing算法，可以用来检查实验结果（**已完成**）

   `simulator/SVSim.[h/cpp]`

2. 基于Repeat counter的压缩模拟（张）

   `simulator/RepeatCounter.[h/cpp]`

3. 基于字典+counter的压缩模拟（堵）

   `simulator/DicCounter.[h/cpp]`

4. 基于分块的无压缩模拟（徐）（**已完成**）

   `simulator/BlockSVSim.[h/cpp]`

   - [x] single qubit gates
   - [x] 2-qubit controlled gates
   - [x] 2-qubit non-controlled gates in either high-order or low-order
   - [ ] 2-qubit non-controlled gates that causes inseprable levels

5. 基于QuanPath分块的无压缩模拟（**已完成**）

   `simulator/HybridSVSim.[h/cpp]`

   - [x] 块的内外存交换
   - [x] 逐个载入块到内存，执行单块内的indexing计算（在`LocalComputing`中已经实现）
   - [x] 最后的Merge操作
   - [ ] 用线程计算高位操作矩阵

6. 对于分块BDD压缩+indexing来说，需要实现（可以写到`util/compress.h`文件里）（**暂时放弃**）
   - [ ] 对`localSv`初始态的压缩
   - [ ] 计算压缩率
   - [x] 需要实现块的定位（找到参与indexing的块）
   - [ ] 参与运算的块的解压缩
   - [ ] 多块参与的indexing运算（在`LocalComputing`中实现了一个单块indexing，可以参考）
   - [ ] 重新压缩（和初始态压缩用的是同一个函数）
   - [ ] 如果压缩率变低，需要交换一些块到外存，这个可以晚点再考虑


## 代码运行（Windows）

目前的`Makefile`文件用于编译`main/main.cpp`文件到`obj/main.exe`，`util/*`和`simulator/*`中的依赖文件都会包括进去。

编译并执行的指令为：

```shell
make
make cleanssd
.\obj\main.exe <numQubits> <memQubits> <numDepths>
python main/merge.py
make cleanssd && make && .\obj\main.exe 10 16 5 && python main\merge.py
```

其中，`numQubits`用于指定随机生成的线路的量子比特个数；`memQubits`说明内存最多能存下的量子比特数量，存不下的需要分块；`numDepths`说明线路深度（层数）。
