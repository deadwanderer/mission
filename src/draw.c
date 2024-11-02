#include "draw.h"
#include <cdbgui/cdbgui.h>
#include <sokol_app.h>
#include <sokol_glue.h>
#include <sokol_log.h>

void draw_init(draw_state_t* state) {
  sg_setup(&(sg_desc){
      .environment = sglue_environment(),
      .logger.func = slog_func,
  });
  __cdbgui_setup(sapp_sample_count());
  state->pa =
      (sg_pass_action){.colors[0] = {.load_action = SG_LOADACTION_CLEAR,
                                     .clear_value = {0.1f, 0.3f, 0.2f}}};
}

void draw_shutdown(draw_state_t* state) {
  __cdbgui_shutdown();
  sg_shutdown();
}

void draw_begin(draw_state_t* state) {
  sg_begin_pass(
      &(sg_pass){.action = state->pa, .swapchain = sglue_swapchain()});
}

void draw_render_list(draw_state_t* state, r_list_t* list) {}

void draw_end(draw_state_t* state) {
  (void)state;
  __cdbgui_draw();
  sg_end_pass();
  sg_commit();
}

// void draw_present() {}
