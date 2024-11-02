@ctype mat4 mat4
@ctype vec4 v4
@ctype vec3 v3
@ctype vec2 v2

@vs immediate_vs

uniform vs_uniforms {
  mat4 view;
  mat4 proj;
  float depth_bias;
};

in vec2 in_pos;
in vec2 in_uv;
in uint in_col;

out vec2 uv;
out vec4 col;

void main() {
  gl_Position = vec4(in_pos, depth_bias, 1.0);
  uv = in_uv;
  col = vec4(float((in_col >>0) & 0xFF) / 255.0,  float((in_col >>8) & 0xFF) / 255.0,    float((in_col >>16) & 0xFF) / 255.0,    float((in_col >>24) & 0xFF) / 255.0  );
}

@end

@fs immediate_fs

uniform texture2D tex;
uniform sampler smp;

in vec2 uv;
in vec4 col;

out vec4 frag_color;

void main() {
  frag_color = vec4(texture(sampler2D(tex, smp), uv).xyz, 1.0) * col;
}

@end

@program immediate immediate_vs immediate_fs