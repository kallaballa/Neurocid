#!/bin/bash

set -x
function run() {
  m=1
	if [ -n "$3" ]; then
    neurocid -c $2.mp4 -i $1 --saveA $2.pop --loadA $3 $2 > timeseries.csv
	else
    neurocid -c $2.mp4 -i $1 --saveA $2.pop $2 > timeseries.csv
	fi
  cp timeseries.csv $2.csv
  echo $2.pop
}

[ -n "$1" ] && POP="$1"

#POP=`run 10000 AimOnOneNoMove $POP`
#POP=`run 300 SymmetricLinesNoMoveShort $POP`
#POP=`run 100 SymmetricLinesAttackerMove $POP`
POP=`run 100 SymmetricLinesAttackerMoveFar $POP`
POP=`run 500 SymmetricLinesFar $POP`

cp $POP basic_training.pop

