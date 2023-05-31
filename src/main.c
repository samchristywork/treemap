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

void render_treemap(struct TreeNode *data, void (*render_func)(struct Rect, char *, int)) {
  printf("<svg viewBox=\"0 0 1 1\" xmlns=\"http://www.w3.org/2000/svg\">\n");
  printf("</svg>\n");
}

struct TreeNode *init_data() {
  struct TreeNode *data = new_node(0, 0);
  return data;
}

void svg_renderer(struct Rect r, char *label, int hue) {
  char *color = color_hsl(hue, 50, 70);

  int id=rand();

  printf(
      "<rect class=\"solid\" fill=\"%s\" width=\"%f\" height=\"%f\" x=\"%f\" y=\"%f\" />\n",
      color, id, r.w, r.h, r.x, r.y);

  printf(
      "<rect stroke=\"%s\" stroke-width=\".001\" fill=\"none\" width=\"%f\" height=\"%f\" x=\"%f\" y=\"%f\" />\n",
      "black", r.w, r.h, r.x, r.y);

  printf("<text fill=\"%s\" font-size=\".03\" x=\"%f\" y=\"%f\" text-anchor=\"middle\" "
      "alignment-baseline=\"middle\">%s</text>\n",
      "black", r.x + r.w / 2, r.y + r.h / 2, label);

  free(color);
}

int main() {
  init_data();

  render_treemap();
}
