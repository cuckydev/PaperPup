#pragma once

#include "Backend/VFS.h"

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

#include "Types/File.h"

namespace PaperPup::INT
{

// INT file structures
struct BlockHeader
{
	enum Type : uint32_t
	{
		Type_FIRST = 1,

		Type_TIM = 1,
		Type_VAB = 2,
		Type_MEM = 3,
		
		Type_LAST = 3,

		Type_END = 0xFFFFFFFFUL,
	};

	uint32_t type;
	uint32_t file_size;
	uint32_t data_sectors;
	uint32_t pad;
};
static_assert(sizeof(BlockHeader) == 16);
static_assert(alignof(BlockHeader) <= 4);

struct FileEntry
{
	uint32_t size;
	char name[16];
};
static_assert(sizeof(FileEntry) == 20);
static_assert(alignof(FileEntry) <= 4);

class INT;

// INT classes
class File : public PaperPup::Types::File
{
private:
	std::shared_ptr<PaperPup::Types::File> file;

	friend INT;
	File(const std::shared_ptr<PaperPup::Types::File> &_file, Types::Span<char> _span) : PaperPup::Types::File(_span), file(_file) {}
};

class INT
{
private:
	std::shared_ptr<PaperPup::Types::File> file;

	std::unordered_map<std::string, Types::Span<char>> spans;
	std::vector<std::string> tims;

public:
	INT(const std::shared_ptr<PaperPup::Types::File> &_file);
	~INT();

	bool Contains(const char *name) const;
	File *Open(const char *name) const;

	const std::vector<std::string> &TIMs() const;
};

}
