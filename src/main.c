#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

struct Rect _render_treemap(struct TreeNode **children, int data_size, struct Rect available_space,
    float *area_left, int start, int *end, void (*render_func)(struct Rect, char *, int), int hue) {

  int best_i = 0;
  float best_i_score = -1;
  for (int i = 1; i < data_size-start; i++) {
    float sum = 0;
    for (int j = 0; j < i; j++) {
      sum += children[j+start]->data;
    }

    float score = 0;
    float x = available_space.x;
    float y = available_space.y;
    float w = available_space.w;
    float h = sum / (*area_left) * available_space.h;

    float sum_of_aspect_ratios = 0;
    for (int j = 0; j < i; j++) {
      float neww = w * children[j+start]->data / sum;

      float aspect_ratio = neww / h;
      sum_of_aspect_ratios += aspect_ratio;
    }

    float average_aspect_ratio = sum_of_aspect_ratios / i;
    float one_delta = fabsf(1 - average_aspect_ratio);

    if (one_delta < best_i_score || best_i_score == -1) {
      best_i = i;
      best_i_score = one_delta;
    }
  }

  if (best_i+start+1==data_size) {
    best_i++;
  }

  float sum = 0;
  for (int j = 0; j < best_i; j++) {
    sum += children[j+start]->data;
  }

  float x = available_space.x;
  float y = available_space.y;
  float w = available_space.w;
  float h = sum / (*area_left) * available_space.h;
  for (int j = 0; j < best_i; j++) {
    float neww = w * children[j+start]->data / sum;
    char buf[100];
    sprintf(buf, "%d", (int)children[j+start]->data);

    int cell_hue = hue;
    if (cell_hue == -1) {
      cell_hue=rand()%360;
    }

    render_func((struct Rect){x, y, neww, h}, buf, cell_hue);

    if (children[j+start]->num_children > 0) {
      struct Rect available_space2 = (struct Rect){x, y, neww, h};
      struct TreeNode *data = children[j+start];
      int start = 0;
      int end = 0;
      float area_left = data->data;

      qsort(data->children, data->num_children, sizeof(struct TreeNode *), compare_tree_asc);
      while (end != data->num_children) {
        available_space2 = _render_treemap(data->children, data->num_children, available_space2, &area_left, start, &end, render_func, cell_hue);
        start = end;
      }
    }
    x += neww;
    (*area_left)-=children[j+start]->data;
  }

  (*end)=best_i+start;
  return (struct Rect){available_space.x, available_space.y + h,
    available_space.w, available_space.h - h};
}

void render_treemap(struct TreeNode *data, void (*render_func)(struct Rect, char *, int)) {
  printf("<svg viewBox=\"0 0 1 1\" xmlns=\"http://www.w3.org/2000/svg\">\n");
  float area_left = 0;
  for (int i = 0; i < data->num_children; i++) {
    area_left += data->children[i]->data;
  }

  int end = 0;
  int start = 0;

  struct Rect available_space = {0, 0, 1, 1};

  qsort(data->children, data->num_children, sizeof(struct TreeNode *), compare_tree_asc);
  while (end != data->num_children) {
    available_space = _render_treemap(data->children, data->num_children, available_space, &area_left, start, &end, render_func, -1);
    start = end;
  }
  printf("</svg>\n");
}

struct TreeNode *init_data() {
  struct TreeNode *data = new_node(0, 0);

  for (int i = 0; i < 5; i++) {
    int n = rand() % 100 + 1;
    struct TreeNode *foo = new_node(0, 0);
    float sum = 0;
    for (int i = 0; i < n; i++) {
      float d = rand() % 100 + 1;
      add_child(foo, new_node(d, 0));
      sum += d;
    }
    foo->data = sum;
    add_child(data, foo);
  }

  return data;
}

void svg_renderer(struct Rect r, char *label, int hue) {
  char *color_fg = color_hsl(hue, 50, 20);
  char *color_bg = color_hsl(hue, 50, 70);

  int id=rand();

  printf("<defs>\n");
  printf("   <linearGradient id=\"Gradient%d\" x1=\"0\" x2=\"1\" y1=\"0\" y2=\"1\">\n", id);
  printf("      <stop offset=\"0%%\" stop-color=\"white\"/>\n");
  printf("      <stop offset=\"100%%\" stop-color=\"%s\"/>\n", color_bg);
  printf("   </linearGradient>\n");
  printf("</defs>\n");

  printf("<g class=\"hover-element\" data-tooltip=\"%s\">\n", label);

  printf(
      "<rect class=\"solid\" fill=\"url(#Gradient%d)\" width=\"%f\" height=\"%f\" x=\"%f\" y=\"%f\" />\n",
      id, r.w, r.h, r.x, r.y);

  printf(
      "<rect stroke=\"%s\" stroke-width=\".001\" fill=\"none\" width=\"%f\" height=\"%f\" x=\"%f\" y=\"%f\" />\n",
      color_fg, r.w, r.h, r.x, r.y);


  printf("<text fill=\"%s\" font-size=\".03\" x=\"%f\" y=\"%f\" text-anchor=\"middle\" "
      "alignment-baseline=\"middle\">%s</text>\n",
      "black", r.x + r.w / 2, r.y + r.h / 2, label);
  printf("</g>\n");

  free(color_fg);
  free(color_bg);
}

int main() {
  struct TreeNode *data = init_data();

  srand(time(NULL));

  render_treemap(data, svg_renderer);
}
