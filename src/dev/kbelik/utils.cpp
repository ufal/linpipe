#include "dev/kbelik/utils.h"

namespace linpipe::kbelik {

void split(const string& text, const string& delimiter, vector<string>& splitted) {
  vector<string> result;
  size_t start = 0;
  size_t end = 0;
  while((end = text.find(delimiter, start)) != string::npos) {
    result.push_back(text.substr(start, end - start));
    start = end + delimiter.size();
  }
  result.push_back(text.substr(start));
  splitted = result;
}

} // linpipe::kbelik
