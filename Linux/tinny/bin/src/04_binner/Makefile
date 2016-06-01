CC = g++
CFLAGS = -Wall -O3 
#CFLAGS = -g

binner: main.o EqualWidthBinner.o EqualVarsFreqBinner.o EqualBadsFreqBinner.o
	$(CC) $(CFLAGS) -I/usr/lib/ -lboost_regex -o binner main.o EqualWidthBinner.o EqualVarsFreqBinner.o EqualBadsFreqBinner.o

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

EqualWidthBinner.o: EqualWidthBinner.h EqualWidthBinner.cpp
	$(CC) $(CFLAGS) -c EqualWidthBinner.cpp

EqualVarsFreqBinner.o: EqualVarsFreqBinner.h EqualVarsFreqBinner.cpp
	$(CC) $(CFLAGS) -c EqualVarsFreqBinner.cpp

EqualBadsFreqBinner.o: EqualBadsFreqBinner.h EqualBadsFreqBinner.cpp
	$(CC) $(CFLAGS) -c EqualBadsFreqBinner.cpp

clean:
	rm -f *.o binner
