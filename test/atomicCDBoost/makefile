CC=g++-4.9
CFLAGS=-std=c++11
#INCLUDEBOOST=-I ../../../../boost_1_61_0 
#IF YOU INSTALLED BOOST FOLLOWING OPTION 2, UNCOMMENT LINE 3 (DELETE #) AND UPDATE THE RELATIVE PATH TO YOUR BOOST LIBRARY
INCLUDEBCDPP=-I ../../../../cdboost/include 
#IN LINE 5, UPDATE THE RELATIVE PATH TO cdbost/include

all: main.o ../../data_structures/message.o 
	$(CC) -g -o MODELNAME main.o ../../data_structures/message.o  
#CHANGE MODELNAME TO THE NAME OF YOUR MODEL

main.o: main.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDEBOOST) $(INCLUDEBCDPP) main.cpp -o main.o

../../data_structures/message.o: 
	$(CC) -g -c $(CFLAGS) $(INCLUDEBOOST) $(INCLUDEBCDPP) ../../data_structures/message.cpp -o ../../data_structures/message.o

clean:
	rm -f MODELNAME *.o *~ 
	#CHANGE MODELNAME TO THE NAME OF YOUR MODEL
	-for d in ../../data_structures; do (cd $$d; rm -f *.o *~ ); done\

#IF YOU USE THE FOLDER STRUCTURE PROVIDED, YOU DO NOT NEED TO CHANGE ANYTHING ELSE
