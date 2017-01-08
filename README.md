# MIC_BC
Betweenness centrality on Intel Processors (including Intel Xeon Phi 1st gen(*KNC*), 2nd gen(*KNL*) and Xeon CPU).

##Compiling

This project uses CMake and Intel Compiler as compile toolchain.

Please follow following steps:
* make a build folder: `mkdir build && cd build`
* generate Makefiles by using CMake (using Intel compiler): `cmake -D CMAKE_C_COMPILER=icc -D CMAKE_CXX_COMPILER=icpc ..`
* compile: `make -j`

*Notice:* If you are compiling this project on KNL (2nd generation Intel Xeon Phi) platform, please add compiler definition *`-DKNL`* in `CMakeLists.txt` file.
	
## Running options

* `-i input_file` : the input graph file
* `-f running_mode_flag`

We use a bitwise flag to control the running mode, different running modes can be add up with bitwise `|` operation:

### 1st generation Xeon Phi and CPU
* mode: 0x1 task: Naive CPU
* mode: 0x2 task: Parallel CPU
* mode: 0x4 task: Parallel CPU with 1-deg vertices reduction
* mode: 0x8 task: Xeon Phi Offload
* mode: 0x10 task: Xeon Phi Offload with 1-deg vertices reduction
* mode: 0x20 task: Xeon Phi Offload with edge&vertices traversal enabled
* mode: 0x80 task: Verify the results


*example:* if you want to run this program in mode `0x2`, `0x10`  and `0x20`:
`./mic_bc -i somegraph.graph -f 0x32`, here `0x32` equals to `0x2 | 0x10 | 0x20`.

### 2nd generation Xeon Phi (KNL) platform

* mode: `0x1` task: Naive KNL
* mode: `0x8` task: Parallel KNL
* mode: `0x10` task: Parallel KNL with 1-deg vertices reduction
* mode: `0x20` task: Parallel with edge&vertices traversal enabled
* mode: `0x80` task: Verify the results


 
