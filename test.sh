#!/bin/bash
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
