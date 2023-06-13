#ifndef UTIL_H
#define UTIL_H

#include <node.h>

float aspect(float width, float height);
float read_dir(struct TreeNode *data, char *path);
int compare_tree_desc(const void *a, const void *b);
int compare_tree_asc(const void *a, const void *b);
void append_format(char **str, const char *format, ...);

#endif
