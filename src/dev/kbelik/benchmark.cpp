#include "dev/kbelik/agnostic_kbelik.h"
#include "dev/kbelik/specific_kbelik.h"
#include "dev/kbelik/map_keys/qid4.h"
#include "dev/kbelik/map_keys/qid8.h"
#include "dev/kbelik/map_values/agnostic_entity_info_huff.h"
#include "dev/kbelik/map_values/specific_entity_info_huff.h"

#define USE(x) asm volatile ("" : : "rm" (x))

using namespace linpipe;
using namespace linpipe::kbelik;

auto combined_path = filesystem::path("../../damuel_1.0/damuel_wikidata_medium/combined");

int n_of_finds = 10000;

vector<int> qids;

int random_qid() {
  return qids[rand() % qids.size()];
}

int main() {
  cout << "Starting benchmarking of kbelik find.\n";

  ifstream jsons(combined_path, ifstream::in | ifstream::binary);
  ofstream ofs("temp/test_agnostic_kbelik.bin", ofstream::out | ofstream::binary);
  AgnosticKbelik<map_keys::QID8>::build(jsons, ofs);
  ofs.close();
  filesystem::path fp("temp/test_agnostic_kbelik.bin");

  cout << "Kbelik build and saved.\n";
  auto ak = AgnosticKbelik<map_keys::QID8>(fp);
  

  using chrono::high_resolution_clock;
  using chrono::duration_cast;
  using chrono::duration;
  using chrono::nanoseconds;
  auto t1 = high_resolution_clock::now();
  map_values::AgnosticEntityInfoH::Type res;
  int n_of_finds_with_seeks = 0;
  for (int i = 0; i < n_of_finds; ++i) {
    auto tseek1 = high_resolution_clock::now();
    //cout << qid << '\n';
    USE(ak.find(ID(i+35000), res));
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
