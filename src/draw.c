#include "draw.h"
#include <cdbgui/cdbgui.h>
#include <sokol_app.h>
#include <sokol_glue.h>
#include <sokol_log.h>

// #include "shaders.glsl.h"

sg_pixel_format _render_to_sg_format(pixel_format_t fmt) {
  switch (fmt) {
    case PIXEL_FORMAT_R8:
      return SG_PIXELFORMAT_R8;
    case PIXEL_FORMAT_RG8:
      return SG_PIXELFORMAT_RG8;
    case PIXEL_FORMAT_RGBA8:
      return SG_PIXELFORMAT_RGBA8;
    case PIXEL_FORMAT_SRGB8_A8:
      return SG_PIXELFORMAT_SRGB8A8;
    case PIXEL_FORMAT_R11G11B10F:
      return SG_PIXELFORMAT_RG11B10F;
    // case PIXEL_FORMAT_R32G32B32F:
    //   return SG_PIXELFORMAT_R32G32B32F;
    case PIXEL_FORMAT_R32G32B32A32F:
      return SG_PIXELFORMAT_RGBA32F;
      INVALID_DEFAULT_CASE;
  }
  return SG_PIXELFORMAT_NONE;
}

pixel_format_t _sg_to_render_format(sg_pixel_format fmt) {
  switch (fmt) {
    case SG_PIXELFORMAT_R8:
      return PIXEL_FORMAT_R8;
    case SG_PIXELFORMAT_RG8:
      return PIXEL_FORMAT_RG8;
    case SG_PIXELFORMAT_RGBA8:
      return PIXEL_FORMAT_RGBA8;
    case SG_PIXELFORMAT_SRGB8A8:
      return PIXEL_FORMAT_SRGB8_A8;
    case SG_PIXELFORMAT_RG11B10F:
      return PIXEL_FORMAT_R11G11B10F;
    case SG_PIXELFORMAT_RGBA32F:
      return PIXEL_FORMAT_R32G32B32A32F;
      INVALID_DEFAULT_CASE;
  }
  return PIXEL_FORMAT_RGBA8;
}

static struct render_state_t {
  sg_buffer ibuffer;
  sg_buffer vbuffer;
  sg_pipeline pip;
  sg_bindings bind;
} render_state;

void get_resolution(int* w, int* h) {
  *w = sapp_width();
  *h = sapp_height();
}

void describe_texture(sg_image handle, texture_desc_t* desc) {
  sg_image_desc img_desc = sg_query_image_desc(handle);
  desc->w = img_desc.width;
  desc->h = img_desc.height;
  desc->type =
      img_desc.type == SG_IMAGETYPE_3D ? TEXTURE_TYPE_3D : TEXTURE_TYPE_2D;
  desc->format = _sg_to_render_format(img_desc.pixel_format);
  desc->d = img_desc.num_slices;
}

sg_image reserve_texture(void) {
  return sg_alloc_image();
}

void populate_texture(sg_image handle, const upload_texture_t* params) {
  if (!params->data.pixels) {
    return;
  }
  sg_init_image(
      handle,
      &(sg_image_desc){
          .type = params->desc.type == TEXTURE_TYPE_3D ? SG_IMAGETYPE_3D
                                                       : TEXTURE_TYPE_2D,
          .width = params->desc.w,
          .height = params->desc.h,
          .num_slices =
              params->desc.type == TEXTURE_TYPE_3D ? params->desc.d : 1,
          .pixel_format = _render_to_sg_format(params->desc.format),
          .data.subimage[0][0] = {params->data.pixels,
                                  params->data.pitch * params->desc.h},
      });
}

sg_image upload_texture(const upload_texture_t* params) {
  sg_image img = reserve_texture();
  populate_texture(img, params);
  return img;
}

void destroy_texture(sg_image texture) {
  sg_destroy_image(texture);
}

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

render_api_i* draw_get_api(void) {
  static render_api_i api = {
      .get_resolution = get_resolution,
      .describe_texture = describe_texture,
      .reserve_texture = reserve_texture,
      .populate_texture = populate_texture,
      .upload_texture = upload_texture,
      .destroy_texture = destroy_texture,
  };
  return &api;
}

void draw_shutdown(draw_state_t* state) {
  __cdbgui_shutdown();
  sg_shutdown();
}

