#!/bin/bash
BASEDIR=$(dirname "$0")

source /etc/neurocid
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$NEUROCID_PATH
export PATH=$PATH:$NEUROCID_PATH
export LUA_PATH=$NEUROCID_PATH/lua/?.lua

set -x
function run() {
  m=1
 if [ -n "$4" ]; then
    $BASEDIR/felgo-neurocid -i $1 -a 10 --saveA $2.pop --loadA $3 --loadB $4 $2 > timeseries.csv
  elif [ -n "$3" ]; then
    $BASEDIR/felgo-neurocid -i $1 -a 10 --saveA $2.pop --loadA $3 $2 > timeseries.csv
	else
    $BASEDIR/felgo-neurocid -i $1 -a 10 --saveA $2.pop $2 > timeseries.csv
	fi
  cp timeseries.csv $2.csv
  echo $2.pop
}

[ -n "$1" ] && POP="$1"

POP=`run 1000 SymmetricLinesAttackerMove $POP`
POP=`run 1000 SymmetricLinesAttackerMoveLong $POP`
POP=`run 1000 SymmetricLines $POP $POP`
POP=`run 5000 SymmetricLinesFarLong $POP $POP`

cp $POP training.pop

