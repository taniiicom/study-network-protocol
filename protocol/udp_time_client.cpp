// Echo Program (client)

#include <arpa/inet.h>
#include <iostream>
#include <chrono>
#include <ctime>
#include <unistd.h> // https://linux.die.net/man/2/read
#include <random>

const int BUFF_SIZE = 64; // バッファのサイズ
using namespace std;

/*
 * UDP Daytimeクライアント
 */
int main(int argc, char* argv[])
{
    cout << "upd time client v1.0.0" << endl; // ソースコードへの変更を行ったら数値を変える．
    string serv_ip = "127.0.0.1"; // ループバックアドレス
    in_port_t port_num = 5000; // ポート番号
    int n = 0; // 戻り値の保存用
    char buff[BUFF_SIZE]; // 送受信用バッファ

    // options
    bool option_auto = false;
    int option_auto_length = 0;

    {
        /* コマンドライン引数 */
        // UNIX ライクなハイフンを使ったオプションの解析
        int opt;
        opterr = 0; //getopt()のエラーメッセージを無効にする。
        while ((opt = getopt(argc, argv, "a:h")) != -1) {
            //コマンドライン引数のオプションがなくなるまで繰り返す
            switch (opt) {
                case 'a':
                    printf("-a [int]: auto: 文字列を標準入力で指定せずに, 自動で生成します. 引数には文字列の長さを指定します.\n");
                    printf("string length: %s\n", optarg);
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
        serv_ip = argv[optind];
    }

    // パラメータの初期化
    struct sockaddr_in serv_addr; // アドレス構造体
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(serv_ip.c_str());
    serv_addr.sin_port = htons(port_num);

    // ソケットの作成．UDPを用いるため第２引数にDatagram，第３引数にUDPを指定する．
    int socketd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socketd < 0) {
        cout << "Failed to create a client socket.\n";
        return -1;
    }

    // クエリ作成．
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
    }

    for (;;)
    {
        std::time_t now, later;

        if (option_auto == false) {
            cout << "input message: ";
            cin >> msg;
            if (msg == " q" || msg == "quit" || msg == "exit") {
                break;
            }
        }

        time(&now);
        n = sendto(socketd, msg.c_str(), msg.size(), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        if (n < 0) {
            cout << "failed to receive a message.\n";
            return -1;
        }

        // サーバから現在時刻を文字列として受信．
        n = recvfrom(socketd, buff, sizeof(buff)-1, 0, NULL, NULL); // 終端文字列を入れるために，sizeof(buff)-1 として，文字列一つ分必ず余裕を持たせてデータを受信する．buff をこのまま文字列として使わない場合は全記憶を受信に使う．
        if (n < 0) {
            cout << "Failed to receive a message.\n";
            return -1;
        }
        time(&later);

        buff[n] = 0; // 終端文字列を追加．送信者が終端文字列を入れてデータを送ってきているとは限らない．
        cout << "Echo: " << buff << ", " << htons(serv_addr.sin_port) << "\n";

        double seconds = std::difftime(later, now);
        cout << "RTT: " << seconds << " seconds\n";

        if (option_auto == true) {
            printf("-a オプションが有効のため, 自動で生成した文字列を送信しました.\n");
            printf("string: %s\n", msg.c_str());
            break;
        };
    }

    // ソケットを閉じる
    close(socketd);
}
