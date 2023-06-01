#include "node.h"
#include <stdlib.h>

struct TreeNode *new_node(float data, int children_count) {
  struct TreeNode *node = malloc(sizeof(struct TreeNode));
  node->data = data;
  node->children = malloc(sizeof(struct TreeNode *) * children_count);
  node->num_children = children_count;
  return node;
}

struct TreeNode *add_child(struct TreeNode *parent, struct TreeNode *child) {
  parent->children = realloc(parent->children, sizeof(struct TreeNode *) *
                                                   (parent->num_children + 1));
  parent->children[parent->num_children++] = child;

  return child;
}
