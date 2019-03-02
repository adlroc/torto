#!/bin/bash
if [ "$#" -ne 1 ]; then
    echo "Usage:" $0 "<input.txt>  >output.txt"
    exit 1
fi
for i in 0 1 2 3 4 5; do
	for p in 0 1; do
		./torto -s -m -i ${i} -p 1 ${p} < $1 >tmptor.${i}.${p} &
    	pids[${i}*2+${p}]=$!
   	done
done
for pid in ${pids[*]}; do
    wait $pid
done

cat tmptor.{0,1,2,3,4,5}.{0,1} | ./tortomerge

rm -f tmptor.{0,1,2,3,4,5}.{0,1}
