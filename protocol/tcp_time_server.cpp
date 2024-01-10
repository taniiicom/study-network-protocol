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
#include <chrono>
#include <ctime>
#include <unistd.h> // https://linux.die.net/man/2/read

const int BUFF_SIZE = 64; // バッファのサイズ

int main(int argc, char *argv[])
{
    // パラメータ
    using namespace std;
    cout << "tcp time server v1.0.0" << endl; // ソースコードへの変更を行ったら数値を変える．

    int port_num = 5000; // ポート番号

    struct sockaddr_in serv_addr, clnt_addr; // ソケットアドレス
    int serv_socket, clnt_socket;            // ソケット記述子
    socklen_t addr_len;                      // アドレス長
    int n;                                   // 戻り値の保存用

    time_t now;           // 時間
    char buff[BUFF_SIZE]; // 送信用バッファ（６４バイト）

    // パラメータの初期化
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port_num);
    addr_len = sizeof(clnt_addr);

    // 接続要求受付用のソケットを作成。
    // ソケット記述子（Socket descripter）が戻り値であるが、エラーが起こった場合は「-1」が返される。
    serv_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serv_socket < 0)
    {
        cout << "Failed to create a socket.\n";
        return -1;
    }
    // バインド（ソケットとポートの結合）
    if (bind(serv_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        cout << "Failed to bind a socket to the system.\n";
        return -1;
    }
    // ソケットをコネクション受け入れ可能な状態にする。
    // 第２引数は、接続キューのサイズ。５つまで同時接続を受け入れると指定。
    if (listen(serv_socket, 5) < 0)
    {
        cout << "Filaed to listen to a socket.\n";
        return -1;
    }

    // クライアントから接続要求があれば、順次対応
    while (true)
    {
        // accept(.)により、クライアントからの接続要求を受け付ける。
        // 戻り値はクライアントとのデータ通信用ソケット記述子、エラーの場合は０以下の値が返される。
        cout << "Waiting for a client..." << endl;
        clnt_socket = accept(serv_socket, (struct sockaddr *)&clnt_addr, &addr_len);

        // クライアントのIPアドレスとポート番号を表示。
        // それぞれ、struct sockaddr_inから取得。
        // inet_ntoa(.)は、arpa/inet.hで定義されている（Unix系の場合）。 htons はエンディアンを変換する．
        cout << "Accepted a connection from [" << inet_ntoa(clnt_addr.sin_addr) << "," << htons(clnt_addr.sin_port) << "]" << endl;
        n = read(clnt_socket, buff, sizeof(buff) - 1);
        if(n <= 0){
            // 相手の通信が切断されている．
            return -1;
        }
        buff[n] = 0; // 文字列として他の関数に渡す場合は，終端文字を追加することを忘れないように気をつける．
        cout << buff;
        // time(.)で現在時間取得（秒単位の歴時間）、ctime(.)で文字列に変換し、送信バッファに書き込み。
        time(&now);

        string msg = ctime(&now);

        // クライアントソケットにバッファの内容を書き込む。
        n = write(clnt_socket, msg.c_str(), msg.size());

        // クライアントとの通信は終了したので、ソケットを閉じる。
        close(clnt_socket);
    }

    // 受付用のソケットを閉じる。
    close(serv_socket);
    return 0;
}
