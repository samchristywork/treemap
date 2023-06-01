#ifndef NODE_H
#define NODE_H

struct TreeNode {
  float data;
  int num_children;
  char *label;
  struct TreeNode **children;
};

struct TreeNode *new_node(float data, int children_count);
struct TreeNode *add_child(struct TreeNode *parent, struct TreeNode *child);

#endif
