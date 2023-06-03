#include <math.h>
#include <node.h>
#include <render.h>
#include <stdio.h>
#include <stdlib.h>
#include <util.h>

char *color_hsl(int hue, int saturation, int lightness) {
  char *color = malloc(20);
  sprintf(color, "hsl(%d, %d%%, %d%%)", hue, saturation, lightness);
  return color;
}

void svg_renderer(struct Rect r, char *label, char *tooltip, int hue) {
  char *color_fg = color_hsl(hue, 50, 20);
  char *color_bg = color_hsl(hue, 50, 70);

  static int lastHue = -1;
  if (lastHue != hue) {
    printf("<defs>\n");
    printf("   <linearGradient id=\"Gradient%d\" x1=\"0\" x2=\"1\" y1=\"0\" "
           "y2=\"1\">\n",
           hue);
    printf("      <stop offset=\"0%%\" stop-color=\"white\"/>\n");
    printf("      <stop offset=\"100%%\" stop-color=\"%s\"/>\n", color_bg);
    printf("   </linearGradient>\n");
    printf("</defs>\n");
    lastHue = hue;
  }

  printf("<g class=\"hover-element\" data-tooltip=\"%s\">\n", tooltip);

  printf("<rect class=\"solid\" fill=\"url(#Gradient%d)\" width=\"%f\" "
         "height=\"%f\" x=\"%f\" y=\"%f\" />\n",
         hue, r.w, r.h, r.x, r.y);

  printf("<rect stroke=\"%s\" stroke-width=\".001\" fill=\"none\" width=\"%f\" "
         "height=\"%f\" x=\"%f\" y=\"%f\" />\n",
         color_fg, r.w, r.h, r.x, r.y);

  if (r.h > .04 && r.w > .02 * strlen(label)) {
    printf("<text fill=\"%s\" font-size=\".03\" x=\"%f\" y=\"%f\" "
           "text-anchor=\"middle\" "
           "alignment-baseline=\"middle\">%s</text>\n",
           color_fg, r.x + r.w / 2, r.y + r.h / 2 + .03 / 2, label);
  }

  printf("</g>\n");

  free(color_fg);
  free(color_bg);
}

void render_cell(struct Rect r, struct TreeNode *data,
                 void (*render_func)(struct Rect, char *, char *, int)) {
  char label[100];
  sprintf(label, "%d", (int)data->data);
  char tooltip[100];
  sprintf(tooltip, "%s %d", data->label, (int)data->data);
  render_func(r, label, tooltip, 0);
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

void _render_treemap_3(struct TreeNode **data, int data_len,
                       void (*render_func)(struct Rect, char *, char *, int),
                       struct Rect bounds) {

  float data_sum = 0;
  for (int i = 0; i < data_len; i++) {
    data_sum += data[i]->data;
  }

  if (bounds.w > bounds.h) {

    float best_ratio = 0;
    int best_n = 1;
    float best_slice_sum = 0;

    // Try all possible slices
    for (int n = 1; n <= data_len; n++) {
      float slice_sum = 0;
      for (int i = 0; i < n; i++) {
        slice_sum += data[i]->data;
      }

      float average_normalized_ratio = 0;
      for (int j = 0; j < n; j++) {
        average_normalized_ratio +=
            aspect(bounds.w * slice_sum / data_sum,
                   bounds.h * data[j]->data / slice_sum);
      }
      average_normalized_ratio /= n;

      if (average_normalized_ratio > best_ratio) {
        best_ratio = average_normalized_ratio;
        best_n = n;
        best_slice_sum = slice_sum;
      }
    }

    // Split into two rectangles
    struct Rect r1 = bounds;
    r1.w *= best_slice_sum / data_sum;

    struct Rect r2 = bounds;
    r2.x += r1.w;
    r2.w -= r1.w;

    // Render
    for (int i = 0; i < best_n; i++) {
      float ratio = data[i]->data / best_slice_sum;
      struct Rect r = {r1.x, r1.y, r1.w, r1.h * ratio};

      render_cell(r, data[i], render_func);
      r1.y += r.h;
    }

    // Recurse
    if (data_len > best_n) {
      _render_treemap_3(data + best_n, data_len - best_n, render_func, r2);
      return;
    }
  } else {
    float best_ratio = 0;
    int best_n = 1;
    float best_slice_sum = 0;

    // Try all possible slices
    for (int n = 1; n <= data_len; n++) {
      float slice_sum = 0;
      for (int i = 0; i < n; i++) {
        slice_sum += data[i]->data;
      }

      float average_normalized_ratio = 0;
      for (int j = 0; j < n; j++) {
        average_normalized_ratio +=
            aspect(bounds.h * slice_sum / data_sum,
                   bounds.w * data[j]->data / slice_sum);
      }
      average_normalized_ratio /= n;

      if (average_normalized_ratio > best_ratio) {
        best_ratio = average_normalized_ratio;
        best_n = n;
        best_slice_sum = slice_sum;
      }
    }

    // Split into two rectangles
    struct Rect r1 = bounds;
    r1.h *= best_slice_sum / data_sum;

    struct Rect r2 = bounds;
    r2.y += r1.h;
    r2.h -= r1.h;

    // Render
    for (int i = 0; i < best_n; i++) {
      float ratio = data[i]->data / best_slice_sum;
      struct Rect r = {r1.x, r1.y, r1.w * ratio, r1.h};

      render_cell(r, data[i], render_func);
      r1.x += r.w;
    }

    // Recurse
    if (data_len > best_n) {
      _render_treemap_3(data + best_n, data_len - best_n, render_func, r2);
      return;
    }
  }
}

void render_treemap_3(struct TreeNode *data,
                      void (*render_func)(struct Rect, char *, char *, int)) {
  printf("<svg viewBox=\"0 0 1 1\" xmlns=\"http://www.w3.org/2000/svg\">\n");
  printf("<rect x=\"0\" y=\"0\" width=\"1\" height=\"1\" fill=\"pink\" />\n");

  qsort(data->children, data->num_children, sizeof(struct TreeNode *),
        compare_tree_desc);
  _render_treemap_3(data->children, data->num_children, render_func,
                    (struct Rect){0, 0, 1, 1});

  printf("</svg>\n");
}
