#!/bin/zsh

# $1: ip address of the server  

mkdir -p out
for i in $(seq 0 20); do
    ../uec -a $i -o out/uec_$i.csv $1 >> out/uec_console_$i.log
    sleep 3
done
