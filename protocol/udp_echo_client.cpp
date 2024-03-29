// Echo Program (client)

#include <arpa/inet.h>
#include <unistd.h>  // https://linux.die.net/man/2/read

#include <chrono>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>
#include <random>
#include <string>

const int BUFF_SIZE = 500;  // バッファのサイズ
using namespace std;

int main(int argc, char* argv[]) {
  cout << "upd echo client v1.0.0"
       << endl;  // ソースコードへの変更を行ったら数値を変える．
  string serv_ip = "127.0.0.1";  // ループバックアドレス
  in_port_t port_num = 5000;     // ポート番号
  int n = 0;                     // 戻り値の保存用
  char buff[BUFF_SIZE];          // 送受信用バッファ

  // options
  bool option_auto = false;
  int option_auto_length = 0;
  string out_name = "";

  {
    /* コマンドライン引数 */
    // UNIX ライクなハイフンを使ったオプションの解析
    int opt;
    opterr = 0;  // getopt()のエラーメッセージを無効にする。
    while ((opt = getopt(argc, argv, "ho:a:")) != -1) {
      // コマンドライン引数のオプションがなくなるまで繰り返す
      switch (opt) {
        case 'a':
          option_auto = true;
          option_auto_length = atoi(optarg);
          break;

        case 'h':
          printf(R"(-h: hint: 
-a [int]: auto: 文字列を標準入力で指定せずに, 自動で生成します. 引数には文字列の長さ (2^n) を指定します.
-h: hint: ヒントを表示します.
to_ip: 送信先の ip アドレスを指定します.
Usage: %s [-a] to_ip ...
)");
          break;

        case 'o':
          if (optarg) {
            out_name = optarg;
          }
          break;

        default: /* '?' */
          // 指定していないオプションが渡された場合
          printf("Usage: %s [-a] arg1 ...\n", argv[0]);
          break;
      }
    }
    // arg1: to_ip: 送信先の ip アドレスを指定します.
    if (optind < argc) {
      serv_ip = argv[optind];
    } else {
      string serv_ip = "127.0.0.1";  // ループバックアドレス
      // printf("IP address is not provided.\n");
      // return -1;
    }
  }

  // ファイル出力
  if (out_name == "") {
    out_name = "out.txt";
  }

  std::ofstream ofs(out_name);

  if (!ofs.is_open()) {
    printf("Failed to open a file.\n");
    return -1;
  }

  // パラメータの初期化
  struct sockaddr_in serv_addr;  // アドレス構造体
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

  if (option_auto == true) {
    random_device rd;
    mt19937_64 mt(rd());
    uniform_int_distribution<char> cU(30, 80);
    for (size_t i = 0; i < pow(2, option_auto_length); i++) {
      char c = cU(mt);
      msg.push_back(c);
    }
    msg.push_back('\0');
  }

  for (;;) {
    std::time_t now, later;

    if (option_auto == true) {
      std::cout << "-a オプションが有効のため自動で生成した文字列を送信します."
                << std::endl;
      std::cout << "length: 2^" << option_auto_length << std::endl;
      std::cout << "string: " << std::endl;
      std::cout << msg << std::endl;

      ofs << msg << std::endl;
    } else {
      cout << "input message: ";
      cin >> msg;
      if (msg == " q" || msg == "quit" || msg == "exit") {
        break;
      }
    }

    auto start = std::chrono::high_resolution_clock::now();
    time(&now);

    {  // departure
      // 文字列を分割して送信
      msg += char(4);  // EOT: end of transmission

      int cursor = 0;
      int msg_size = msg.size();

      while (cursor < msg_size) {
        int sending_size = min(BUFF_SIZE, msg_size - cursor);

        // 送信
        // buf: char 型配列の送信する先頭ポインタ
        n = sendto(socketd, msg.c_str() + cursor, sending_size + 1, 0,
                   (struct sockaddr*)&serv_addr, sizeof(serv_addr));
        if (n < 0) {
          cout << "Failed to send a message.\n";
          return -1;
        }

        cursor += sending_size;
      }
    }

    // 受信
    string recv_msg = "";
    bool is_end = false;
    {                  // arrival
      int cursor = 0;  // バッファのカーソル
      char snippet[BUFF_SIZE];

      while (!is_end) {
        n = recvfrom(socketd, snippet, sizeof(snippet), 0, NULL, NULL);
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

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    time(&later);

    cout << "Echo: " << endl;
    cout << recv_msg << "\n";
    ofs << recv_msg << std::endl;

    double seconds = std::difftime(later, now);
    cout << "RTT: " << elapsed.count() << " ms\n";

    ofs << elapsed.count() << std::endl;

    if (option_auto == true) {
      break;
    };
  }

  // ソケットを閉じる
  close(socketd);

  ofs.close();

  return 0;
}
