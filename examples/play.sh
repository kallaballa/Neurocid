#!/bin/bash
set -x
function run() {
  m=1

  if [ -n "$4" ]; then
    neurocid -a 10 -x 1600 -y 900 -i $1 --saveA $2.pop --loadA $3 --loadB $4 $2 > timeseries.csv
  elif [ -n "$3" ]; then
		neurocid -a 10 -x 1600 -y 900 -i $1 --saveA $2.pop --loadA $3 --loadB $3 $2 > timeseries.csv 
	else
    neurocid -a 10 -x 1600 -y 900 -i $1 --saveA $2.pop $2 > timeseries.csv             
	fi
  echo $2.pop
}

[ -n "$1" ] && POP="$1"
POP=`run 14000 SymmetricLinesHuge $POP`
cp $POP play.pop
