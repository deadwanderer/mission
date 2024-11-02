#ifndef RENDER_HELPERS_H
#define RENDER_HELPERS_H

#include "api_types.h"

void r_push_line(struct r_immediate_draw_t* draw_call,
                 v2 start,
                 v2 end,
                 v4 color);
void r_push_line_gradient(struct r_immediate_draw_t* draw_call,
                          v2 start,
                          v2 end,
                          v4 start_color,
                          v4 end_color);
// void r_push_arrow(struct r_immediate_draw_t* draw_call,
//                   v2 start,
//                   v2 end,
//                   v4 color);
// void r_push_arrow_gradient(struct r_immediate_draw_t* draw_call,
//                            v2 start,
//                            v2 end,
//                            v4 start_color,
//                            v4 end_color);
void r_push_rect2_filled(struct r_immediate_draw_t* draw_call,
                         rect2 rect,
                         v4 color);
void r_push_rect2_filled_gradient(struct r_immediate_draw_t* draw_call,
                                  rect2 rect,
                                  v4 colors[4]);
// void r_push_rect3_outline(struct r_immediate_draw_t* draw_call,
//                           rect3 bounds,
//                           v4 color);

void r_push_text(struct r_immediate_draw_t* draw_call,
                 const struct bitmap_font_t* font,
                 v2 p,
                 v4 color,
                 string_t string);

#endif  // RENDER_HELPERS_H