#pragma once
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>
#include <pybind11/pybind11.h>

namespace py = pybind11;

template <typename T>
bool KeyLessImpl(const T& left, const T& right) {
  return left < right;
}

inline bool KeyLessImpl(const pybind11::object& left, const pybind11::object& right) {
    const int result = PyObject_RichCompareBool(left.ptr(), right.ptr(), Py_LT);
    if (result == -1) {
        throw pybind11::error_already_set();
    }
    return result == 1;
}



template <typename Key, typename Value>
struct Node {
  Key key;
  Value val;

  Node* l = nullptr;
  Node* r = nullptr;
  Node* parent = nullptr;

  Node(const Key& key, const Value& val)
      : key(key), val(val), l(nullptr), r(nullptr), parent(nullptr) {}
};

template <typename Key, typename Value>
struct SplayTree {
 private:
  using NodeType = Node<Key, Value>;

  NodeType* root_ = nullptr;
  size_t size_ = 0;

  static bool Exists(NodeType* cur) {
    return cur != nullptr;
  }

  bool Less(const Key& left, const Key& right) const {
    return KeyLessImpl(left, right);
  }

  bool Equal(const Key& left, const Key& right) const {
    return !Less(left, right) && !Less(right, left);
  }

  void SetLeft(NodeType* parent, NodeType* child) {
    if (Exists(parent)) {
      parent->l = child;
    }
    if (Exists(child)) {
      child->parent = parent;
    }
  }

  void SetRight(NodeType* parent, NodeType* child) {
    if (Exists(parent)) {
      parent->r = child;
    }
    if (Exists(child)) {
      child->parent = parent;
    }
  }

  void LeftRotation(NodeType* cur_vertex) {
    NodeType* cur = cur_vertex->r;
    NodeType* grand = cur_vertex->parent;
    SetRight(cur_vertex, cur->l);
    SetLeft(cur, cur_vertex);
    cur->parent = grand;
    if (Exists(grand)) {
      if (grand->l == cur_vertex) {
        grand->l = cur;
      } else {
        grand->r = cur;
      }
    }
  }

  void RightRotation(NodeType* cur_vertex) {
    NodeType* cur = cur_vertex->l;
    NodeType* grand = cur_vertex->parent;
    SetLeft(cur_vertex, cur->r);
    SetRight(cur, cur_vertex);
    cur->parent = grand;
    if (Exists(grand)) {
      if (grand->l == cur_vertex) {
        grand->l = cur;
      } else {
        grand->r = cur;
      }
    }
  }

  NodeType* Splay(NodeType* cur) {
    if (!Exists(cur)) {
      return cur;
    }
    while (Exists(cur->parent)) {
      NodeType* par = cur->parent;
      NodeType* grand = par->parent;
      if (cur == par->l) {
        if (!Exists(grand)) {
          RightRotation(par);
        } else if (par == grand->l) {
          RightRotation(grand);
          RightRotation(par);
        } else {
          RightRotation(par);
          LeftRotation(grand);
        }
      } else {
        if (!Exists(grand)) {
          LeftRotation(par);
        } else if (par == grand->r) {
          LeftRotation(grand);
          LeftRotation(par);
        } else {
          LeftRotation(par);
          RightRotation(grand);
        }
      }
    }
    root_ = cur;
    return cur;
  }

  NodeType* FindClosest(NodeType* cur, const Key& key) {
    if (!Exists(cur)) {
      return nullptr;
    }

    NodeType* cur_ans = nullptr;
    if (Equal(cur->key, key)) {
      cur_ans = cur;
    }
    if (Less(cur->key, key)) {
      cur_ans = FindClosest(cur->r, key);
    }
    if (Less(key, cur->key)) {
      cur_ans = FindClosest(cur->l, key);
    }
    return Exists(cur_ans) ? cur_ans : cur;
  }

  NodeType* FindExact(NodeType* cur, const Key& key) {
    while (Exists(cur)) {
      if (Equal(cur->key, key)) {
        return cur;
      }
      if (Less(cur->key, key)) {
        cur = cur->r;
      } else {
        cur = cur->l;
      }
    }
    return nullptr;
  }

