#!/bin/bash

# 出力ファイルをクリア（存在する場合）
> combined_rtt_uec.csv

for i in $(seq 0 20); do
    file="out/uec_$i.csv"
    # 各ファイルから3行目を抽出し、出力ファイルに追加
    awk 'NR==3' "$file" >> combined_rtt_uec.csv
done
