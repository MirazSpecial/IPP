#!/bin/bash

prog=$(realpath "$1")
dir=$2

out=$(mktemp)
err=$(mktemp)
valgrind=$(mktemp)

for input in ${dir}/*.in
do
	name=${input::-2}
	echo "Running test $(basename $input).."
	valgrind --leak-check=full -q --log-file=$valgrind ${prog} < $input > $out 2> $err
    if [ $? -ne 0 ]
    then
        echo "PROGRAM CRASHED."
	elif [ "$(diff "$name"out $out)" != "" ] || [ "$(diff "$name"err $err)" != "" ]
	then
		echo "WRONG ANSWER."
	elif [ "$(cat $valgrind)" != "" ]
	then
		echo "MEMORY LEAK."
	else
		echo "OK."
	fi
done

rm $out $err $valgrind