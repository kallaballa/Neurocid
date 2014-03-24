#!/bin/bash

set -x
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

rm AimOnOneNoMove.pop
POP=`run 500 AimOnOneNoMove`
POP=`run 500 SymmetricLinesNoMove $POP`
POP=`run 500 AimOnOne $POP`
POP=`run 500 SymmetricLinesAttackerMoveFar $POP`
POP=`run 500 SymmetricLines $POP`
POP=`run 500 SymmetricLinesFar $POP`
cp $POP basic_training.pop

