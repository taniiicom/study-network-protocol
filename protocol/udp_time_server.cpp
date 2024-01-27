// Echo Program (server)

#include <arpa/inet.h>
#include <unistd.h>  // https://linux.die.net/man/2/read

#include <chrono>
#include <ctime>
#include <iostream>

const int BUFF_SIZE = 500;  // バッファのサイズ

int main(int argc, char* argv[]) {
  using namespace std;
  cout << "upd echo server v1.0.0"
       << endl;  // ソースコードへの変更を行ったら数値を変える．

  // パラメータ
  int port_num = 5000;  // 待ち受けポート番号
  struct sockaddr_in serv_addr,
      clnt_addr;    // サーバとクライアントのソケットアドレス
  int serv_socket;  // ソケット記述子
  socklen_t addr_len;  // アドレス長
  int n = 0;           // 戻り値の保存用

  time_t now;  // 現在時刻の保存用変数

  // パラーメータ初期化
  serv_addr.sin_family = AF_INET;  // IPv4 プロトコルファミリー
  serv_addr.sin_addr.s_addr = INADDR_ANY;  // インターネットアドレス
  serv_addr.sin_port = htons(port_num);    // ポート番号設定

  // ソケット作成
  // 引数にIPv4, データグラム，UDPを指定する．
  serv_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (serv_socket < 0) {
    cout << "Fail to create a socket.\n";
  }
  // バインド（ソケットとポートの結合）
  if (bind(serv_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    cout << "Failed to bind a socket.\n";
    return -1;
  }

  // クライアントからのクエリを待ち受け．
  while (true) {
    bool is_end = false;

    // クライアントからクエリ文字列を待ち受ける．
    // UDPはコネクションを確立しないため，クライアントがクエリ文字列を送ってくるのを待機．
    cout << "waiting for a client...\n";
    addr_len = sizeof(clnt_addr);

    // 受信
    string recv_msg = "";  // メッセージ
    {                      // arrival
      int cursor = 0;      // バッファのカーソル
      char snippet[BUFF_SIZE];

      while (!is_end) {
        n = recvfrom(serv_socket, snippet, BUFF_SIZE, 0,
                     (struct sockaddr*)&clnt_addr, &addr_len);
        if (n < 0) {
          cout << "failed to read a query from the socket.\n";
          return -1;
        }

        printf("受信中...\n");

        // sub_msg に 終端文字が含まれいているか check
        for (int i = 0; i < n; i++) {
          if (snippet[i] == char(4)) {
            printf("終端文字を検出しました.\n");
            is_end = true;
            recv_msg.push_back('\0');
            break;
          }

          if (snippet[i] != '\0') {
            recv_msg.push_back(snippet[i]);
          }
        }
        cursor += BUFF_SIZE;
      }
    }

    // 標準出力
    // cout << "Received a query from [" << inet_ntoa(clnt_addr.sin_addr) << ",
    // " << htons(clnt_addr.sin_port) << "]" << endl;
    string m = recv_msg;  // 受け取ったメッセージ
    string ip = inet_ntoa(clnt_addr.sin_addr);
    string port = to_string(htons(clnt_addr.sin_port));
    string msg = m + " " + ip + " " +
                 port;  // string クラスは加算演算子で文字列を結合可能．
    cout << msg << endl;

    // 現在時刻取得
    // time(&now);
    // string msg = string("from shibata ") + ctime(&now); // string
    // クラスは加算演算子で文字列を結合可能．

    {  // departure
      // 文字列を分割して送信
      msg += char(4);  // EOT: end of transmission

      int cursor = 0;
      int msg_size = msg.size();

      while (cursor < msg_size) {
        int sending_size = min(BUFF_SIZE, msg_size - cursor);

        // 送信
        // buf: char 型配列の送信する先頭ポインタ
        n = sendto(serv_socket, msg.c_str() + cursor, sending_size + 1, 0,
                   (struct sockaddr*)&clnt_addr, sizeof(clnt_addr));
        if (n < 0) {
          cout << "Failed to send a message.\n";
          return -1;
        }

        cursor += BUFF_SIZE;
      }
    }
  }

  // ソケットを閉じる．
  close(serv_socket);
  return 0;
}
