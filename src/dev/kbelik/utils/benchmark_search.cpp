#include <functional>
#include <fstream>
#include "common.h"
#include "search.cpp"

#define dump(x) cout << #x << " = " << x << endl;

using namespace linpipe;

typedef function<int(int[], int, int)> search_func;

vector<pair<search_func, string>> to_test = {
	pair<search_func, string>(binary, "binary search"),
	pair<search_func, string>(expo2, "exponential searc2"),
	pair<search_func, string>(expo3, "exponential search3"),
	pair<search_func, string>(expo4, "exponential search4"),
	pair<search_func, string>(expo8, "exponential search8"),
	pair<search_func, string>(expo64, "exponential search64"),
	pair<search_func, string>(interpolated, "interpolated search")
};

constexpr int ARR_SIZE = 35000000;
constexpr int SMALL = 1000;
constexpr int BIG = 10000000;
constexpr int N_OF_TRIES = 100;
string QIDS_PATH = "qids.txt";
int arr[ARR_SIZE];

int qids_cnt = 0;

void fill_ascending(int n) {
  for (int i = 0; i < n; ++i)
    arr[i] = i;
}

void fill_even(int n) {
  for (int i = 0; i < n; ++i)
    arr[i] = i * 2;
}

void fill_break_interpolated(int n) {
  fill_ascending(n);
  arr[n - 1] = ARR_SIZE;
}

void benchmark(int n, int ma) {
  for (auto p: to_test) {
    int tot = 0;
    for (int i = 0; i < N_OF_TRIES; ++i) {
      int val = rand() & ma;
      //cout << val << '\n';
      int accesses = p.first(arr, n, val);
      //cout << "accesses: " << accesses << '\n';
      tot += accesses;
    }
    cout << p.second << '\n';
    cout << tot / N_OF_TRIES << '\n';
    cout << "---------------------------\n";
  }
}

void benchmark_present(int n) {
  fill_ascending(n);
  benchmark(n, n);
}

void benchmark_missing(int n) {
  fill_even(n);
  benchmark(n, 3 * n);
}

void benchmark_small() {
  cout << "BENCHMARK SMALL\n";
  dump(SMALL);
  benchmark_present(SMALL);
}

void benchmark_big() {
  cout << "BENCHMARK BIG\n";
  dump(BIG);
  benchmark_present(BIG);
}

void benchmark_big_missing() {
  cout << "BENCHMARK BIG MISSING\n";
  dump(BIG);
  benchmark_missing(BIG);
}

void benchmark_small_missing() {
  cout << "BENCHMARK SMALL MISSING\n";
  dump(SMALL);
  benchmark_missing(SMALL);
}

int get_id(string line) {
  line.erase(0, 1);
  return stoi(line);
}

void check_arr(int n = 20) {
  cout << "Printing first " << n << " elements of the array\n";
  for (int i = 0; i < n; ++i) {
    cout << arr[i] << ' ';
  }
  cout << endl;
}

void load_qids() {
  qids_cnt = 0;
  string line;
  ifstream f(QIDS_PATH);
  if (f.is_open()) {
    while (getline(f,line)){
      int id = get_id(line);
      arr[qids_cnt++] = id;
    }
    f.close();
  }
  sort(arr, arr + qids_cnt);
  check_arr();
}

void benchmark_qids() {
  cout << "BENCHMARK QIDS PRESENT\n";
  dump(qids_cnt);
  for (auto p: to_test) {
    int tot = 0;
    for (int i = 0; i < N_OF_TRIES; ++i) {
      int val = arr[rand() & qids_cnt];
      int accesses = p.first(arr, qids_cnt, val);
      tot += accesses;
    }
    cout << p.second << '\n';
    cout << tot / N_OF_TRIES << '\n';
    cout << "---------------------------\n";
  }
}

void benchmark_qids_missing() {
  cout << "BENCHMARK QIDS MISSING\n";
  dump(qids_cnt);
  int ma = 0;
  for (int i = 0; i < qids_cnt; ++i) {
    ma = max(arr[i], ma);
  }
  dump(ma);
  for (auto p: to_test) {
    int tot = 0;
    for (int i = 0; i < N_OF_TRIES; ++i) {
      int val = rand() % ma;
      int accesses = p.first(arr, qids_cnt, val);
      tot += accesses;
    }
    cout << p.second << '\n';
    cout << tot / N_OF_TRIES << '\n';
    cout << "---------------------------\n";
  }
}
