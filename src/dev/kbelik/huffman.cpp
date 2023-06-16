#include <queue>

#include "dev/kbelik/huffman.h"

namespace linpipe::kbelik {
HuffmanTree::HuffmanTree() { 
  add_symbol(end_symbol);
  before_build[end_symbol].w = 0;
}

void HuffmanTree::add(string text) {
  for (char c: text) {
    add_symbol({c});
  }
}
void HuffmanTree::add_symbol(char symbol) {
  if (before_build.find(symbol) == before_build.end()) 
    before_build[symbol] = {nullptr, nullptr, symbol, 0, creation_time++};
  before_build[symbol].w++;
}

Node HuffmanTree::merge(Node& l, Node& r) {
  uint64_t tot_w = l.w + l.w;
  return {make_shared<Node>(l), make_shared<Node>(r), (char)0, tot_w, creation_time++};
}

void HuffmanTree::build_tree() {
  if (before_build.size() < 2)
    throw LinpipeError("Cannot build an empty HuffmanTree. Are you sure you added text to the tree?");
  
  // Deterministic Node comparison.
  auto cmp = [](Node& n1, Node& n2) {
    return n1.w > n2.w || 
      (n1.w == n2.w && !n1.is_leaf() && n2.is_leaf()) ||
      (n1.w == n2.w && n1.is_leaf() == n2.is_leaf() && n1.creation_time > n2.creation_time);
  };
  priority_queue<Node, vector<Node>, decltype(cmp)> bq(cmp);
  for (auto p: before_build) {
    bq.push(p.second);
  }
  while (bq.size() > 1) {
    Node l = bq.top();
    bq.pop();
    Node r = bq.top();
    bq.pop();

    bq.push(merge(l, r));
  }
  root = make_shared<Node>(bq.top());
}

void HuffmanTree::dfs(shared_ptr<Node> n, vector<byte>& bits) {
  if (n->is_leaf()) {
    auto to_store = vector<byte>(bits.size());
    copy(bits.begin(), bits.end(), to_store.begin());
    paths[n->val] = to_store;
    return ;
  }
  if (n->left) {
    bits.push_back((byte)0);
    dfs(n->left, bits);
    bits.pop_back();
  }
  if (n->right) {
    bits.push_back((byte)1);
    dfs(n->right, bits);
    bits.pop_back();
  }
}

void HuffmanTree::build_paths() {
  vector<byte> bits = {};
  dfs(root, bits);
}

void HuffmanTree::build() {
  if (is_built) 
    throw LinpipeError("Tree is already built");
  build_tree();
  build_paths();
  is_built = true;
}

void HuffmanTree::encode(string text, vector<byte>& out) {
  out = vector<byte>();
  int bit_idx = 0;
  byte b = (byte)0;

  auto encode_symbol = [&](const char symbol) {
    for(byte bit: paths[symbol]) {
      if (bit == (byte)1) {
        b |= bit << bit_idx;
      }
      bit_idx = (bit_idx + 1) % 8;
      if (bit_idx == 0) {
        out.push_back(b);      
        b = (byte)0;
      }
    }
  };

  for (auto c: text) {
    encode_symbol({c});
  }
  encode_symbol(end_symbol);
  
  if(bit_idx > 0) 
    out.push_back(b);
}

void HuffmanTree::decode(byte* in, string& text) {
  int bit_idx = 0;
  text = "";
  shared_ptr<Node> in_tree = root;
  while (true) {
    if (in_tree->is_leaf()) {
      if (in_tree->val == end_symbol)
        break;
      text += in_tree->val;
      in_tree = root;
    }
    bool go_right = ((((int)*in) >> bit_idx)&1)==1;
    bit_idx = (bit_idx + 1) % 8;
    if (bit_idx == 0)
      in++;
    if (go_right)
      in_tree = in_tree -> right;
    else
      in_tree = in_tree -> left;
  }
}


void HuffmanTree::prefix_serialize(shared_ptr<Node> n, vector<byte>& result) const {
  auto serializeed = n->serialize();
  result.insert(result.end(), serializeed.begin(), serializeed.end());
  if (n->left)
    prefix_serialize(n->left, result);
  if (n->right)
    prefix_serialize(n->right, result);
}

void HuffmanTree::serialize(vector<byte>& to) const {
  if (!is_built) 
    throw LinpipeError("Tree is not built and cannot be serialized.");
  to.resize(0);
  prefix_serialize(root, to);
  to.push_back(end_dump_sign);
}

Node HuffmanTree::prefix_construct(byte*& in) const {
  if (*in == (byte) 1)
    return {nullptr, nullptr, (char)*(in + 1), 0, 0};
  Node n = {nullptr, nullptr, (char)0, 0, 0};
  if (*(in+2) != end_dump_sign) {
    in = in + 2;
    n.left = make_shared<Node>(prefix_construct(in));
  }
  if (*(in+2) != end_dump_sign) {
    in = in + 2;
    n.right = make_shared<Node>(prefix_construct(in));
  }
  return n;
}

void HuffmanTree::deserialize(byte* in) {
  root = make_shared<Node>(prefix_construct(in));
  build_paths();
  is_built = true;
}
  
} // linpipe::kbelik
