#include "Header.h"
R"(

out vec4 o_color;

uniform usampler2D u_vram;

in vec2 v_uv;
flat in uint v_clut;

vec4 RGBA5551ToRGBA8(uint v)
{
  uint r = (v & 31u);
  uint g = ((v >> 5) & 31u);
  uint b = ((v >> 10) & 31u);
  uint a = ((v >> 15) & 1u);

  return vec4(float(r) / 31.0, float(g) / 31.0, float(b) / 31.0, float(a));
}

void main()
{
	uvec2 tex_coord = uvec2(v_uv);
	ivec2 texel_coord = ivec2(v_uv.x / 4u, v_uv.y);
	uint texel = texelFetch(u_vram, texel_coord, 0).r;
	uint nibble = (texel >> ((tex_coord.x & 3u) << 2u)) & 0xFu;

	ivec2 clut_coord = ivec2(((v_clut & 0x3Fu) << 4u) + nibble, v_clut >> 6u);
	uint clut_texel = texelFetch(u_vram, clut_coord, 0).r;

	if (clut_texel == 0u)
		discard;
	o_color = RGBA5551ToRGBA8(clut_texel);
}
)"
