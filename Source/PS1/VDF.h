#pragma once

#include <cstdint>
#include <memory>
#include <span>

#include "Types/File.h"

namespace PaperPup::VDF
{

namespace DAT
{

// DAT file structures
struct Header
{
	uint16_t keys;
};
static_assert(sizeof(Header) == 2);
static_assert(alignof(Header) <= 2);

struct KeyHeader
{
	uint16_t frames;
};
static_assert(sizeof(KeyHeader) == 2);
static_assert(alignof(KeyHeader) <= 2);

}

// VDF file structures
struct Header
{
	uint32_t keys;
};
static_assert(sizeof(Header) == 4);
static_assert(alignof(Header) <= 4);

struct KeyHeader
{
	uint32_t object_id;
	uint32_t vertex_start;
	uint32_t vertex_size;
};
static_assert(sizeof(KeyHeader) == 12);
static_assert(alignof(KeyHeader) <= 4);

struct Vertex
{
	int16_t x;
	int16_t y;
	int16_t z;
	int16_t pad;
};
static_assert(sizeof(Vertex) == 8);
static_assert(alignof(Vertex) <= 2);

// Data class
struct Data
{
	std::unique_ptr<Vertex[]> vertices;
	std::unique_ptr<int16_t[]> weights;

	struct Keyframe
	{
		Vertex *vertex_p;
		int16_t *weight_p;

		uint32_t object_id;
		uint32_t vertex_start;
		uint32_t vertex_size;

		uint16_t frames;
	};

	std::unique_ptr<Keyframe[]> keyframes;
	uint16_t keyframes_size;

	Data(const Types::File &vdf_file, const Types::File &dat_file);
};

}
