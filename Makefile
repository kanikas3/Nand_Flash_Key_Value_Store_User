CC=gcc
CFLAGS=-Wall -Werror
LDFLAGS=
TARGET=user@192.168.53.89:~

all: kvlib.o testbench

kvlib.o: kvlib.c
	$(CC) $(CFLAGS) -c $^ -Wall -o $@ $(LDFLAGS)
	
testbench: testbench.c kvlib.o 
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

install: all
	scp -r testbench $(TARGET)

clean:
	rm -rf *.o testbench
