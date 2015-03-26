CXX = icpc
CXXFLAGS = -Wall -std=c++11 -offload -O3 -mavx

LIBS = 

PROG = mic_bc.out

all : $(PROG)

OBJS = Main.o ParseArgs.o Graph.o TimeCounter.o CPU_BC.o MIC_BC.o

%.o: %.cpp %.h Makefile
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(PROG) : ${OBJS}
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ $(LIBS) -o $@
#	strip ${PROG}
clean :
	rm -f *.o *~ $(PROG)
