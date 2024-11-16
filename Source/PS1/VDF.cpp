#include "PS1/VDF.h"

#include "Util/Endian.h"

namespace PaperPup::VDF
{

Data::Data(const Types::File &vdf_file, const Types::File &dat_file)
{
	// Read headers
	auto &vdf_header = vdf_file.Get<Header>(0);
	auto &dat_header = dat_file.Get<DAT::Header>(0);

	if (dat_header.keys != vdf_header.keys)
		throw Types::RuntimeException("VDF and DAT key count mismatch");

	// Read keys
	keyframes_size = Endian::SwapLE(dat_header.keys);
	keyframes = std::make_unique<Keyframe[]>(keyframes_size);

	uintptr_t vdf_p = sizeof(Header);
	uintptr_t dat_p = sizeof(DAT::Header);

	size_t total_vertices = 0;
	size_t total_weights = 0;

	for (uint16_t i = 0; i < keyframes_size; i++)
	{
		// Read keyframe info
		auto &keyframe = keyframes[i];

		auto &vdf_key = vdf_file.Get<KeyHeader>(vdf_p);
		auto &dat_key = dat_file.Get<DAT::KeyHeader>(dat_p);

		keyframe.object_id = Endian::SwapLE(vdf_key.object_id);
		keyframe.vertex_start = Endian::SwapLE(vdf_key.vertex_start);
		keyframe.vertex_size = Endian::SwapLE(vdf_key.vertex_size);

		keyframe.frames = Endian::SwapLE(dat_key.frames);

		// Increment pointers
		total_vertices += keyframe.vertex_size;
		total_weights += keyframe.frames;

		auto vertex_inc = sizeof(Vertex) * keyframe.vertex_size;
		auto weight_inc = sizeof(int16_t) * keyframe.frames;
		
		auto vdf_inc = sizeof(KeyHeader) + vertex_inc;
		auto dat_inc = sizeof(DAT::KeyHeader) + weight_inc;

		vdf_p += vdf_inc;
		dat_p += dat_inc;
	}

	// Read data
	vertices = std::make_unique<Vertex[]>(total_vertices);
	weights = std::make_unique<int16_t[]>(total_weights);

	Vertex *vertex_p = vertices.get();
	int16_t *weight_p = weights.get();

	vdf_p = sizeof(Header);
	dat_p = sizeof(DAT::Header);

	for (uint16_t i = 0; i < keyframes_size; i++)
	{
		// Read keyframe data
		auto &keyframe = keyframes[i];

		auto vdf_span = vdf_file.GetSubspan<Vertex>(vdf_p + sizeof(KeyHeader), keyframe.vertex_size);
		auto dat_span = dat_file.GetSubspan<int16_t>(dat_p + sizeof(DAT::KeyHeader), keyframe.frames);

		std::transform(vdf_span.begin(), vdf_span.end(), vertex_p, [](const Vertex &v) { return Vertex{ Endian::SwapLE(v.x), Endian::SwapLE(v.y), Endian::SwapLE(v.z), Endian::SwapLE(v.pad) }; });
		std::transform(dat_span.begin(), dat_span.end(), weight_p, Endian::SwapLE<int16_t>);

		// Increment pointers
		vdf_p += sizeof(KeyHeader) + sizeof(Vertex) * keyframe.vertex_size;
		dat_p += sizeof(DAT::KeyHeader) + sizeof(int16_t) * keyframe.frames;

		vertex_p += keyframe.vertex_size;
		weight_p += keyframe.frames;
	}
}

}
