#!/bin/bash
if [ "$#" -ne 1 ]; then
	echo "Usage:" $0 "<input.txt>  >output.txt"
	exit 1
fi
for a in 0 1 2 3 4 5 6 7; do
    ./torto -s -a -m -p 8 ${a} < $1 >tmptor.${a} &
    pids[${a}]=$!
done
for pid in ${pids[*]}; do
	wait $pid
done

cat tmptor.{0,1,2,3,4,5,6,7} | ./tortomerge

rm -f tmptor.{0,1,2,3,4,5,6,7}
