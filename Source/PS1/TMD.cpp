#include "PS1/TMD.h"

#include "Util/Endian.h"
#include <iostream>

#include <vector>

namespace PaperPup::TMD
{

Data::Data(const Types::File &tmd_file)
{
	auto &header = tmd_file.Get<Header>(0);

	if (Endian::SwapLE(header.id) != 0x41)
		throw Types::RuntimeException("Invalid TMD file");

	const auto flags = Endian::SwapLE(header.flags);
	if ((flags & Header::Flag_FixedPointer) != 0)
		throw Types::RuntimeException("Fixed pointer TMDs are unsupported");

	// Read objects
	objects_size = Endian::SwapLE(header.object_size);
	objects = std::make_unique<Object[]>(objects_size);

	auto tmd_p = sizeof(Header);

	size_t total_vertices = 0;
	size_t total_normals = 0;
	size_t total_primitives = 0;

	for (size_t i = 0; i < objects_size; i++)
	{
		auto &object = objects[i];

		// Read object header
		auto &object_header = tmd_file.Get<ObjectHeader>(tmd_p);

		object.vertex_size = Endian::SwapLE(object_header.vertex_size);
		object.normal_size = Endian::SwapLE(object_header.normal_size);
		object.primitive_size = Endian::SwapLE(object_header.primitive_size);

		total_vertices += object.vertex_size;
		total_normals += object.normal_size;

		// Count primitive words
		uint32_t primitive_words = 0;

		auto primitive_p = tmd_p + object_header.primitive_start;

		for (size_t p = 0; p < object.primitive_size; p++)
		{
			auto primitive_header = Endian::SwapLE(tmd_file.Get<PrimitiveHeader>(primitive_p));

			primitive_words += 1 + primitive_header.InWords();

			primitive_p += sizeof(PrimitiveHeader) + sizeof(uint32_t) * primitive_header.InWords();
		}

		object.primitive_words = primitive_words;
		total_primitives += primitive_words;

		tmd_p += sizeof(ObjectHeader);
	}

	// Read data
	vectors = std::make_unique<Vector[]>(total_vertices + total_normals);
	primitive_blob = std::make_unique<uint32_t[]>(total_primitives);

	Vector *vector_p = vectors.get();
	uint32_t *primitive_p = primitive_blob.get();

	tmd_p = sizeof(Header);

	for (size_t i = 0; i < objects_size; i++)
	{
		auto &object = objects[i];

		auto &object_header = tmd_file.Get<ObjectHeader>(tmd_p);

		const auto vertex_span = tmd_file.GetSubspan<Vector>(tmd_p + Endian::SwapLE(object_header.vertex_start), object.vertex_size);
		const auto normal_span = tmd_file.GetSubspan<Vector>(tmd_p + Endian::SwapLE(object_header.normal_start), object.normal_size);
		const auto primitive_span = tmd_file.GetSubspan<uint32_t>(tmd_p + Endian::SwapLE(object_header.primitive_start), object.primitive_words);

		std::transform(vertex_span.begin(), vertex_span.end(), vector_p, [](const Vector &v) { return Vector{ Endian::SwapLE(v.x), Endian::SwapLE(v.y), Endian::SwapLE(v.z), Endian::SwapLE(v.pad) }; });
		std::transform(normal_span.begin(), normal_span.end(), vector_p + object.vertex_size, [](const Vector &v) { return Vector{ Endian::SwapLE(v.x), Endian::SwapLE(v.y), Endian::SwapLE(v.z), Endian::SwapLE(v.pad) }; });
		std::transform(primitive_span.begin(), primitive_span.end(), primitive_p, Endian::SwapLE<uint32_t>);

		object.vertex_p = vector_p;
		object.normal_p = vector_p + object.vertex_size;
		object.primitive_p = primitive_p;

		vector_p += object.vertex_size + object.normal_size;
		primitive_p += object.primitive_words;
	}
}

// TMD model object
Model::Object::Object()
{
	glGenVertexArrays(1, vao_id.GetAddressOf());
	glGenBuffers(1, vbo_id.GetAddressOf());
	glGenBuffers(1, ebo_id.GetAddressOf());
}

void Model::Object::InitMesh(const Data::Object &object)
{
	std::vector<Vertex> vertices;
	std::vector<uint16_t> indices;

	auto add_vertex = [&vertices](const Vertex &vertex) -> uint16_t
	{
		for (size_t i = 0; i < vertices.size(); i++)
		{
			if (vertices[i] == vertex)
				return static_cast<uint16_t>(i);
		}

		size_t i = vertices.size();
		vertices.push_back(vertex);
		return static_cast<uint16_t>(i);
	};

	const auto *vertex_p = object.vertex_p;
	const auto *primitive_p = object.primitive_p;
	for (size_t p = 0; p < object.primitive_size; p++)
	{
		const auto &primitive_header = *reinterpret_cast<const PrimitiveHeader *>(primitive_p);

		// const auto primitive_flags = primitive_header.Flags();
		const auto primitive_mode = primitive_header.Mode();

		const auto primitive_kind = primitive_mode & PrimitiveHeader::Mode_Kind_Mask;
		if (primitive_kind == PrimitiveHeader::Mode_Kind_Poly)
		{
			if (primitive_mode & PrimitiveHeader::Mode_Quad)
			{
				if (primitive_mode & PrimitiveHeader::Mode_NoLight)
				{
					if (primitive_mode & PrimitiveHeader::Mode_Textured)
					{
						if (primitive_mode & PrimitiveHeader::Mode_Gouraud)
						{

						}
						else
						{
							const auto &uv0 = *reinterpret_cast<const PrimitiveUV *>(&primitive_p[1]);
							const auto &uv1 = *reinterpret_cast<const PrimitiveUV *>(&primitive_p[2]);
							const auto &uv2 = *reinterpret_cast<const PrimitiveUV *>(&primitive_p[3]);
							const auto &uv3 = *reinterpret_cast<const PrimitiveUV *>(&primitive_p[4]);
							const auto &rgb = *reinterpret_cast<const PrimitiveRGB *>(&primitive_p[5]);
							const auto &v10 = *reinterpret_cast<const PrimitivePair *>(&primitive_p[6]);
							const auto &v32 = *reinterpret_cast<const PrimitivePair *>(&primitive_p[7]);

							const auto i0 = v10.Right();
							const auto i1 = v10.Left();
							const auto i2 = v32.Right();
							const auto i3 = v32.Left();

							const auto &v0 = vertex_p[i0];
							const auto &v1 = vertex_p[i1];
							const auto &v2 = vertex_p[i2];
							const auto &v3 = vertex_p[i3];

							const auto clut = uv0.CLUT();
							const auto tpage = uv1.TPage();

							auto tpu = static_cast<uint16_t>(64 * tpage.X());
							switch (tpage.Depth())
							{
								case BitDepth_4Bit:
									tpu *= 4;
									break;
								case BitDepth_8Bit:
									tpu *= 2;
									break;
								case BitDepth_15Bit:
									break;
							}

							const auto tpv = static_cast<uint16_t>(256 * tpage.Y());

							Vertex vertex0 = { { v0.x, v0.y, v0.z }, { 0, 0, 0, static_cast<uint16_t>(tpu + uv0.U()), static_cast<uint16_t>(tpv + uv0.V()), std::bit_cast<uint16_t>(clut), rgb.R(), rgb.G(), rgb.B(), 0 } };
							Vertex vertex1 = { { v1.x, v1.y, v1.z }, { 0, 0, 0, static_cast<uint16_t>(tpu + uv1.U()), static_cast<uint16_t>(tpv + uv1.V()), std::bit_cast<uint16_t>(clut), rgb.R(), rgb.G(), rgb.B(), 0 } };
							Vertex vertex2 = { { v2.x, v2.y, v2.z }, { 0, 0, 0, static_cast<uint16_t>(tpu + uv2.U()), static_cast<uint16_t>(tpv + uv2.V()), std::bit_cast<uint16_t>(clut), rgb.R(), rgb.G(), rgb.B(), 0 } };
							Vertex vertex3 = { { v3.x, v3.y, v3.z }, { 0, 0, 0, static_cast<uint16_t>(tpu + uv3.U()), static_cast<uint16_t>(tpv + uv3.V()), std::bit_cast<uint16_t>(clut), rgb.R(), rgb.G(), rgb.B(), 0 } };

							const auto vi0 = add_vertex(vertex0);
							const auto vi1 = add_vertex(vertex1);
							const auto vi2 = add_vertex(vertex2);
							const auto vi3 = add_vertex(vertex3);

							vertex_indices.push_back(i2);
							indices.push_back(vi2);

							vertex_indices.push_back(i1);
							indices.push_back(vi1);

							vertex_indices.push_back(i0);
							indices.push_back(vi0);

							vertex_indices.push_back(i1);
							indices.push_back(vi1);

							vertex_indices.push_back(i2);
							indices.push_back(vi2);

							vertex_indices.push_back(i3);
							indices.push_back(vi3);
						}
					}
				}
			}
			else
			{
				if (primitive_mode & PrimitiveHeader::Mode_NoLight)
				{
					if (primitive_mode & PrimitiveHeader::Mode_Textured)
					{
						if (primitive_mode & PrimitiveHeader::Mode_Gouraud)
						{
							
						}
						else
						{
							const auto &uv0 = *reinterpret_cast<const PrimitiveUV *>(&primitive_p[1]);
							const auto &uv1 = *reinterpret_cast<const PrimitiveUV *>(&primitive_p[2]);
							const auto &uv2 = *reinterpret_cast<const PrimitiveUV *>(&primitive_p[3]);
							const auto &rgb = *reinterpret_cast<const PrimitiveRGB *>(&primitive_p[4]);
							const auto &v10 = *reinterpret_cast<const PrimitivePair *>(&primitive_p[5]);
							const auto &v_2 = *reinterpret_cast<const PrimitivePair *>(&primitive_p[6]);

							const auto i0 = v10.Right();
							const auto i1 = v10.Left();
							const auto i2 = v_2.Right();

							const auto &v0 = vertex_p[i0];
							const auto &v1 = vertex_p[i1];
							const auto &v2 = vertex_p[i2];

							const auto clut = uv0.CLUT();
							const auto tpage = uv1.TPage();

							auto tpu = static_cast<uint16_t>(64 * tpage.X());
							switch (tpage.Depth())
							{
								case BitDepth_4Bit:
									tpu *= 4;
									break;
								case BitDepth_8Bit:
									tpu *= 2;
									break;
								case BitDepth_15Bit:
									break;
							}

							const auto tpv = static_cast<uint16_t>(256 * tpage.Y());

							Vertex vertex0 = { { v0.x, v0.y, v0.z }, { 0, 0, 0, static_cast<uint16_t>(tpu + uv0.U()), static_cast<uint16_t>(tpv + uv0.V()), std::bit_cast<uint16_t>(clut), rgb.R(), rgb.G(), rgb.B(), 0 } };
							Vertex vertex1 = { { v1.x, v1.y, v1.z }, { 0, 0, 0, static_cast<uint16_t>(tpu + uv1.U()), static_cast<uint16_t>(tpv + uv1.V()), std::bit_cast<uint16_t>(clut), rgb.R(), rgb.G(), rgb.B(), 0 } };
							Vertex vertex2 = { { v2.x, v2.y, v2.z }, { 0, 0, 0, static_cast<uint16_t>(tpu + uv2.U()), static_cast<uint16_t>(tpv + uv2.V()), std::bit_cast<uint16_t>(clut), rgb.R(), rgb.G(), rgb.B(), 0 } };

							vertex_indices.push_back(i2);
							indices.push_back(add_vertex(vertex2));

							vertex_indices.push_back(i1);
							indices.push_back(add_vertex(vertex1));

							vertex_indices.push_back(i0);
							indices.push_back(add_vertex(vertex0));
						}
					}
					else
					{

					}
				}
			}
		}

		primitive_p += 1 + primitive_header.InWords();
	}

	glBindVertexArray(vao_id.Get());

	glBindBuffer(GL_ARRAY_BUFFER, vbo_id.Get());
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id.Get());
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof(uint16_t)), indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribIPointer(0, 3, GL_SHORT, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, position)));

	glEnableVertexAttribArray(1);
	glVertexAttribIPointer(1, 3, GL_SHORT, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, attribute.nx)));

	glEnableVertexAttribArray(2);
	glVertexAttribIPointer(2, 2, GL_UNSIGNED_SHORT, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, attribute.u)));

	glEnableVertexAttribArray(3);
	glVertexAttribIPointer(3, 1, GL_UNSIGNED_SHORT, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, attribute.clut)));

	glEnableVertexAttribArray(4);
	glVertexAttribIPointer(4, 4, GL_UNSIGNED_BYTE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, attribute.r)));

	vertex_count = indices.size();
}

void Model::Object::Draw() const
{
	glBindVertexArray(vao_id.Get());
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(vertex_count), GL_UNSIGNED_SHORT, nullptr);
}

// TMD model
Model::Model(const std::shared_ptr<Data> &_tmd_data) : tmd_data(_tmd_data)
{
	objects = std::make_unique<Object[]>(tmd_data->objects_size);

	for (uint32_t i = 0; i < tmd_data->objects_size; i++)
		objects[i].InitMesh(tmd_data->objects[i]);
}

void Model::Draw() const
{
	for (uint32_t i = 0; i < tmd_data->objects_size; i++)
		objects[i].Draw();
}

}
