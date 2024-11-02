#include "game.h"

static bool initialized = false;

static void game_init(void) {}

void game_tick(game_io_t* io, f64 dt) {
  if (!initialized) {
    game_init();
  }

  update_input_state(io->input_state);

  if (button_pressed(BUTTON_ESCAPE)) {
    io->exit_requested = true;
  }
}