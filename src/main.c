#include <command_line.h>
#include <render.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <util.h>

struct TreeNode *init_data(void) {
  struct TreeNode *data = new_node(0, 0);

  read_dir(data, "../notepad/");

  return data;
}

int main(int argc, char *argv[]) {

  char *output_file = "output.svg";

  add_arg('x', "width", "The width of the viewport (default 1.0).");
  add_arg('y', "height", "The height of the viewport (default 0.6).");
  add_arg('o', "output", "The output file (default output.svg).");
  add_arg('s', "seed", "The seed for the random number generator (default time(NULL)).");

  parse_opts(argc, argv);

  float width = 0.;
  float height = 0.;

  if (get_is_set('x')) {
    width = atof(get_value('x'));
  }

  if (get_is_set('y')) {
    height = atof(get_value('y'));
  }

  if (width==0) {
    width = 1.;
  }

  if (height==0) {
    height = 0.6;
  }

  if (get_is_set('o')) {
    output_file = get_value('o');

    if (output_file == NULL) {
      fprintf(stderr, "Error: no output file specified.\n");
      exit(1);
    }
  }

  struct Rect viewport = {0, 0, width, height};

  struct TreeNode *data = init_data();
  srand(time(NULL));

  char *svg = malloc(1);
  svg[0] = '\0';

  render_treemap(&svg, data, svg_renderer, viewport);

  FILE *fp = fopen(output_file, "w");
  fprintf(fp, "%s", svg);
  fclose(fp);

  free(svg);
}
