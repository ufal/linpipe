#pragma once

#include <unordered_map>

#include "common.h"

namespace linpipe::kbelik {

struct Node {
  shared_ptr<Node> left = nullptr;
  shared_ptr<Node> right = nullptr;
  char val; 
  uint64_t w;
  uint16_t creation_time;

  bool is_leaf() {
    return left == nullptr && right == nullptr;
  }
  vector<byte> dump() {
    auto result = vector<byte>(2, (byte)0);
    result[0] = is_leaf() ? (byte)1 : (byte) 0;
    if (is_leaf()) {
      result[1] = (byte) val;
    }
    return result;
  }

  void print() {
    cout << val << ' '<< w << ' ' << creation_time << endl;
  }
};


class HuffmanTree {
  public:
    HuffmanTree();
    HuffmanTree(byte* from);

    void add(string text);
    void build();

    void encode(string text, vector<byte>& out);
    void decode(byte* in, string& text);

    void dump(vector<byte>& to) const;

  private:
    char end_symbol = (char)0b11111111;  // <DEL>
    uint16_t creation_time = 0;
    bool is_built = false;
    unordered_map<char , Node> before_build;
    unordered_map<char , vector<byte>> paths;
    shared_ptr<Node> root;

    void add_symbol(char symbol);

    Node merge(Node& l, Node& r);
    void build_paths();
    void build_tree();
    void dfs(shared_ptr<Node> n, vector<byte>& bits);

    void prefix_dump(shared_ptr<Node> n, vector<byte>& result) const;
    Node prefix_construct(byte*& in) const;

    void load(byte* from);
};

} // linpipe::kbelik
