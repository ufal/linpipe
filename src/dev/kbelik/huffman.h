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
  byte val; 
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
    cout <<(int)val << ' '<< w << ' ' << creation_time << endl;
  }
};

class HuffmanTree {
  public:
    HuffmanTree();

    void add(string text);
    void build();

    void encode(const vector<byte>& data, vector<byte>& out);
    void encode(const string text, vector<byte>& out);

    void decode(const byte* in, vector<byte>& text);
    void decode(const byte* in, string& text);

    void serialize(vector<byte>& to) const;

    void deserialize(byte* from);
    void deserialize(const vector<byte>& from);

    inline byte end_symbol() const { return (byte)0b11111111; }
    inline byte end_serialize_symbol() const { return (byte)0b11111110; }
  private:

    uint16_t creation_time = 0;
    bool is_built = false;
    unordered_map<byte, Node> before_build;
    unordered_map<byte, vector<byte>> paths;
    shared_ptr<Node> root;

    void add_symbol(byte symbol);

    Node merge(Node& l, Node& r);
    void build_paths();
    void build_tree();
    void dfs(shared_ptr<Node> n, vector<byte>& bits);

    void prefix_serialize(shared_ptr<Node> n, vector<byte>& result) const;
    Node prefix_construct(byte*& in) const;
};

} // linpipe::kbelik
