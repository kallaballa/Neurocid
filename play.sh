#!/bin/bash

function run() {
  m=1
	[ -n "$4" ] && m=$4;
  if [ -n "$3" ]; then
		./run.sh game/tankwar -m $m  -i $1  -s $2.pop -l $3 $2 > timeseries.csv 
	else
    ./run.sh game/tankwar -m $m  -i $1  -s $2.pop $2 > timeseries.csv             
	fi
  echo $2.pop
}

POP="basic_training.pop"
POP=`run 100 SymmetricLinesHuge $POP 5`

