CC=g++
CFLAGS=-std=c++11
INCLUDEBCDPP=-I ../include 

all: main.o ../data_structures/message.o ../include/FCFS.o ../include/PriorityQueue.o
	$(CC) -g -o SIM main.o ../data_structures/message.o ../include/FCFS.o ../include/PriorityQueue.o

main.o: main.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDEBOOST) $(INCLUDEBCDPP) main.cpp -o main.o

../data_structures/message.o: 
	$(CC) -g -c $(CFLAGS) $(INCLUDEBOOST) $(INCLUDEBCDPP) ../data_structures/message.cpp -o ../data_structures/message.o

../include/FCFS.o: 
	$(CC) -g -c $(CFLAGS) $(INCLUDEBOOST) $(INCLUDEBCDPP) ../include/FCFS.cpp -o ../include/FCFS.o
	
../include/PriorityQueue.o: 
	$(CC) -g -c $(CFLAGS) $(INCLUDEBOOST) $(INCLUDEBCDPP) ../include/PriorityQueue.cpp -o ../include/PriorityQueue.o

clean:
	rm -f SIM *.o *~ 
	-for d in ../data_structures; do (cd $$d; rm -f *.o *~ ); done
	-for d in ../include; do (cd $$d; rm -f *.o *~ ); done
