#include <command_line.h>
#include <render.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <util.h>

int main(int argc, char *argv[]) {

  char *output_file = NULL;
  int seed = time(NULL);

  add_arg('d', "data-source", "The data source (default read_dir).");
  add_arg('x', "width", "The width of the viewport (default 1.0).");
  add_arg('y', "height", "The height of the viewport (default 0.6).");
  add_arg('o', "output", "The output file (default output.svg).");
  add_arg('s', "seed",
          "The seed for the random number generator (default time(NULL)).");

  parse_opts(argc, argv);

  float width = 0.;
  float height = 0.;

  if (get_is_set('x')) {
    width = atof(get_value('x'));
  }

  if (get_is_set('y')) {
    height = atof(get_value('y'));
  }

  if (width == 0) {
    width = 1.;
  }

  if (height == 0) {
    height = 0.6;
  }

  if (get_is_set('o')) {
    output_file = get_value('o');

    if (output_file == NULL) {
      fprintf(stderr, "Error: no output file specified.\n");
      exit(1);
    }
  }

  if (get_is_set('s')) {
    seed = atoi(get_value('s'));
  }

  struct Rect viewport = {0, 0, width, height};

  struct TreeNode *data = NULL;

  if (get_is_set('d')) {
    char *data_source = get_value('d');

    if (strcmp(data_source, "read_dir") == 0) {
      data = new_node(0, 0);
      read_dir(data, "build/");
    } else {
      fprintf(stderr, "Error: unknown data source.\n");
      exit(1);
    }
  } else {
    data = new_node(0, 0);
    read_dir(data, "build/");
  }

  srand(seed);

  char *svg = malloc(1);
  svg[0] = '\0';

  render_treemap(&svg, data, svg_renderer, viewport);

  if (output_file == NULL) {
    printf("%s", svg);
  } else {
    FILE *fp = fopen(output_file, "w");
    fprintf(fp, "%s", svg);
    fclose(fp);
  }

  free(svg);
}
