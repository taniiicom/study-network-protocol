#!/bin/zsh

# $1: ip address of the server  

mkdir -p out
for i in $(seq 0 20); do
    ../tec -a $i -o out/tec_$i.csv $1 >> out/tec_console_$i.log
    sleep 3
done
