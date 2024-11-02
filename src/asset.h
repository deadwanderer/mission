#ifndef MISSION_ASSET_H
#define MISSION_ASSET_H

#include "api_types.h"
#include "string.h"
#include <sokol_gfx.h>

// Streaming asset API

typedef enum asset_kind_t {
  ASSET_KIND_NONE,

  ASSET_KIND_IMAGE,
  ASSET_KIND_WAVEFORM,

  ASSET_KIND_COUNT,
} asset_kind_t;

typedef enum asset_state_t {
  ASSET_STATE_NONE,

  ASSET_STATE_ON_DISK,
  ASSET_STATE_BEING_LOADED_ASYNC,
  ASSET_STATE_IN_MEMORY,

  ASSET_STATE_COUNT,
} asset_state_t;

typedef struct asset_hash_t {
  u64 value;
} asset_hash_t;

MISSION_INLINE asset_hash_t asset_hash_from_string(string_t string) {
  asset_hash_t result = {
      .value = string_hash(string),
  };
  return result;
}

typedef struct image_t {
  u32 w;
  u32 h;
  u32 pitch;
  u32 channel_count;
  void* pixels;

  sg_image gpu;
} image_t;

// typedef struct cubemap_t {
//   u32 w;
//   u32 h;
//   u32 pitch;
//   u32 channel_count;
//   u32* pixels[6];

//   resource_handle_t gpu;
// } cubemap_t;

typedef struct waveform_t {
  u32 channel_count;
  u32 frame_count;
  s16* frames;
} waveform_t;

void initialize_asset_system(void);
void preload_asset(asset_hash_t hash);

image_t missing_image;
waveform_t missing_waveform;

bool asset_exists(asset_hash_t hash, asset_kind_t kind);

image_t* get_image(asset_hash_t hash);
image_t* get_missing_image(void);
waveform_t* get_waveform(asset_hash_t hash);
waveform_t* get_missing_waveform(void);

image_t* get_image_blocking(asset_hash_t hash);
waveform_t* get_waveform_blocking(asset_hash_t hash);

MISSION_INLINE image_t* get_image_from_string(string_t string) {
  return get_image(asset_hash_from_string(string));
}

MISSION_INLINE waveform_t* get_waveform_from_string(string_t string) {
  return get_waveform(asset_hash_from_string(string));
}

// Raw asset loading

image_t load_image_from_memory(arena_t* arena,
                               string_t file_data,
                               unsigned nchannels);
image_t load_image_from_disk(arena_t* arena, string_t path, unsigned nchannels);

// bool split_image_into_cubemap_faces(const image_t* source, cubemap_t*
// cubemap);

#define WAVE_SAMPLE_RATE 44100

waveform_t load_waveform_from_memory(arena_t* arena, string_t file_data);
waveform_t load_waveform_from_disk(arena_t* arena, string_t path);

#endif