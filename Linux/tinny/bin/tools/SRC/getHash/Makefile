SHELL= /bin/sh
LINT= lclint
CC= gcc
COMPILE.c= $(CC) $(CFLAGS) $(CPPFLAGS) -c
LINT.c= $(LINT) $(LINTFLAGS) $(CPPFLAGS)
LINK.c= $(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS)
SUFFIXES= .o .c
CFLAGS= -Wall -O


all: getHashValues hashValue_filter

hashValue.h: hashValue.o getHashValues.o hashValue_filter.o

getHashValues: getHashValues.c hashValue.o
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ getHashValues.c hashValue.o $(LDLIBS)

hashValue_filter: hashValue_filter.c hashValue.o
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ hashValue_filter.c hashValue.o $(LDLIBS)


.SUFFIXES:
.SUFFIXES: .o .c

.c.o:
	$(COMPILE.c) $(OUTPUT_OPTION) $<
.c:
	$(LINK.c) -o $@ $< $(LDLIBS)

clean:
	rm *.o


