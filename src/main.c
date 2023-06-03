#include <dirent.h>
#include <node.h>
#include <render.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

  if (r.w > .05 && r.h > .05) {
    printf("<text fill=\"%s\" font-size=\".03\" x=\"%f\" y=\"%f\" "
           "text-anchor=\"middle\" "
           "alignment-baseline=\"middle\">%s</text>\n",
           color_fg, r.x + r.w / 2, r.y + r.h / 2 + .03 / 2, label);
  }

  printf("</g>\n");

  free(color_fg);
  free(color_bg);
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
      fprintf(stderr, "%s\n", dir->d_name);
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
