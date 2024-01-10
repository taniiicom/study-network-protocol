//
// 情報通信応用実験 ネットワークプログラミング
//
// 首都大学東京 システムデザイン学部 情報通信システムコース
// 准教授・酒井和哉
// ２０１８年４月５日
//
// 東京都立大学　情報科学科　柴田祐樹
// ２０２０年１０月改訂
//
// https://stackoverflow.com/questions/997946/how-to-get-current-time-and-date-in-c

// 以下の通りコンパイルして実行．
// g++ -std=c++14 ex_gettimeofday.cpp -o gettimeofday
// ./gettimeofday

#include <iostream>
#include <chrono>
#include <ctime>

int main()
{
    using namespace std;
    using namespace chrono;
    cout << "ex_gettimeofday v2.1.0\n";
    cout << "please input something\n";

    auto start = system_clock::now();
    // Some computation here
    string str;
    cout << ">> ";
    cin >> str;
    auto end = system_clock::now();

    duration<double> elapsed_seconds = end - start;

    time_t end_time = system_clock::to_time_t(end);

    // 処理にかかった時間を計算
    // tv_sec には秒単位の時間が格納されている。
    // tv_usec には1/1000000秒単位の時間が格納されている。

    cout << "input string: " << str << "\n";
    cout << "finished computation at " << ctime(&end_time)
         << "elapsed time to input: " << elapsed_seconds.count() << " s" << endl;
}
