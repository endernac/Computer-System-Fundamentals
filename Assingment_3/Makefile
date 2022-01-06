CC= g++
CFLAGS= -g -std=c++11 -pedantic -Wall -Wextra

csim: main.o adtls.o cache.o set.o simulation.o setLRU.o setFIFO.o
	$(CC) -g -o csim main.o adtls.o cache.o set.o simulation.o setLRU.o setFIFO.o

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

adtls.o: adtls.cpp adtls.h
	$(CC) $(CFLAGS) -c adtls.cpp

simulation.o: simulation.cpp simulation.h
	$(CC) $(CFLAGS) -c simulation.cpp

cache.o: cache.cpp cache.h 
	$(CC) $(CFLAGS) -c cache.cpp

setLRU.o: setLRU.cpp setLRU.h
	$(CC) $(CFLAGS) -c setLRU.cpp

setFIFO.o: setFIFO.cpp setFIFO.h
	$(CC) $(CFLAGS) -c setFIFO.cpp

set.o: set.cpp set.h
	$(CC) $(CFLAGS) -c set.cpp

make clean:
	$ rm -f csim *.o

