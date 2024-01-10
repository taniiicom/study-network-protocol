//
// 情報通信応用実験 ネットワークプログラミング
//
// 首都大学東京 システムデザイン学部 情報通信システムコース
// 准教授・酒井和哉
// ２０１５年２月５日
//
// 情報科学科
// 助教・柴田祐樹
// ２０１９年１０月　改訂
// ２０２０年１０月　改訂
//

#include <arpa/inet.h>
#include <iostream>
#include <unistd.h> // https://linux.die.net/man/2/read
#include <string>
const int BUFF_SIZE = 64; // バッファのサイズ

int main(int argc, char* argv[])
{
    using namespace std;
    cout << "tcp time client v1.0.0" << endl; // ソースコードへの変更を行ったら数値を変える．

    // サーバのアドレスとポート番号
    // 127.0.0.1は，ループバックアドレス
    // 他のPCと通信する場合は，当該PCのIPアドレスに変更する．
    string serv_ip = "127.0.0.1";
    in_port_t serv_port = 5000;
    
    if(argc > 1)
    {
	    serv_ip = argv[1];
    }
    char buff[BUFF_SIZE];// 受信用バッファ
    int n = 0; // 戻り値の保存用に使う変数．

    // ソケット作成，入力はIP，ストリーム型，TCPを指定．
    int socketd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socketd < 0) {
        cout << "Failed to createa socket\n";
        return -1;
    }
    // サーバのアドレス等を初期化．
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(serv_ip.c_str());
    serv_addr.sin_port = htons(serv_port);

    // サーバに接続する．
    n = connect(socketd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (n < 0) {
        cout << "Failed to connect to the server\n";
        return -1;
    }
    string msg = "hello\n";
    n = write(socketd, msg.c_str(), msg.size()); // 文字列の送信．第二引数は記憶域．第３引数は送信するByte数．
    // 接続すると，サーバは現在時刻を文字列として返信する．
    // read(.)により，データを受信する．
    n = read(socketd, buff, sizeof(buff)-1);
    if (n < 0) {
        // readの戻り値が負の場合，通信に不具合が生じたことを意味する．
        cout << "failed to read from a socket\n";
        return -1;
    }
    // readの戻り値が 0 の場合，相手が接続を遮断したことを意味する．
    buff[n] = 0;
    // サーバからの返信された文字列（現在時刻）を表示
    cout << buff;

    // close the socket
    close(socketd);
}
