#ifndef CUSTOM_BSTMAP_H
#define CUSTOM_BSTMAP_H

#include <iostream>

template <typename K, typename V> class BSTMap {
private:
  struct Node {
    K key;
    V value;
    Node *left;
    Node *right;

    Node(const K &k, const V &v)
        : key(k), value(v), left(nullptr), right(nullptr) {}
    Node(const K &k) : key(k), value(), left(nullptr), right(nullptr) {}
  };

  Node *root;

  Node *insert(Node *node, const K &key) {
    if (!node) {
      return new Node(key);
    }
    if (key < node->key) {
      node->left = insert(node->left, key);
    } else if (key > node->key) {
      node->right = insert(node->right, key);
    }
    return node;
  }

  Node *find(Node *node, const K &key) {
    if (!node || node->key == key) {
      return node;
    }
    if (key < node->key) {
      return find(node->left, key);
    }
    return find(node->right, key);
  }

  void clear(Node *node) {
    if (!node)
      return;
    clear(node->left);
    clear(node->right);
    delete node;
  }

  // Iterate In-Order
  template <typename Func> void inorder(Node *node, Func func) {
    if (!node)
      return;
    inorder(node->left, func);
    func(node->key, node->value);
    inorder(node->right, func);
  }

public:
  BSTMap() : root(nullptr) {}

  ~BSTMap() { clear(root); }

  V &operator[](const K &key) {
    Node *node = find(root, key);
    if (node) {
      return node->value;
    }
    // Key not found, insert default
    if (!root) {
      root = new Node(key);
      return root->value;
    } else {
      // Need to insert properly
      // We use simple insert logic here, not balanced (ok for assignment)
      // But we need to return the reference to the new node's value
      // So we re-implement insert to return the node found/created
      Node **current = &root;
      while (*current) {
        if (key < (*current)->key) {
          current = &((*current)->left);
        } else if (key > (*current)->key) {
          current = &((*current)->right);
        } else {
          return (*current)->value;
        }
      }
      *current = new Node(key);
      return (*current)->value;
    }
  }

  bool contains(const K &key) { return find(root, key) != nullptr; }

  void clear() {
    clear(root);
    root = nullptr;
  }

  // Helper to iterate (since we don't have standard iterators easily)
  template <typename Func> void forEach(Func func) { inorder(root, func); }

  // Needed to mimic iteration in SeasonManager (raceEvents loop)
  // This is tricky without full iterator support.
  // For now, we might need a way to get all keys into a vector to iterate?
  // Or we rely on 'forEach'
};

#endif // CUSTOM_BSTMAP_H
