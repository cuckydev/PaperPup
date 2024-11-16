#include "Header.h"
R"(
layout(location = 0) in ivec3 i_pos;
layout(location = 1) in ivec3 i_normal;
layout(location = 2) in uvec2 i_uv;
layout(location = 3) in uint i_clut;
layout(location = 4) in uvec4 i_rgbx;

uniform mat4 u_projection;

out vec2 v_uv;
flat out uint v_clut;

void main()
{
	gl_Position = u_projection * vec4(vec3(i_pos), 1.0);
	v_uv = vec2(float(i_uv.x), float(i_uv.y) / 1.0);
	v_clut = i_clut;
}
)"
