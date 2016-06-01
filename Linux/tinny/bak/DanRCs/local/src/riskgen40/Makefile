SHELL = /bin/sh

CC = gcc
CXX = g++
LDFLAGS = -lm

OBJs = risktabgen40.o split.o unpack_zips.o
OBJ_stats = stats.o

all : risktabgen40.exe stats.exe

stats.exe : ${OBJ_stats}
	${CXX} -o $@ $^

stats.o : stats.hpp

risktabgen40.exe : ${OBJs}
	 ${CC} -o $@ ${LDFLAGS} $^

split.o : split.h
unpack_zips.o : unpack_zips.h

.PHONY : clean
clean :
	 -rm risktabgen40.exe ${OBJs}
