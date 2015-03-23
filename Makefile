LIBS = 
CXX = icpc
CXXFLAGS = -Wall -std=c++11 -offload -O3 -mavx
CC = $(CXX)

PROG = mic_bc.out

all : $(PROG)

OBJS = Main.o ParseArgs.o Graph.o

DEPS = 

Main.o : Main.cpp ${DEPS}
ParseArgs.o: ParseArgs.cpp ${DEPS}
Graph.o : Graph.cpp

$(PROG) : $(OBJS)
	$(CC) -o $(PROG) $(OBJS) $(LIBS)
#	strip ${PROG}
clean :
	rm -f *.o *~ $(PROG)
