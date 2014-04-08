#!/bin/bash

function run() {
  m=1
	[ -n "$4" ] && m=$4;
  if [ -n "$3" ]; then
		./run.sh game/neurocid -x 1600 -y 900  -m $m -i $1 -s $2.pop -l $3 $2 > timeseries.csv 
	else
    ./run.sh game/neurocid -x 1600 -y 900 -m $m -i $1 -s $2.pop $2 > timeseries.csv             
	fi
  echo $2.pop
}

[ -n "$1" ] && POP="$1"
POP=`run 10000 SymmetricLinesHuge $POP 5`
POP=`run 10000 CrossHuge $POP 5`
cp $POP play.pop
