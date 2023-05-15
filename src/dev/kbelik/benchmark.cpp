#include <chrono>
#include <fstream>

#include "common.h"

#include "dev/kbelik/dynamic_map.h"
#include "dev/kbelik/map_values/int4.h"
#include "dev/kbelik/persistent_map.h"

#define USE(x) asm volatile ("" : : "rm" (x))

using namespace linpipe;
using namespace linpipe::kbelik;

auto dm = DynamicMap<int, map_values::Int4>();
auto qids_path = filesystem::path("dev/kbelik/utils/qids.txt");

int n_of_finds = 10000;

vector<int> qids;

int string_to_id(string qid_string) {
  return stoi(qid_string.substr(1));
}

void fill_dm() {
  std::ifstream infile(qids_path);
  string line;
  while( infile >> line) {
    int qid = string_to_id(line);
    int rv = rand();
    dm.add(qid, rv);
    qids.push_back(qid);
  }
}

int random_qid() {
  return qids[rand() % qids.size()];
}

void save_dm() {
  filesystem::create_directories("./temp");
  ofstream ofs("temp/test_map.bin", ofstream::out | ofstream::binary);
  dm.save_map(ofs, test);
  ofs.close();
}

int main() {
  cout << "Starting benchmarking of persistent map find.\n";
  fill_dm();
  cout << "Dynamic map is filled.\n";
  save_dm();
  cout << "Dynamic map saved.\n";
  auto dm = DynamicMap<int, map_values::Int4>();
  cout << "Dynamic map released from memory.\n";

  filesystem::path fp("temp/test_map.bin");
  auto pm = PersistentMap<int, map_values::Int4>(fp);
  

  using chrono::high_resolution_clock;
  using chrono::duration_cast;
  using chrono::duration;
  using chrono::nanoseconds;
  auto t1 = high_resolution_clock::now();
  map_values::Int4::Type res;
  int n_of_finds_with_seeks = 0;
  for (int i = 0; i < n_of_finds; ++i) {
    auto tseek1 = high_resolution_clock::now();
    int qid = random_qid();
    //cout << qid << '\n';
    USE(pm.find(qid, res));
    //cout << su << '\n';
    auto tseek2 = high_resolution_clock::now();
    if (duration_cast<nanoseconds>(tseek2 - tseek1).count() > (100 * 100)) {
      n_of_finds_with_seeks++;
    }
  }
  auto t2 = high_resolution_clock::now();

  auto ns_int = duration_cast<nanoseconds>(t2 - t1);
  
  cout << "n of finds performed: " << n_of_finds << '\n';
  cout << "approximate n of seeks performed: " << n_of_finds_with_seeks << '\n';
  cout << "total time spent: " <<  ns_int.count() << "ns\n";
  cout << "average time spent per find: " << ns_int.count() / n_of_finds << "ns\n";

  return 0;
}
