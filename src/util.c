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
  char *ignore_list[] = {".", "..", ".git", NULL};
  data->label = strdup(path);

  static int path_len = 0;
  if (path_len == 0) {
    path_len = strlen(path);
  }

  float size = 0;
  struct dirent *dir;
  DIR *d = opendir(path);
  if (d) {
    while ((dir = readdir(d)) != NULL) {
      int ignore = 0;
      for (int i = 0; ignore_list[i] != NULL; i++) {
        if (strcmp(dir->d_name, ignore_list[i]) == 0) {
          ignore = 1;
          break;
        }
      }
      if (ignore) {
        continue;
      }

      if (dir->d_type == DT_DIR) {
        char dirname[PATH_MAX];
        sprintf(dirname, "%s%s/", path, dir->d_name);
        struct TreeNode *child = add_child(data, new_node(0, 0));
        float s = read_dir(child, dirname);
        child->data = s;
        child->label = strdup(dirname + path_len);
        size += s;

      } else {
        char filename[PATH_MAX];
        sprintf(filename, "%s%s", path, dir->d_name);
        FILE *f = fopen(filename, "r");
        fseek(f, 0, SEEK_END);
        long fsize = ftell(f);
        fclose(f);

        struct TreeNode *child = add_child(data, new_node(fsize, 0));
        child->label = strdup(filename + path_len);
        size += fsize;
      }
    }
    closedir(d);
  }

  data->data = size;
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
