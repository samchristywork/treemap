#include <stdio.h>
#include <stdlib.h>

struct TreeNode {
  float data;
  int num_children;
  struct TreeNode **children;
};

struct TreeNode *new_node(float data, int children_count) {
  struct TreeNode *node = malloc(sizeof(struct TreeNode));
  node->data = data;
  node->children = malloc(sizeof(struct TreeNode *) * children_count);
  node->num_children = children_count;
  return node;
}

struct TreeNode *add_child(struct TreeNode *parent, struct TreeNode *child) {
  parent->children = realloc(parent->children,
      sizeof(struct TreeNode *) * (parent->num_children + 1));
  parent->children[parent->num_children++] = child;

  return child;
void render_treemap() {
}

void init_data() {
}

int main() {
  init_data();

  render_treemap();
}
