CC = g++
CFLAGS = -Wall -O3 
#CFLAGS = -g

hist: hist.o CatHistInfo.o CatTaggedHistInfo.o NumHistInfo.o NumTaggedHistInfo.o
	$(CC) $(CFLAGS) -I/usr/lib/ -lboost_regex -o hist hist.o CatHistInfo.o CatTaggedHistInfo.o NumHistInfo.o NumTaggedHistInfo.o

hist.o: hist.cpp
	$(CC) $(CFLAGS) -c hist.cpp

CatHistInfo.o: CatHistInfo.h CatHistInfo.cpp
	$(CC) $(CFLAGS) -c CatHistInfo.cpp

CatTaggedHistInfo.o: CatTaggedHistInfo.h CatTaggedHistInfo.cpp
	$(CC) $(CFLAGS) -c CatTaggedHistInfo.cpp

NumHistInfo.o: NumHistInfo.h NumHistInfo.cpp
	$(CC) $(CFLAGS) -c NumHistInfo.cpp

NumTaggedHistInfo.o: NumTaggedHistInfo.h NumTaggedHistInfo.cpp
	$(CC) $(CFLAGS) -c NumTaggedHistInfo.cpp

clean:
	rm -f *.o hist
