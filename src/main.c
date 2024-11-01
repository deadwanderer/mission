#include <sokol_gfx.h>
#include <sokol_app.h>
#include <sokol_time.h>
#include <sokol_fetch.h>
#include <sokol_log.h>
#include <sokol_glue.h>
#include <cdbgui/cdbgui.h>

#include "config.h"

static struct {
  sg_pass_action pass_action;
} state;

static void init(void) {
  sg_setup(&(sg_desc){
      .environment = sglue_environment(),
      .logger.func = slog_func,
  });
  __cdbgui_setup(sapp_sample_count());
  state.pass_action =
      (sg_pass_action){.colors[0] = {.load_action = SG_LOADACTION_CLEAR,
                                     .clear_value = {0.1f, 0.3f, 0.2f}}};
}

static void frame(void) {
  sg_begin_pass(
      &(sg_pass){.action = state.pass_action, .swapchain = sglue_swapchain()});
  __cdbgui_draw();
  sg_end_pass();
  sg_commit();
}

static void cleanup(void) {
  __cdbgui_shutdown();
  sg_shutdown();
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