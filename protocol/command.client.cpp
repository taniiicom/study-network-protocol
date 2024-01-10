//
// 東京都立大学 情報科学科
// 助教・柴田祐樹
// ２０１９年１０月　初版
// ２０２０年１０月　改訂
//

#include <arpa/inet.h>
#include <iostream>
#include <unistd.h> // https://linux.die.net/man/2/read
#include <string>
#include <sstream>

// このプログラム，command.client.cppとcommand.server.cpp　は，指導書p. 5の第二段落で例として取り上げられている，文字列符号化を使い，簡単な遠隔操作コマンドを実装したものです．
// 
// <命令><空白><数値><空白><命令><数値>・・・
// 
// の順で文字列を送ることで，サーバを操作できます．改行コードも空白として認識します．命令には，
// 
// set reg: 初期値を設定
// add: 数値を加算
// subtract: 数値を減算
// multiply: 数値を乗算
// divide: 数値を除算
// end process: 数値を除算
// get reg: 結果を取得
// 
// するものが用意されています．
// たとえば，サーバに以下の文字列
// 
// "set reg" 1 
// add 1
// add 1 
// subtract 3
// add 1
// "get reg"
// "end process"
// 
// を送ると，1が帰ってきます．

const int BUFF_SIZE = 64; // 一時記憶の長さ

int main(int argc, char *argv[])
{
    using namespace std;
    cout << "command client v1.1.0" << endl; // ソースコードへの変更を行ったら数値を変える．

    // サーバのアドレスとポート番号
    // 127.0.0.1は、ループバックアドレス
    // 他のPCと通信する場合は、当該PCのIPアドレスに変更する。
    string serv_ip = "127.0.0.1";
    in_port_t serv_port = 5000;
    if (argc > 1)
    {
        serv_ip = argv[1];
    }
    if (argc > 2)
    {
        serv_port = stoi(argv[2]);
    }

    // サーバのアドレス等を初期化。
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(serv_ip.c_str());
    serv_addr.sin_port = htons(serv_port);

    // ソケット作成、入力はIP、ストリーム型、TCPを指定。
    int socketd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socketd < 0)
    {
        cout << "Failed to createa socket\n";
        return -1;
    }
    // サーバに接続する。
    int ret = connect(socketd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (ret < 0)
    {
        cout << "Failed to connect to the server\n";
        return -1;
    }
    // 接続すると、サーバは現在時刻を文字列として返信する。
    // read(.)により、データを受信する。
    string ope;
    char buff[BUFF_SIZE];
    int n; // read の戻り値格納用変数
    ope = "\"get reg\"\n"; // 空白が有る場合は " "で囲む．「"」はC言語の文字列の中で表すためには「\"」と書かなければならない．
    write(socketd, ope.c_str(), ope.size());
    n = read(socketd, buff, BUFF_SIZE - 1);
    if (n < 0)
    {
        cout << "failed to read from a socket\n";
        return -1;
    }
    buff[n] = 0;
    cout << buff << "\n";

    ope = "\"set reg\" 1\n"; 
    ope += "add 1\n";
    ope += "\"add\" 1\n"; // add は空白がないので"があってもなくてもどちらでも良い．
    ope += "subtract 3\n";
    ope += "add 1\n";
    ope += "\"get reg\"\n";
    ope += "\"end process\"\n";
    write(socketd, ope.c_str(), ope.size());
    n = read(socketd, buff, BUFF_SIZE - 1);
    if (n < 0)
    {
        cout << "failed to read from a socket\n";
        return -1;
    }
    buff[n] = 0;
    cout << buff << "\n"; // 1と出力されるはずである．
    
    close(socketd);
}
