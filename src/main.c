#include <sokol_app.h>
#include <sokol_time.h>
#include <sokol_fetch.h>
#include <sokol_log.h>
#include <cdbgui/cdbgui.h>

#include "config.h"
#include "core.h"
#include "draw.h"
#include "render.h"
// #include "ui.h"
#include "input.h"
#include "game.h"

static struct {
  arena_t frame_arena;
  r_list_t render_list;
  draw_state_t draw_state;
  input_state_t input;
  bool has_focus;
} state = {0};

#define MAIN_ARENA_BUFFER_SIZE GB(1)
static arena_t main_arena;

#define FRAME_ARENA_BUFFER_SIZE MB(128)

static void init(void) {
  void* main_arena_buffer = malloc(MAIN_ARENA_BUFFER_SIZE);
  m_init_with_memory(&main_arena, main_arena_buffer, MAIN_ARENA_BUFFER_SIZE);
  void* frame_arena_buffer = malloc(FRAME_ARENA_BUFFER_SIZE);
  m_init_with_memory(&state.frame_arena, frame_arena_buffer,
                     FRAME_ARENA_BUFFER_SIZE);

  equip_render_api(draw_get_api());
  draw_init(&state.draw_state);
  state.render_list = (r_list_t){
      .command_list_size = MB(16),
      .command_list_base = m_alloc_nozero(&main_arena, MB(16), 16),
      .max_immediate_icount = 1 << 24,
      .immediate_indices = m_alloc_array_nozero(&main_arena, 1 << 24, u32),
      .max_immediate_vcount = 1 << 23,
      .immediate_vertices =
          m_alloc_array_nozero(&main_arena, 1 << 23, vertex_immediate_t),
  };
  r_set_command_list(&state.render_list);
}

static void frame(void) {
  r_reset_command_list();

  game_io_t io = {
      .has_focus = state.has_focus,
      .input_state = &state.input,
  };

  game_tick(&io, sapp_frame_duration());

  draw_begin(&state.draw_state);
  draw_render_list(&state.draw_state, &state.render_list);
  draw_end(&state.draw_state);

  m_reset(&state.frame_arena);

  if (io.exit_requested) {
    sapp_request_quit();
  }
}

static void cleanup(void) {
  draw_shutdown(&state.draw_state);
}

static void event(const sapp_event* e) {
  __cdbgui_event(e);
  switch (e->type) {
    case SAPP_EVENTTYPE_KEY_DOWN: {
      switch (e->key_code) {
        case SAPP_KEYCODE_W: {
          state.input.button_states |= BUTTON_FORWARD;
        } break;
        case SAPP_KEYCODE_S: {
          state.input.button_states |= BUTTON_BACK;
        } break;
        case SAPP_KEYCODE_A: {
          state.input.button_states |= BUTTON_LEFT;
        } break;
        case SAPP_KEYCODE_D: {
          state.input.button_states |= BUTTON_RIGHT;
        } break;
        case SAPP_KEYCODE_SPACE: {
          state.input.button_states |= BUTTON_JUMP;
        } break;
        case SAPP_KEYCODE_LEFT_CONTROL:
        case SAPP_KEYCODE_RIGHT_CONTROL: {
          state.input.button_states |= BUTTON_CROUCH;
        } break;
        case SAPP_KEYCODE_LEFT_SHIFT:
        case SAPP_KEYCODE_RIGHT_SHIFT: {
          state.input.button_states |= BUTTON_RUN;
        } break;
        case SAPP_KEYCODE_V: {
          state.input.button_states |= BUTTON_TOGGLE_NOCLIP;
        } break;
        case SAPP_KEYCODE_ESCAPE: {
          state.input.button_states |= BUTTON_ESCAPE;
          // sapp_request_quit();
        } break;
      }
    } break;
    case SAPP_EVENTTYPE_KEY_UP: {
    } break;
    case SAPP_EVENTTYPE_CHAR: {
    } break;
    case SAPP_EVENTTYPE_MOUSE_DOWN: {
      switch (e->mouse_button) {
        case SAPP_MOUSEBUTTON_LEFT: {
          state.input.button_states |= BUTTON_FIRE1;
        } break;
        case SAPP_MOUSEBUTTON_RIGHT: {
          state.input.button_states |= BUTTON_FIRE2;
        } break;
      }
    } break;
    case SAPP_EVENTTYPE_MOUSE_UP: {
    } break;
    case SAPP_EVENTTYPE_MOUSE_SCROLL: {
    } break;
    case SAPP_EVENTTYPE_MOUSE_MOVE: {
      state.input.mouse_x = (int)e->mouse_x;
      state.input.mouse_y = (int)e->mouse_y;
      state.input.mouse_dx = (int)e->mouse_dx;
      state.input.mouse_dy = (int)e->mouse_dy;
    } break;
    case SAPP_EVENTTYPE_MOUSE_ENTER: {
    } break;
    case SAPP_EVENTTYPE_MOUSE_LEAVE: {
    } break;
    case SAPP_EVENTTYPE_TOUCHES_BEGAN: {
    } break;
    case SAPP_EVENTTYPE_TOUCHES_MOVED: {
    } break;
    case SAPP_EVENTTYPE_TOUCHES_ENDED: {
    } break;
    case SAPP_EVENTTYPE_TOUCHES_CANCELLED: {
    } break;
    case SAPP_EVENTTYPE_RESIZED: {
    } break;
    case SAPP_EVENTTYPE_ICONIFIED: {
    } break;
    case SAPP_EVENTTYPE_RESTORED: {
    } break;
    case SAPP_EVENTTYPE_FOCUSED: {
      state.has_focus = true;
    } break;
    case SAPP_EVENTTYPE_UNFOCUSED: {
      state.has_focus = false;
    } break;
    case SAPP_EVENTTYPE_SUSPENDED: {
    } break;
    case SAPP_EVENTTYPE_RESUMED: {
    } break;
    case SAPP_EVENTTYPE_QUIT_REQUESTED: {
    } break;
    case SAPP_EVENTTYPE_CLIPBOARD_PASTED: {
    } break;
    case SAPP_EVENTTYPE_FILES_DROPPED: {
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
      .width = 1920,
      .height = 1080,
      .window_title = "mission",
      .sample_count = 4,
      .icon.sokol_default = true,
      .logger.func = slog_func,
  };
}