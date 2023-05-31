#include <stdio.h>
#include <stdlib.h>

struct Rect {
  float x;
  float y;
  float w;
  float h;
};

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
}

char *color_hsl(int hue, int saturation, int lightness) {
  char *color = malloc(20);
  sprintf(color, "hsl(%d, %d%%, %d%%)", hue, saturation, lightness);
  return color;
}

int compare_tree_desc(const void *a, const void *b) {
  struct TreeNode *fa = *(struct TreeNode **)a;
  struct TreeNode *fb = *(struct TreeNode **)b;
  return (fa->data < fb->data) - (fa->data > fb->data);
}

int compare_tree_asc(const void *a, const void *b) {
  struct TreeNode *fa = *(struct TreeNode **)a;
  struct TreeNode *fb = *(struct TreeNode **)b;
  return (fa->data > fb->data) - (fa->data < fb->data);
}

struct TreeNode *init_data() {
}

int main() {
  init_data();

  render_treemap();
}
