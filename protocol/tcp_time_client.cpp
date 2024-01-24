// Echo Program (client)

#include <arpa/inet.h>
#include <iostream>
#include <chrono>
#include <ctime>
#include <unistd.h> // https://linux.die.net/man/2/read
#include <string>
#include <random>

const int BUFF_SIZE = 500; // バッファのサイズ

int main(int argc, char* argv[])
{
    using namespace std;
    cout << "tcp time client v1.0.0" << endl; // ソースコードへの変更を行ったら数値を変える．

    // サーバのアドレスとポート番号
    // 127.0.0.1は，ループバックアドレス
    // 他のPCと通信する場合は，当該PCのIPアドレスに変更する．
    string serv_ip = "127.0.0.1";
    in_port_t serv_port = 5000;
    
    // options
    bool option_auto = false;
    int option_auto_length = 0;

    {
        /* コマンドライン引数 */
        // UNIX ライクなハイフンを使ったオプションの解析
        int opt;
        opterr = 0; //getopt()のエラーメッセージを無効にする。
        while ((opt = getopt(argc, argv, "ha:")) != -1) {
            //コマンドライン引数のオプションがなくなるまで繰り返す
            switch (opt) {
                case 'a':
                    option_auto = true;
                    option_auto_length = atoi(optarg);
                    break;

                case 'h':
                    printf(R"(-h: hint: 
-a [int]: auto: 文字列を標準入力で指定せずに, 自動で生成します. 引数には文字列の長さを指定します.
-h: hint: ヒントを表示します.
to_ip: 送信先の ip アドレスを指定します.
Usage: %s [-a] to_ip ...
)");
                    break;

                default: /* '?' */
                    //指定していないオプションが渡された場合
                    printf("Usage: %s [-a] arg1 ...\n", argv[0]);
                    break;
            }
        }
        // arg1: to_ip: 送信先の ip アドレスを指定します.
        if (optind < argc) {
            serv_ip = argv[optind];
        } else {
            printf("IP address is not provided.\n");
            return 1;
        }
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

    string msg = "";
    
    if (option_auto == true)
    {
        random_device rd;
        mt19937_64 mt(rd());
        uniform_int_distribution<char> cU(30, 80);
        for (size_t i = 0; i < option_auto_length; i++)
        {
            char c = cU(mt);
            msg.push_back(c);
        }
        msg.push_back('\0');
    }

    for (;;) {
        std::time_t now, later;

        if (option_auto == true) {
            printf("-a オプションが有効のため, 自動で生成した文字列を送信します.\n");
            printf("string length: %i\n", option_auto_length);
            printf("string: %s\n", msg.c_str());
        } else {
            cout << "input message: ";
            cin >> msg;
            if (msg == " q" || msg == "quit" || msg == "exit") {
                break;
            }
        }

        time(&now);

        n = write(
            socketd, msg.c_str(),
            msg.size());  // 文字列の送信．第二引数は記憶域．第３引数は送信するByte数．
    
        // read(.)により，データを受信する．
        n = read(socketd, buff, sizeof(buff)-1);
        if (n < 0) {
            // readの戻り値が負の場合，通信に不具合が生じたことを意味する．
            cout << "failed to read from a socket\n";
            return -1;
        }
        // readの戻り値が 0 の場合，相手が接続を遮断したことを意味する．
        buff[n] = 0;
        if(n <= 0){
            // 相手の通信が切断されている．
            return -1;
        }

        // サーバからの返信された文字列（現在時刻）を表示
        cout << buff;
    }

    // close the socket
    close(socketd);
}
