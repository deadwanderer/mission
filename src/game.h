#ifndef MISSION_GAME_H
#define MISSION_GAME_H

#include "input.h"
#include "ui.h"

typedef struct game_io_t {
  bool has_focus;

  const input_state_t* input_state;

  bool cursor_locked;
  bool exit_requested;
} game_io_t;

void game_tick(game_io_t* io, f64 dt);

#endif  // MISSION_GAME_H