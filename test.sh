#!/bin/bash

if [ "$#" -ne 3 ]; then
	echo "Illegal number of parameters"
	echo "./test.sh <test_number> <num_blocks> <is_large_key>"
	echo "test_number: "
	echo "    1 for read-write test"
	echo "    2 for delete test"
	echo "    3 for update test"
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

echo "############################ for $COUNTER $value"
./testbench $1 $value $3

echo "############################"
let COUNTER=COUNTER+50 
