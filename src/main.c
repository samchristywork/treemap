#include <render.h>
#include <stdlib.h>
#include <time.h>
#include <util.h>

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
