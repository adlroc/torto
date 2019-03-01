#!/bin/bash
if [ "$#" -ne 1 ]; then
    echo "Usage:" $0 "<input.txt>  >output.txt"
    exit 1
fi
for i in 0 1 2 3 4 5; do
	./torto -s -m -p ${i}  < $1 >tmptor.${i} &
    pids[${i}]=$!
done
for pid in ${pids[*]}; do
    wait $pid
done

cat tmptor.{0,1,2,3,4,5} | ./tortomerge

rm -f tmptor.{0,1,2,3,4,5}
