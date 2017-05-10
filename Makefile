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
	scp -r testbench test.sh short_test.sh large_sample.sh small_sample.sh launch_flash_simulator.sh $(TARGET)

clean:
	rm -rf *.o testbench
