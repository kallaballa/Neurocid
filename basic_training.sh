#!/bin/bash

set -x
function run() {
  m=1
	[ -n "$4" ] && m=$4;
  if [ -n "$3" ]; then
		./run.sh game/neurocid -m $m  -c $2.mp4 -i $1  -s $2.pop -l $3 $2 > timeseries.csv 
	else
    ./run.sh game/neurocid -m $m  -c $2.mp4 -i $1  -s $2.pop $2 > timeseries.csv             
	fi
  cp timeseries.csv $2.csv
  echo $2.pop
}

[ -n "$1" ] && POP="$1"

#POP=`run 51 AimOnOneNoMove $POP`
#POP=`run 51 SymmetricLinesNoMoveShort $POP`
POP=`run 200 SymmetricLinesAttackerMoveFacingInward $POP`
POP=`run 200 SymmetricLinesAttackerMoveFarFacingInward $POP`
POP=`run 100 SymmetricLinesFar $POP`

cp $POP basic_training.pop

