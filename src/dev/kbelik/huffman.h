// Huffman tree for strin serialization
// The tree can be loaded from byte* by deserialize method or created directly
// by calling add(text) as many times us needed.
// Once the tree is loaded/built it cannot be changed.
// end_symbol should never be present in the inputed text.

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

  vector<byte> serialize() {
    auto result = vector<byte>(2, (byte)0);
    result[0] = is_leaf() ? (byte)1 : (byte) 0;
    if (is_leaf())
      result[1] = (byte) val;
    return result;
  }

  void print() {
    cout << val << ' '<< w << ' ' << creation_time << endl;
  }
};

class HuffmanTree {
  public:
    HuffmanTree();

    void add(string text);
    void build();

    void encode(string text, vector<byte>& out);
    void decode(byte* in, string& text);

    void serialize(vector<byte>& to) const;

    void deserialize(byte* from);

  private:
    const char end_symbol = (char)0b11111111;  // <DEL>
    const byte end_dump_sign = (byte)0b11111111;

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

    void prefix_serialize(shared_ptr<Node> n, vector<byte>& result) const;
    Node prefix_construct(byte*& in) const;
};

} // linpipe::kbelik
