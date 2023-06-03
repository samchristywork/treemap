#include <dirent.h>
#include <node.h>
#include <render.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
        child->label = strdup(dir->d_name);
        size += fsize;
      }
    }
    closedir(d);
  }

  return size;
}

struct TreeNode *init_data(void) {
  struct TreeNode *data = new_node(0, 0);

  read_dir(data, "../notepad/");
  return data;
}

int main(int argc, char *argv[]) {

  struct TreeNode *data = init_data();
  srand(time(NULL));
  render_treemap(data, svg_renderer);
}
