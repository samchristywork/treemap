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

  add_arg('x', "width", "TODO");
  add_arg('y', "height", "TODO");
  parse_opts(argc, argv);

  struct TreeNode *data = init_data();
  srand(time(NULL));
  render_treemap_3(data, svg_renderer);
}
