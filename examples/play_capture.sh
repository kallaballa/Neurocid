#!/bin/bash

function run() {
  m=1
	[ -n "$4" ] && m=$4;
  if [ -n "$3" ]; then
		neurocid -x 800 -y 800 -c $2.mp4 -m $m -i $1 -s $2.pop -l $3 $2 > timeseries.csv 
	else
    neurocid -x 800 -y 800 -c $2.mp4 -m $m -i $1 -s $2.pop $2 > timeseries.csv             
	fi
  echo $2.pop
}

[ -n "$1" ] && POP="$1"
POP=`run 10000 SymmetricLinesHuge $POP 5`
POP=`run 10000 CrossHuge $POP 5`
cp $POP play.pop
