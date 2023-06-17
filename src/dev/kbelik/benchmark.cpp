#include <chrono>
#include <future>
#include <fstream>
#include <mutex>

#include "common.h"

#include "dev/kbelik/dynamic_map.h"
#include "dev/kbelik/map_values/agnostic_entity_info.h"
#include "dev/kbelik/persistent_map.h"

#define USE(x) asm volatile ("" : : "rm" (x))

using namespace linpipe;
using namespace linpipe::kbelik;

auto dm = DynamicMap<int, map_values::AgnosticEntityInfo>();
auto qids_path = filesystem::path("dev/kbelik/utils/qids.txt");
auto directory_path = filesystem::path("../../damuel_1.0/damuel_1.0_wikidata");

int n_of_finds = 10000;

vector<int> qids;

mutex map_mutex;

int string_to_id(string qid_string) {
  return stoi(qid_string.substr(1));
}

void insert_to_dm(int qid, AgnosticEntityInfo& aei) {
  lock_guard<std::mutex> lock(map_mutex);
  dm.add(qid, aei);
}

DynamicMap<int, map_values::AgnosticEntityInfo> process_page(filesystem::path p) {
  std::ifstream infile(p);
  string line;
  DynamicMap<int, map_values::AgnosticEntityInfo> m;
  while( getline(infile, line)) {
    //cout << line << endl;
    Json j = Json::parse(line);
    AgnosticEntityInfo aei = AgnosticEntityInfo(j["claims"]);
    string q_str = j["qid"];
    int qid = stoi(q_str.substr(1));
    m.add(qid, aei);
  }
  cout << "Processed entry: " << p << "\n";
  return m;
}

void fill_dm() {
  /*
  for (const auto &entry : filesystem::directory_iterator(directory_path)) 
    if (entry.is_regular_file())
      futures.emplace_back(async(launch::async, process_page, entry));
      */
  vector<future<DynamicMap<int, map_values::AgnosticEntityInfo>>> futures;
  for (const auto &entry : filesystem::directory_iterator(directory_path)) 
    if (entry.is_regular_file())
      futures.emplace_back(async(launch::async, process_page, entry));
  cout << "Adding everything together..\n";
  for (auto& future : futures) {
    auto m = future.get();
    dm.insert(m);
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
  auto dm = DynamicMap<int, map_values::AgnosticEntityInfo>();
  cout << "Dynamic map released from memory.\n";

  filesystem::path fp("temp/test_map.bin");
  auto pm = PersistentMap<int, map_values::AgnosticEntityInfo>(fp);
  

  using chrono::high_resolution_clock;
  using chrono::duration_cast;
  using chrono::duration;
  using chrono::nanoseconds;
  auto t1 = high_resolution_clock::now();
  map_values::AgnosticEntityInfo::Type res;
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
