#!/bin/bash

if [ "$#" -ne 3 ]; then
	echo :"DONOT USE THIS FOR HUGE PAGES, OTHERWISE IT WILL SPAM STDOUT(USE TEST.SH INSTEAD"
	echo "Illegal number of parameters"
	echo "./short_test.sh <test_number> <num_pages> <is_large_key>"
	echo "test_number: "
	echo "    1 for read-write test"
	echo "    2 for delete test"
	echo "    3 for update test"
	echo "    4 for remount test"
	echo "num_pages: "
	echo "    count of the number of pages"
	echo "is_large_key: "
	echo "    1 for key/val more than page size"
	echo "    0 for key/val less than page size"
	exit 0
fi

rmmod project6
rmmod nandsim

./launch_flash_simulator.sh 5
./insert_mod.sh

echo "######## Running for $2 pages #########"


if [ "$1" -ne 4 ]; then
	./testbench $1 $2 $3 1
fi


if [ "$1" -eq 4 ]; then
	./testbench 5 $2 $3 1
	./insert_mod.sh
	./testbench 6 $2 $3 1
fi

echo "#################################################"
