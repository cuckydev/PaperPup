R"(
usampler2D u_texture_vram;

uint SampleVRAM(uvec2 pos)
{
	return texelFetch(u_texture_vram, pos, 0).r;
}
)"
