#!/bin/bash
if [ "$#" -ne 1 ]; then
    echo "Usage:" $0 "<input.txt>  >output.txt"
    exit 1
fi
for a in 0 1; do
	for b in 0 1; do
		for c in 0 1; do
			./torto -s -m -p 0 ${a} -p 1 ${b} -p 2 ${c} < $1 >tmptor.${a}.${b}.${c} &
			pids[${a}*4+${b}*2+${c}]=$!
		done
    done
done
for pid in ${pids[*]}; do
    wait $pid
done

cat tmptor.{0,1}.{0,1}.{0,1} | ./tortomerge

rm -f tmptor.{0,1}.{0,1}.{0,1}
