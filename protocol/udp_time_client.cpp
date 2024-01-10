// Echo Program (client)

#include <arpa/inet.h>
#include <iostream>
#include <chrono>
#include <ctime>
#include <unistd.h> // https://linux.die.net/man/2/read

const int BUFF_SIZE = 64; // バッファのサイズ


/*
 * UDP Daytimeクライアント
 */
int main(int argc, char* argv[])
{
    using namespace std;
    cout << "upd time client v1.0.0" << endl; // ソースコードへの変更を行ったら数値を変える．
    string serv_ip = "127.0.0.1"; // ループバックアドレス
    in_port_t port_num = 5000; // ポート番号
    int n = 0; // 戻り値の保存用
    char buff[BUFF_SIZE]; // 送受信用バッファ

    if (argc == 2) {
        serv_ip = argv[1];
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
    // クエリ送信．
    string msg = "query";

    for (;;)
    {
        std::time_t now, later;

        cout << "input message: ";
        cin >> msg;
        if (msg == " q" || msg == "quit" || msg == "exit") {
            break;
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
    }

    // ソケットを閉じる
    close(socketd);
}
