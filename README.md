# MIC_BC
Betweenness centrality on Intel Xeon Phi.

##compiler options

* make -FLAGS=-DKAHAN 
	
	If you define the KAHAN macro, the bc results of each thread will be added by [KahanSum](https://en.wikipedia.org/wiki/Kahan_summation_algorithm "kahan summation") function.
	
	
## Run options

* `-i input_file` : the input file
* `-o [output_file]` : enable print result, output_file is null, print the result to the terminal.
*  `-c` : run cpu parallel function and print info.
*  `-v` : verify the result

 
