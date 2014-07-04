#!/bin/bash

set -x
function run() {
  m=1
 if [ -n "$4" ]; then
    neurocid -i $1 -a 10 --saveA $2.pop --loadA $3 --loadB $4 $2 > timeseries.csv
  elif [ -n "$3" ]; then
    neurocid -i $1 -a 10 --saveA $2.pop --loadA $3 $2 > timeseries.csv
	else
    neurocid -i $1 -a 10 --saveA $2.pop $2 > timeseries.csv
	fi
  cp timeseries.csv $2.csv
  echo $2.pop
}

[ -n "$1" ] && POP="$1"

#POP=`run 1000 AimOnOneNoMove $POP`
#POP=`run 300 SymmetricLinesNoMoveShort $POP`
POP=`run 1000 SymmetricLinesAttackerMove $POP`
#POP=`run 500 SymmetricLinesAttackerMoveFar $POP`
POP=`run 1000 SymmetricLinesAttackerMoveLong $POP`
POP=`run 1000 SymmetricLines $POP $POP`
POP=`run 5000 SymmetricLinesFarLong $POP $POP`




cp $POP basic_training.pop