  std::pair<NodeType*, NodeType*> Split(NodeType* cur, const Key& key) {
    if (!Exists(cur)) {
      return {nullptr, nullptr};
    }

    cur = FindClosest(cur, key);
    Splay(cur);

    NodeType* left;
    NodeType* right;

    if (!Less(cur->key, key)) {
      left = cur->l;
      cur->l = nullptr;
      if (Exists(left)) {
        left->parent = nullptr;
      }
      right = cur;
    } else {
      right = cur->r;
      cur->r = nullptr;
      if (Exists(right)) {
        right->parent = nullptr;
      }
      left = cur;
    }

    return {left, right};
  }

  NodeType* Merge(NodeType* left, NodeType* right) {
    if (!Exists(left)) {
      return right;
    }
    if (!Exists(right)) {
      return left;
    }

    NodeType* cur = left;
    while (Exists(cur->r)) {
      cur = cur->r;
    }

    root_ = left;
    Splay(cur);
    root_->r = right;
    right->parent = root_;
    return root_;
  }

  static void CollectKeys(NodeType* cur, std::vector<Key>& keys) {
    if (!Exists(cur)) {
      return;
    }
    CollectKeys(cur->l, keys);
    keys.push_back(cur->key);
    CollectKeys(cur->r, keys);
  }

  static void CollectValues(NodeType* cur, std::vector<Value>& values) {
    if (!Exists(cur)) {
      return;
    }
    CollectValues(cur->l, values);
    values.push_back(cur->val);
    CollectValues(cur->r, values);
  }

  static void CollectItems(NodeType* cur,
                           std::vector<std::pair<Key, Value>>& items) {
    if (!Exists(cur)) {
      return;
    }
    CollectItems(cur->l, items);
    items.push_back({cur->key, cur->val});
    CollectItems(cur->r, items);
  }

 public:
  void Insert(const Key& key, const Value& val) {
    NodeType* cur = FindExact(root_, key);
    if (Exists(cur)) {
      root_ = Splay(cur);
      root_->val = val;
      return;
    }

    auto [left, right] = Split(root_, key);
    NodeType* cur_node = new NodeType(key, val);

    cur_node->l = left;
    if (Exists(left)) {
      left->parent = cur_node;
    }

    cur_node->r = right;
    if (Exists(right)) {
      right->parent = cur_node;
    }

    root_ = cur_node;
    root_->parent = nullptr;
    ++size_;
  }

  Value& Get(const Key& key) {
    NodeType* cur = FindExact(root_, key);
    if (!Exists(cur)) {
      throw std::out_of_range("Key does not exist");
    }
    root_ = Splay(cur);
    return root_->val;
  }

  void Erase(const Key& key) {
    NodeType* cur = FindExact(root_, key);
    if (!Exists(cur)) {
      return;
    }

    root_ = Splay(cur);

    NodeType* left = root_->l;
    NodeType* right = root_->r;

    if (Exists(left)) {
      left->parent = nullptr;
    }
    if (Exists(right)) {
      right->parent = nullptr;
    }

    delete root_;
    root_ = Merge(left, right);
    if (Exists(root_)) {
      root_->parent = nullptr;
    }
    --size_;
  }

  bool Contains(const Key& key) {
    NodeType* cur = FindExact(root_, key);
    if (!Exists(cur)) {
      return false;
    }
    root_ = Splay(cur);
    return true;
  }

  [[nodiscard]] size_t Size() const {
    return size_;
  }

  std::vector<Key> Keys() const {
    std::vector<Key> keys;
    keys.reserve(size_);
    CollectKeys(root_, keys);
    return keys;
  }

  std::vector<Value> Values() const {
    std::vector<Value> values;
    values.reserve(size_);
    CollectValues(root_, values);
    return values;
  }

  std::vector<std::pair<Key, Value>> Items() const {
    std::vector<std::pair<Key, Value>> items;
    items.reserve(size_);
    CollectItems(root_, items);
    return items;
  }

  static void Clear(NodeType* cur) {
    if (!Exists(cur)) {
      return;
    }
    Clear(cur->l);
    Clear(cur->r);
    delete cur;
  }

  ~SplayTree() {
    Clear(root_);
  }
};
