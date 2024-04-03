# CompressData
## 完成进度

目前indexing的主函数在`simulator/QuanPart.cpp`里，目前高位量子比特个数设置的是$h=0$，分块数$H=2^h=1$，也就是不分块。后续要分块就是改变$h$的值。`localSv`代表一块的状态向量，大小是$2^{n-h}$​。

1. 对于分块BDD压缩+indexing来说，需要实现（可以写到`util/compress.h`文件里）
   - [ ] 对`localSv`初始态的压缩
   - [ ] 计算压缩率
   - [ ] 需要实现块的定位（找到参与indexing的块）
   - [ ] 参与运算的块的解压缩
   - [ ] 多块参与的indexing运算（在`LocalComputing`中实现了一个单块indexing，可以参考）
   - [ ] 重新压缩（和初始态压缩用的是同一个函数）
   - [ ] 如果压缩率变低，需要交换一些块到外存，这个可以晚点再考虑
2. 对于QuanPath分块不压缩+indexing来说，需要实现
   - [ ] 块的内外存交换
   - [x] 逐个载入块到内存，执行单块内的indexing计算（在`LocalComputing`中已经实现）
   - [ ] 最后的Merge操作

## 代码运行

目前的`Makefile`文件用于编译`simulator/QuanPart.cpp`文件到`obj/QuanPart.exe`，`util/*`中的依赖文件都会包括进去。

编译并执行的指令为：

```shell
make
.\obj\QuanPart.exe <numQubits> <numDepths>
```

