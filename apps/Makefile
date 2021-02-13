CC:=gcc
CFLAGS:=-g -Wall -Werror

TESTFILES := $(wildcard test*.c) 
$(info TESTFILES are $(TESTFILES))

TESTS := $(TESTFILES:%.c=%)
$(info TESTS are $(TESTS))

all: test01

test01: test01.c ../disk/disk.c ../disk/disk.h ../io/File.c ../io/File.h
	$(CC) $(CFLAGS) -o test01 test01.c ../disk/disk.c ../io/File.c -lm


.PHONY: clean

clean:
	rm -rf *.o
	rm -rf *.dSYM
	rm -f test01
	rm -f ../disk/disk ../io/File