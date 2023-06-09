#include <math.h>
#include <render.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util.h>

void _render_treemap_3(struct TreeNode **data, int data_len,
                       void (*render_func)(struct Rect, char *, char *, char *, int),
                       struct Rect bounds, int hue);

char *color_hsl(int hue, int saturation, int lightness) {
  char *color = malloc(20);
  sprintf(color, "hsl(%d, %d%%, %d%%)", hue, saturation, lightness);
  return color;
}

void svg_renderer(struct Rect r, char *label1, char *label2, char *tooltip, int hue) {
  char *color_fg = color_hsl(hue, 50, 20);
  char *color_bg = color_hsl(hue, 50, 70);

  if (label1 == NULL) {
    label1 = "NULL";
  }

  if (label2 == NULL) {
    label2 = "NULL";
  }

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

  printf("<g class=\"hover-element\" data-tooltip=\"%s\" onclick=\"console.log('%s')\">\n", tooltip, tooltip);

  printf("<rect class=\"solid\" fill=\"url(#Gradient%d)\" width=\"%f\" "
         "height=\"%f\" x=\"%f\" y=\"%f\" />\n",
         hue, r.w, r.h, r.x, r.y);

  printf("<rect stroke=\"%s\" stroke-width=\".001\" fill=\"none\" width=\"%f\" "
         "height=\"%f\" x=\"%f\" y=\"%f\" />\n",
         color_fg, r.w, r.h, r.x, r.y);

  int num_rows=2;

  if (r.h < .06) {
    num_rows = 1;
  }

  if (r.w < .015 * strlen(label1) || r.w < .015 * strlen(label2)) {
    num_rows = 1;
  }

  if (r.h < .03) {
    num_rows = 0;
  }

  if (r.w < .015 * strlen(label1)) {
    num_rows = 0;
  }

  switch (num_rows) {
    case 2:
      printf("<text fill=\"%s\" font-size=\".03\" x=\"%f\" y=\"%f\" "
          "text-anchor=\"middle\" "
          "alignment-baseline=\"middle\">%s</text>\n",
          color_fg, r.x + r.w / 2, r.y + r.h / 2, label2);
      printf("<text fill=\"%s\" font-size=\".03\" x=\"%f\" y=\"%f\" "
          "text-anchor=\"middle\" "
          "alignment-baseline=\"middle\">%s</text>\n",
          color_fg, r.x + r.w / 2, r.y + r.h / 2 + .03, label1);
      break;
    case 1:
      printf("<text fill=\"%s\" font-size=\".03\" x=\"%f\" y=\"%f\" "
          "text-anchor=\"middle\" "
          "alignment-baseline=\"middle\">%s</text>\n",
          color_fg, r.x + r.w / 2, r.y + r.h / 2 + .03 / 2, label1);
  }

  printf("</g>\n");

  free(color_fg);
  free(color_bg);
}

void render_cell(struct Rect r, struct TreeNode *data,
                 void (*render_func)(struct Rect, char *, char *, char *, int), int hue) {

  if (hue == -1) {
    hue = rand() % 360;
  }

  if (data->num_children > 0) {

    _render_treemap_3(data->children, data->num_children, render_func,
        r, hue);
  } else {
    char label[100];
    sprintf(label, "%d", (int)data->data);
    char tooltip[100];
    sprintf(tooltip, "Name: %s<br>Value: %d", data->label, (int)data->data);
    render_func(r, label, data->label, tooltip, hue);
  }
}

void _render_treemap_3(struct TreeNode **data, int data_len,
                       void (*render_func)(struct Rect, char *, char *, char *, int),
                       struct Rect bounds, int hue) {

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

      if (data[i]->data==0) {
        continue;
      }
      render_cell(r, data[i], render_func, hue);
      r1.y += r.h;
    }

    // Recurse
    if (data_len > best_n) {
      _render_treemap_3(data + best_n, data_len - best_n, render_func, r2, hue);
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

      if (data[i]->data==0) {
        continue;
      }
      render_cell(r, data[i], render_func, hue);
      r1.x += r.w;
    }

    // Recurse
    if (data_len > best_n) {
      _render_treemap_3(data + best_n, data_len - best_n, render_func, r2, hue);
      return;
    }
  }
}

void sort_tree(struct TreeNode *data) {
  if (data->num_children == 0) {
    return;
  }

  qsort(data->children, data->num_children, sizeof(struct TreeNode *),
        compare_tree_desc);

  for (int i = 0; i < data->num_children; i++) {
    sort_tree(data->children[i]);
  }
}

void blue() {
  fprintf(stderr, "\033[0;34m");
}

void green() {
  fprintf(stderr, "\033[0;32m");
}

void clear() {
  fprintf(stderr, "\033[0m");
}

void print_tree(struct TreeNode *data) {
  static int depth = 0;
  for (int i = 0; i < depth; i++) {
    fprintf(stderr, "  ");
  }

  blue();
  fprintf(stderr, "%8.0f", data->data);
  clear();
  fprintf(stderr, " → ");
  green();
  fprintf(stderr, "%s", data->label);
  clear();
  fprintf(stderr, "\n");

  for (int i = 0; i < data->num_children; i++) {
    depth++;
    print_tree(data->children[i]);
    depth--;
  }
}

void render_treemap_3(struct TreeNode *data,
                      void (*render_func)(struct Rect, char *, char *, char *, int), struct Rect viewport) {
  printf("<svg viewBox=\"%f %f %f %f\" xmlns=\"http://www.w3.org/2000/svg\">\n",
      viewport.x, viewport.y,
      viewport.w, viewport.h
      );
  printf("<rect x=\"%f\" y=\"%f\" width=\"%f\" height=\"%f\" fill=\"pink\" />\n",
      viewport.x, viewport.y,
      viewport.w, viewport.h
      );

  sort_tree(data);
  _render_treemap_3(data->children, data->num_children, render_func,
                    viewport, -1);

  printf("</svg>\n");
}
