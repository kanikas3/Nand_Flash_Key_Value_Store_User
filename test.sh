#!/bin/bash

if [ "$#" -ne 3 ]; then
	echo "Illegal number of parameters"
	echo "./test.sh <test_number> <num_blocks> <is_large_key>"
	echo "test_number: "
	echo "    1 for read-write test"
	echo "    2 for delete test"
	echo "    3 for update test"
	echo "    4 for remount test"
	echo "num_blocks: "
	echo "    count of the number of blocks"
	echo "is_large_key: "
	echo "    1 for key/val more than page size"
	echo "    0 for key/val less than page size"
	exit 0
fi

rmmod project6
rmmod nandsim

COUNTER=$2

./launch_flash_simulator.sh $COUNTER
./insert_mod.sh

let sum=64
let value=$COUNTER*$sum
let value=value-1

echo "######## Running for $COUNTER blocks #########"


if [ "$1" -ne 4 ]; then
	./testbench $1 $value $3
fi


if [ "$1" -eq 4 ]; then
	./testbench 5 $value $3
	./insert_mod.sh
	./testbench 6 $value $3
fi

echo "#################################################"
let COUNTER=COUNTER+50 
