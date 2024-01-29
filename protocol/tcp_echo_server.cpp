// Echo Program (server)

#include <arpa/inet.h>
#include <unistd.h>  // https://linux.die.net/man/2/read

#include <chrono>
#include <ctime>
#include <iostream>

const int BUFF_SIZE = 500;  // バッファのサイズ

int main(int argc, char *argv[]) {
  // パラメータ
  using namespace std;
  cout << "tcp time server v1.0.0"
       << endl;  // ソースコードへの変更を行ったら数値を変える．

  int port_num = 5000;  // ポート番号

  struct sockaddr_in serv_addr, clnt_addr;  // ソケットアドレス
  int serv_socket, clnt_socket;             // ソケット記述子
  socklen_t addr_len;                       // アドレス長
  int n;                                    // 戻り値の保存用

  time_t now;  // 時間
  char buff[BUFF_SIZE];

  // パラメータの初期化
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(port_num);
  addr_len = sizeof(clnt_addr);

  // 接続要求受付用のソケットを作成。
  // ソケット記述子（Socket
  // descripter）が戻り値であるが、エラーが起こった場合は「-1」が返される。
  serv_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (serv_socket < 0) {
    cout << "Failed to create a socket.\n";
    return -1;
  }
  // バインド（ソケットとポートの結合）
  if (bind(serv_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    cout << "Failed to bind a socket to the system.\n";
    return -1;
  }
  // ソケットをコネクション受け入れ可能な状態にする。
  // 第２引数は、接続キューのサイズ。５つまで同時接続を受け入れると指定。
  if (listen(serv_socket, 5) < 0) {
    cout << "Filaed to listen to a socket.\n";
    return -1;
  }

  // クライアントから接続要求があれば、順次対応
  while (true) {
    // accept(.)により、クライアントからの接続要求を受け付ける。
    // 戻り値はクライアントとのデータ通信用ソケット記述子、エラーの場合は０以下の値が返される。
    cout << "Waiting for a client..." << endl;
    clnt_socket = accept(serv_socket, (struct sockaddr *)&clnt_addr, &addr_len);

    // クライアントのIPアドレスとポート番号を表示。
    // それぞれ、struct sockaddr_inから取得。
    // inet_ntoa(.)は、arpa/inet.hで定義されている（Unix系の場合）。 htons
    // はエンディアンを変換する．
    cout << "Accepted a connection from [" << inet_ntoa(clnt_addr.sin_addr)
         << "," << htons(clnt_addr.sin_port) << "]" << endl;

    int cursor = 0;

    for (;;) {
      n = read(clnt_socket, buff + cursor, sizeof(buff));
      if (n <= 0) {
        // 相手の通信が切断されている．
        break;
      } else {
        cursor += n;
      }
    }

    buff[n] =
        '\0';  // 文字列として他の関数に渡す場合は，終端文字を追加することを忘れないように気をつける．
    cout << buff;

    string m = buff;  // 受け取ったメッセージ
    string ip = inet_ntoa(clnt_addr.sin_addr);
    string port = to_string(htons(clnt_addr.sin_port));
    string msg = m + " " + ip + " " +
                 port;  // string クラスは加算演算子で文字列を結合可能．
    cout << msg << endl;

    // クライアントソケットにバッファの内容を書き込む。
    n = write(clnt_socket, msg.c_str(), msg.size());

    // クライアントとの通信は終了したので、ソケットを閉じる。
    close(clnt_socket);
  }

  // 受付用のソケットを閉じる。
  close(serv_socket);
  return 0;
}
