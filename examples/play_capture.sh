#!/bin/bash
set -x
function run() {
  m=1
  if [ -n "$3" ]; then
		neurocid -x 800 -y 800 -c $2.mp4 -i $1 --saveA $2.pop --loadA $3 --loadB $3 $2 > timeseries.csv 
	else
    neurocid -x 800 -y 800 -c $2.mp4 -i $1 --saveA $2.pop $2 > timeseries.csv             
	fi
  echo $2.pop
}

[ -n "$1" ] && POP="$1"
POP=`run 10000 SymmetricLinesHuge $POP`
cp $POP play.pop
