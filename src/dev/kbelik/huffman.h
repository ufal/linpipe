#pragma once

#include <unordered_map>

#include "common.h"

namespace linpipe::kbelik {

struct Node {
  shared_ptr<Node> left = nullptr;
  shared_ptr<Node> right = nullptr;
  string val; 
  uint64_t w;
  uint16_t creation_time;

  bool is_leaf() {
    return !left && !right;
  }
};


class HuffmanTree {
  public:
    HuffmanTree();
    // HuffmanTree(byte* from);

    void add(string text);
    void add_token(string text);
    void build();

    void encode(string text, vector<byte>& out);
    void decode(byte* in, string& text);

    // void dump(vector<byte> to) const;
  private:
    string end_symbol = "ENDOFTEXT";
    uint16_t creation_time = 0;
    bool is_built = false;
    unordered_map<string, Node> before_build;
    unordered_map<string, vector<byte>> paths;
    shared_ptr<Node> root;

    Node merge(Node& l, Node& r);
    void build_paths();
    void build_tree();
    void dfs(shared_ptr<Node> n, vector<byte>& bits);

    // void load(byte* from);
};

} // linpipe::kbelik