void draw_begin(draw_state_t* state) {
  sg_begin_pass(
      &(sg_pass){.action = state->pa, .swapchain = sglue_swapchain()});
}

static void ensure_swapchain_size() {
  // TODO: does anything need done here? D3D11 at least seems to auto-resize the
  // default framebuffer
}

// typedef struct draw_model_t {
// } draw_model_t;

void draw_render_list(draw_state_t* state, r_list_t* list) {
  ensure_swapchain_size();
  for (char* at = list->command_list_base; at < list->command_list_at;) {
    r_command_base_t* base = (r_command_base_t*)at;
    r_view_t* view = &list->views[base->view];

    switch (base->kind) {
      case R_COMMAND_IMMEDIATE: {
        r_command_immediate_t* command = (r_command_immediate_t*)base;
        at = align_address(at + sizeof(*command), RENDER_COMMAND_ALIGN);

        r_immediate_draw_t* draw_call = &command->draw_call;

        // d3d_cbuffer_t cbuffer = {
        //     .view_matrix = view->camera,
        //     .proj_matrix = view->projection,
        //     .model_matrix = mat4_identity,
        //     .depth_bias = draw_call->params.depth_bias,
        // };

        // update_buffer(d3d.ubuffer, &cbuffer, sizeof(cbuffer));

        sg_primitive_type topology = SG_PRIMITIVETYPE_TRIANGLES;
        switch (draw_call->params.topology) {
          case R_PRIMITIVE_TOPOLOGY_TRIANGLELIST: {
            topology = SG_PRIMITIVETYPE_TRIANGLES;
          } break;
          case R_PRIMITIVE_TOPOLOGY_LINELIST: {
            topology = SG_PRIMITIVETYPE_LINES;
          } break;
          case R_PRIMITIVE_TOPOLOGY_LINESTRIP: {
            topology = SG_PRIMITIVETYPE_LINE_STRIP;
          } break;
          case R_PRIMITIVE_TOPOLOGY_POINTLIST: {
            topology = SG_PRIMITIVETYPE_POINTS;
          } break;
            INVALID_DEFAULT_CASE;
        }

        // draw_model_t immediate_model = {
        //     .vertex_format = VERTEX_FORMAT_IMMEDIATE,
        //     .primitive_topology = topology,
        //     .vcount = draw_call->vcount,
        //     .vbuffer = d3d.immediate_vbuffer,
        //     .icount = draw_call->icount,
        //     .ibuffer = d3d.immediate_ibuffer,
        // };

        // d3d_texture_t* texture =
        //     d3d_get_texture_or(draw_call->params.texture, d3d.white_texture);

        // rect2 clip_rect =
        //     rect2_intersect(draw_call->params.clip_rect, view->clip_rect);

        // render_model(&(render_pass_t){
        //     .render_target = current_render_target->color_rtv,
        //     .depth_stencil = current_render_target->depth_dsv,
        //     .model = &immediate_model,
        //     .vs = d3d.immediate_vs,
        //     .ps = d3d.immediate_ps,

        //     .blend_mode = draw_call->params.blend_mode,

        //     .ioffset = draw_call->ioffset,
        //     .voffset = draw_call->voffset,

        //     .cbuffer_count = 1,
        //     .cbuffers = (ID3D11Buffer*[]){d3d.ubuffer},
        //     .srv_count = 1,
        //     .srvs = (ID3D11ShaderResourceView*[]){texture->srv},
        //     .depth = draw_call->params.depth_test ? D3D_DEPTH_TEST_GREATER
        //                                           : D3D_DEPTH_TEST_NONE,
        //     .cull = D3D_CULL_NONE,
        //     .viewport = viewport,
        //     .scissor = true,
        //     .scissor_rect = {
        //         .left = (LONG)clip_rect.min.x,
        //         .right = (LONG)clip_rect.max.x,
        //         .bottom = height - (LONG)clip_rect.min.y,
        //         .top = height - (LONG)clip_rect.max.y,
        //     }});
      } break;
      case R_COMMAND_END_SCENE_PASS: {
        at = align_address(at + sizeof(*base), RENDER_COMMAND_ALIGN);

      } break;
        INVALID_DEFAULT_CASE;
    }
  }
}

void draw_end(draw_state_t* state) {
  (void)state;
  __cdbgui_draw();
  sg_end_pass();
  sg_commit();
}

// void draw_present() {}
