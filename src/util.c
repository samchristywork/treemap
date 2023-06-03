#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <util.h>

float aspect(float width, float height) {
  if (height / width < width / height) {
    return height / width;
  }
  return width / height;
}

float read_dir(struct TreeNode *data, char *path) {
  float size = 0;
  DIR *d;
  struct dirent *dir;
  d = opendir(path);
  if (d) {
    while ((dir = readdir(d)) != NULL) {
      if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) {
        continue;
      }

      if (dir->d_type == DT_DIR) {
        char dirname[PATH_MAX];
        sprintf(dirname, "%s%s/", path, dir->d_name);
        struct TreeNode *child = add_child(data, new_node(0, 0));
        float s = read_dir(child, dirname);
        child->data = s;
        child->label = strdup(dir->d_name);
        size += s;

      } else {
        char filename[PATH_MAX];
        sprintf(filename, "%s%s", path, dir->d_name);
        FILE *f = fopen(filename, "r");
        fseek(f, 0, SEEK_END);
        long fsize = ftell(f);
        fclose(f);

        struct TreeNode *child = add_child(data, new_node(fsize, 0));
        child->label = strdup(filename);
        size += fsize;
      }
    }
    closedir(d);
  }

  return size;
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
