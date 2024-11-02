#include <sokol_app.h>
#include <sokol_time.h>
#include <sokol_fetch.h>
#include <sokol_log.h>
#include <cdbgui/cdbgui.h>

#include "config.h"
#include "core.h"
#include "draw.h"
#include "render.h"
#include "ui.h"

static struct {
  arena_t frame_arena;
  r_list_t render_list;
  draw_state_t draw_state;
} state;

static void init(void) {
  draw_init(&state.draw_state);
}

static void frame(void) {
  draw_begin(&state.draw_state);
  draw_render_list(&state.draw_state, &state.render_list);
  draw_end(&state.draw_state);
}

static void cleanup(void) {
  draw_shutdown(&state.draw_state);
}

static void event(const sapp_event* e) {
  __cdbgui_event(e);
  switch (e->type) {
    case SAPP_EVENTTYPE_KEY_DOWN: {
      switch (e->key_code) {
        case SAPP_KEYCODE_ESCAPE: {
          sapp_request_quit();
        } break;
      }
    } break;
  }
}

sapp_desc sokol_main(int argc, char* argv[]) {
  (void)argc;
  (void)argv;

  return (sapp_desc){
      .init_cb = init,
      .frame_cb = frame,
      .cleanup_cb = cleanup,
      .event_cb = event,
      .width = 1280,
      .height = 720,
      .window_title = "mission",
      .sample_count = 4,
      .icon.sokol_default = true,
  };
}