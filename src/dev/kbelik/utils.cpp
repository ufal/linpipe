#include "dev/kbelik/utils.h"

namespace linpipe::kbelik {

void split(const string& text, const string& delimiter, vector<string>& splitted) {
  splitted = vector<string>();
  size_t start = 0;
  size_t end = 0;
  while((end = text.find(delimiter, start)) != string::npos) {
    splitted.push_back(text.substr(start, end - start));
    start = end + delimiter.size();
  }
  splitted.push_back(text.substr(start));
}

} // linpipe::kbelik
