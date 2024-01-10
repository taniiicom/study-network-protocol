# 柴田祐樹 情報科学科　助教改訂
# ２０２０年１０月 

# Makefileについて：
#
# Makefileとは、コンパイルコマンドとファイルとファイル間の依存関係を記す。
# 「make」と入力するだけで、自動的にコマンドを実行しプログラムのコンパイルをしてくれる。
# スクリプトとの違いは、依存関係に基づいて、更新が必要なファイルだけをコンパイルする点である。
#
# 「make」を入力して、Makefileを実行する場合は、ファイル名を「Makefile」にする必要がある。
# 他の名前を使いたい場合は、「make -f file_name」といった要領で、オプションでファイル名を指定する。
# （ファイル名を「make_windows」とした場合は、「make -f make_windows」）

# all ：
# ターミナルで「make」と入力すると実行するコマンドリスト
# タブでallの範囲を指定する。
# 「-w」は、警告を全て出す、「-o」でコンパイル後の実行ファイル名
all: tts ttc uts utc tod cmds cmdc

tts: tcp_time_server.cpp
	g++ -std=c++14  -w  tcp_time_server.cpp -o tts 
ttc: tcp_time_client.cpp
	g++ -std=c++14  -w  tcp_time_client.cpp -o ttc 
uts: udp_time_server.cpp
	g++ -std=c++14  -w  udp_time_server.cpp -o uts 
utc: udp_time_client.cpp
	g++ -std=c++14  -w  udp_time_client.cpp -o utc 
tod: ex_gettimeofday.cpp 
	g++ -std=c++14  -w  ex_gettimeofday.cpp -o tod
cmds:  command.server.cpp
	g++ -std=c++14  -w  command.server.cpp -o cmds
cmdc: command.client.cpp
	g++ -std=c++14  -w  command.client.cpp -o cmdc


clean:
	rm tts ttc uts utc tod cmds cmdc
