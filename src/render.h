#ifndef RENDER_H
#define RENDER_H

#include <node.h>

struct Rect {
  float x;
  float y;
  float w;
  float h;
};


void render_treemap_3(struct TreeNode *data,
                      void (*render_func)(struct Rect, char *, char *, char *, int), struct Rect viewport);

void svg_renderer(struct Rect r, char *label1, char *label2, char *tooltip, int hue);

#endif
