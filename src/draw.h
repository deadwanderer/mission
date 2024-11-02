#ifndef MISSION_DRAW_H
#define MISSION_DRAW_H

#include "render.h"

#include <sokol_gfx.h>

typedef struct draw_state_t {
  sg_pass_action pa;
} draw_state_t;

void draw_init(draw_state_t* state);
void draw_shutdown(draw_state_t* state);
void draw_begin(draw_state_t* state);
void draw_render_list(draw_state_t* state, r_list_t* list);
void draw_end(draw_state_t* state);
// void draw_present();

#endif  // MISSION_DRAW_H
