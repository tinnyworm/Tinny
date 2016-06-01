CC = g++
CFLAGS = -Wall -O3 
#CFLAGS = -g

check_dqr: check_dqr.cpp
	$(CC) $(CFLAGS) -I/usr/lib/ -lboost_regex -o check_dqr check_dqr.cpp

clean:
	rm -f *.o check_dqr
