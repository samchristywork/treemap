#ifndef RENDER_H
#define RENDER_H

#include <node.h>

struct Rect {
  float x;
  float y;
  float w;
  float h;
};

void render_treemap(struct TreeNode *data,
                    void (*render_func)(struct Rect, char *, char *, int));

#endif
