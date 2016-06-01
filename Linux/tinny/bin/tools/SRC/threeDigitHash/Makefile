SHELL= /bin/sh
LINT= lclint
CC= gcc
COMPILE.c= $(CC) $(CFLAGS) $(CPPFLAGS) -c
LINT.c= $(LINT) $(LINTFLAGS) $(CPPFLAGS)
LINK.c= $(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS)
SUFFIXES= .o .c
CFLAGS= -Wall -O


all: getThreeDigitHash

getThreeDigitHash: getThreeDigitHash.c threeDigitHashValue.o
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ getThreeDigitHash.c threeDigitHashValue.o $(LDLIBS)

# hashValue_filter: hashValue_filter.c hashValue.o
# 	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ hashValue_filter.c hashValue.o $(LDLIBS)


.SUFFIXES:
.SUFFIXES: .o .c

.c.o:
	$(COMPILE.c) $(OUTPUT_OPTION) $<
.c:
	$(LINK.c) -o $@ $< $(LDLIBS)




