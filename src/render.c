#include <math.h>
#include <node.h>
#include <stdio.h>
#include <render.h>
#include <stdlib.h>

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

struct Rect
_render_treemap(struct TreeNode **children, int data_size,
                struct Rect available_space, float *area_left, int start,
                int *end, void (*render_func)(struct Rect, char *, char *, int),
                int hue) {

  int best_i = 0;
  float best_i_score = -1;
  for (int i = 1; i < data_size - start; i++) {
    float sum = 0;
    for (int j = 0; j < i; j++) {
      sum += children[j + start]->data;
    }

    float w = available_space.w;
    float h = sum / (*area_left) * available_space.h;

    float sum_of_aspect_ratios = 0;
    for (int j = 0; j < i; j++) {
      float neww = w * children[j + start]->data / sum;

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

  if (best_i + start + 1 == data_size) {
    best_i++;
  }

  float sum = 0;
  for (int j = 0; j < best_i; j++) {
    sum += children[j + start]->data;
  }

  float x = available_space.x;
  float y = available_space.y;
  float w = available_space.w;
  float h = sum / (*area_left) * available_space.h;
  for (int j = 0; j < best_i; j++) {
    float neww = w * children[j + start]->data / sum;
    char label[100];
    sprintf(label, "%d", (int)children[j + start]->data);

    char tooltip[100];
    sprintf(tooltip, "%s %d", children[j + start]->label,
            (int)children[j + start]->data);

    int cell_hue = hue;
    if (cell_hue == -1) {
      cell_hue = rand() % 360;
    }

    render_func((struct Rect){x, y, neww, h}, label, tooltip, cell_hue);

    if (children[j + start]->num_children > 0) {
      struct Rect available_space2 = (struct Rect){x, y, neww, h};
      struct TreeNode *data = children[j + start];
      int start = 0;
      int end = 0;
      float area_left = data->data;

      qsort(data->children, data->num_children, sizeof(struct TreeNode *),
            compare_tree_asc);
      while (end != data->num_children) {
        available_space2 = _render_treemap(data->children, data->num_children,
                                           available_space2, &area_left, start,
                                           &end, render_func, cell_hue);
        start = end;
      }
    }
    x += neww;
    (*area_left) -= children[j + start]->data;
  }

  (*end) = best_i + start;
  return (struct Rect){available_space.x, available_space.y + h,
                       available_space.w, available_space.h - h};
}

void render_treemap(struct TreeNode *data,
                    void (*render_func)(struct Rect, char *, char *, int)) {
  printf("<svg viewBox=\"0 0 1 1\" xmlns=\"http://www.w3.org/2000/svg\">\n");
  float area_left = 0;
  for (int i = 0; i < data->num_children; i++) {
    area_left += data->children[i]->data;
  }

  int end = 0;
  int start = 0;

  struct Rect available_space = {0, 0, 1, 1};

  qsort(data->children, data->num_children, sizeof(struct TreeNode *),
        compare_tree_asc);
  while (end != data->num_children) {
    available_space =
        _render_treemap(data->children, data->num_children, available_space,
                        &area_left, start, &end, render_func, -1);
    start = end;
  }
  printf("</svg>\n");
}
