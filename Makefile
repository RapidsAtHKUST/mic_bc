CXX = icpc
CXXFLAGS = -Wall -std=c++11 -offload -O2 -fopenmp -march=native 
CXXFLAGS+= $(FLAGS)
#CXXFLAGS+= -fp-model fast=2


LIBS = 
LDFLAGS = -offload-option,mic,compiler,"-z defs -lpthread"
PROG = mic_bc.out

all : $(PROG)

OBJS = Main.o ParseArgs.o Graph.o GraphUtility.o TimeCounter.o CPU_BC.o MIC_BC.o MIC_Calc_Function.o Utils.o

%.o: %.cpp %.h Makefile
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(PROG) : ${OBJS}
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ $(LIBS) -o $@
#	strip ${PROG}
clean :
	rm -f *.o *~ $(PROG)
