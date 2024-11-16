#pragma once

#include <cstdint>
#include <bit>
#include <vector>

#include <memory>

#include "Backend/Render.h"

#include "Types/File.h"
#include "Types/UniqueGLInstance.h"

namespace PaperPup::TMD
{

// TMD file structures
struct Header
{
	enum Flags
	{
		Flag_FixedPointer = (1 << 0), // Unsupported, shouldn't appear in any files though
	};

	uint32_t id; // = 0x41
	uint32_t flags;
	uint32_t object_size;
};
static_assert(sizeof(Header) == 12);
static_assert(alignof(Header) <= 4);

struct ObjectHeader
{
	uint32_t vertex_start;
	uint32_t vertex_size;
	uint32_t normal_start;
	uint32_t normal_size;
	uint32_t primitive_start;
	uint32_t primitive_size;
	int32_t scale; // 2^scale, not used for display?
};
static_assert(sizeof(ObjectHeader) == 28);
static_assert(alignof(ObjectHeader) <= 4);

struct Vector
{
	int16_t x, y, z, pad;
};
static_assert(sizeof(Vector) == 8);
static_assert(alignof(Vector) <= 2);

struct Primitive
{
protected:
	uint32_t x;
};
static_assert(sizeof(Primitive) == 4);
static_assert(alignof(Primitive) <= 4);

struct PrimitiveHeader : public Primitive
{
	enum Mode
	{
		Mode_Kind_Poly = (1 << 5),
		Mode_Kind_Line = (2 << 5),
		Mode_Kind_Sprite = (3 << 5),
		Mode_Kind_Mask = (7 << 5),

		Mode_NoLight = (1 << 0), // Poly
		Mode_Transparency = (1 << 1), // Poly, Line, Sprite
		Mode_Textured = (1 << 2), // Poly
		Mode_Quad = (1 << 3), // Poly
		Mode_Gouraud = (1 << 4), // Poly, Line

		Mode_Size_Any = (0 << 3), // Sprite
		Mode_Size_1x1 = (1 << 3), // Sprite
		Mode_Size_8x8 = (2 << 3), // Sprite
		Mode_Size_16x16 = (3 << 3), // Sprite
		Mode_Size_Mask = (3 << 3), // Sprite
	};

	enum Flags
	{
		Flag_NoLight = (1 << 0),
		Flag_DoubleSided = (1 << 1),
		Flag_Gouraud = (1 << 2),
	};

	uint8_t OutWords() const
	{
		return (x >> 0) & 0xFF;
	}

	uint8_t InWords() const
	{
		return (x >> 8) & 0xFF;
	}

	uint8_t Flags() const
	{
		return (x >> 16) & 0xFF;
	}

	uint8_t Mode() const
	{
		return (x >> 24) & 0xFF;
	}
};

struct PrimitivePair : public Primitive
{
	// More significant bits
	uint16_t Left() const
	{
		return (x >> 16) & 0xFFFF;
	}
	// Less significant bits
	uint16_t Right() const
	{
		return (x >> 0) & 0xFFFF;
	}
};

struct PrimitiveRGB : public Primitive
{
	uint8_t R() const
	{
		return (x >> 0) & 0xFF;
	}
	uint8_t G() const
	{
		return (x >> 8) & 0xFF;
	}
	uint8_t B() const
	{
		return (x >> 16) & 0xFF;
	}

	// Most-significant byte (multi-purpose)
	uint8_t MSB() const
	{
		return (x >> 24) & 0xFF;
	}

	uint8_t Type() const { return MSB(); }
};

struct ClutPage
{
public:
	uint16_t X() const
	{
		return x & 0x3F;
	}
	uint16_t Y() const
	{
		return (x >> 6) & 0x3FF;
	}

private:
	uint16_t x;
};

enum SemiMode
{
	// bg * 0.5 + fg * 0.5
	SemiMode_Blend,
	// bg + fg
	SemiMode_Add,
	// bg - fg
	SemiMode_Sub,
	// bg * 1.0 + fg * 0.25
	SemiMode_AddQuarter,
};

enum BitDepth
{
	BitDepth_4Bit,
	BitDepth_8Bit,
	BitDepth_15Bit,
};

struct TexturePage
{
public:
	uint16_t X() const
	{
		return x & 0xF;
	}
	uint16_t Y() const
	{
		return ((x >> 10) & 0x2) | ((x >> 4) & 0x1);
	}
	SemiMode Semi() const
	{
		return static_cast<SemiMode>((x >> 5) & 0x3);
	}
	BitDepth Depth() const
	{
		return static_cast<BitDepth>((x >> 7) & 0x3);
	}

private:
	uint16_t x;
};

struct PrimitiveUV : public Primitive
{
	uint8_t U() const
	{
		return (x >> 0) & 0xFF;
	}
	uint8_t V() const
	{
		return (x >> 8) & 0xFF;
	}

	// Most-significant halfword (multi-purpose)
	uint16_t MSB() const
	{
		return (x >> 16) & 0xFFFF;
	}

	ClutPage CLUT() const { return std::bit_cast<ClutPage>(MSB()); }
	TexturePage TPage() const { return std::bit_cast<TexturePage>(MSB()); }
};

// Data class
struct Data
{
	std::unique_ptr<Vector[]> vectors;
	std::unique_ptr<uint32_t[]> primitive_blob;

	struct Object 
	{
		Vector *vertex_p;
		Vector *normal_p;
		uint32_t *primitive_p;

		uint32_t vertex_size;
		uint32_t normal_size;
		uint32_t primitive_size, primitive_words;
	};
	std::unique_ptr<Object[]> objects;
	uint32_t objects_size;

	Data(const Types::File &tmd_file);
};

// TMD model
class Model
{
private:
	std::shared_ptr<Data> tmd_data;

	// TMD model object
	class Object
	{
		struct Position
		{
			int16_t x, y, z;

			bool operator==(const Position &other) const = default;
		};
		struct Attribute
		{
			int16_t nx, ny, nz;
			uint16_t u, v;
			uint16_t clut;
			uint8_t r, g, b, x;

			bool operator==(const Attribute &other) const = default;
		};
		struct Vertex
		{
			Position position;
			Attribute attribute;

			bool operator==(const Vertex &other) const = default;
		};

		Types::UniqueGLInstance<GLuint, decltype(glDeleteVertexArrays), &glDeleteVertexArrays, false> vao_id;
		Types::UniqueGLInstance<GLuint, decltype(glDeleteBuffers), &glDeleteBuffers, false> vbo_id;
		Types::UniqueGLInstance<GLuint, decltype(glDeleteBuffers), &glDeleteBuffers, false> ebo_id;

		std::vector<uint16_t> vertex_indices;
		size_t vertex_count = 0;

	public:
		Object();

		void InitMesh(const Data::Object &object);

		void Draw() const;
	};

	std::unique_ptr<Object[]> objects;

public:
	Model(const std::shared_ptr<Data> &_tmd_data);

	void Draw() const;
};

}
