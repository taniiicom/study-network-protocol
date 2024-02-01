import matplotlib.pyplot as plt
import numpy as np

# ファイルを開く
with open("combined_rtt_uec.csv", 'r') as file:
    # ファイルの内容を読み込む
    data = file.read()

# 改行で分割してリストに格納する
lines = data.split('\n')
lines.pop()
for i in range(len(lines)):
    lines[i] = float(lines[i])

# Extracting the values from the dataframe
y_values = lines
x_values = [2**i for i in range(0, len(y_values))]

# Plotting the graph
plt.figure(figsize=(10, 6))
plt.plot(x_values, y_values, marker='o')
plt.xscale('log')  # Setting x-axis to logarithmic scale
plt.xlabel('length')
plt.ylabel('time [ms]')
plt.title('UDP')
plt.grid(True)
plt.savefig('graph_udp.pdf')
