#include "input.h"

static input_kind_t input_kind;

static u64 buttons_down;
static u64 buttons_pressed;
static u64 buttons_released;

static int mouse_x, mouse_y;
static int mouse_dx, mouse_dy;

static bool game_input_suppressed;

void update_input_state(const input_state_t* state) {
  input_kind = state->input_kind;

  buttons_pressed =
      (buttons_down ^ state->button_states) & state->button_states;
  buttons_released =
      (buttons_down ^ state->button_states) & ~state->button_states;
  buttons_down = state->button_states;

  mouse_x = state->mouse_x;
  mouse_y = state->mouse_y;

  mouse_dx = state->mouse_dx;
  mouse_dy = state->mouse_dy;

  game_input_suppressed = false;
}

void suppress_game_input(bool suppress) {
  game_input_suppressed = suppress;
}

bool ui_button_pressed(u64 buttons) {
  return !!(buttons_pressed & buttons);
}

bool ui_button_released(u64 buttons) {
  return !!(buttons_released & buttons);
}

bool ui_button_down(u64 buttons) {
  return !!(buttons_down & buttons);
}

input_kind_t get_input_kind(void) {
  return input_kind;
}

bool button_pressed(u64 buttons) {
  if (game_input_suppressed) {
    return false;
  }
  return !!(buttons_pressed & buttons);
}

bool button_released(u64 buttons) {
  if (game_input_suppressed) {
    return false;
  }
  return !!(buttons_released & buttons);
}

bool button_down(u64 buttons) {
  if (game_input_suppressed) {
    return false;
  }
  return !!(buttons_down & buttons);
}

void get_mouse(int* x, int* y) {
  *x = mouse_x;
  *y = mouse_y;
}

void get_mouse_delta(int* dx, int* dy) {
  *dx = mouse_dx;
  *dy = mouse_dy;
}