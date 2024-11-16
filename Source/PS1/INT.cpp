#include "PS1/INT.h"

#include <iostream>

namespace PaperPup::INT
{

static constexpr size_t BLOCK_SIZE = 0x800 * 4;

INT::INT(const std::shared_ptr<PaperPup::Types::File> &_file) : file(_file)
{
	// Read INT data
	Types::Span<char> data = _file->GetSpan(4);

	while (1)
	{
		// Read new block
		if (data.Size() < BLOCK_SIZE)
			throw Types::RuntimeException("INT doesn't contain at least one block");

		const BlockHeader *block = reinterpret_cast<const BlockHeader *>(data.Data());
		if (block->type == BlockHeader::Type::Type_END)
			break;
		if (block->type < BlockHeader::Type::Type_FIRST || block->type > BlockHeader::Type::Type_LAST)
			throw Types::RuntimeException("INT contains an unknown block type");
		
		const FileEntry *entry = reinterpret_cast<const FileEntry *>(data.Data() + sizeof(BlockHeader));
		if (block->file_size > (data.Size() - sizeof(BlockHeader)) / sizeof(FileEntry))
			throw Types::RuntimeException("INT contains too many files in one block");

		size_t offset = BLOCK_SIZE;
		size_t block_end = BLOCK_SIZE + block->data_sectors * 0x800;
		if (block_end > data.Size())
			throw Types::RuntimeException("INT block exceeds end of file");

		for (auto i = block->file_size; i != 0; i--, entry++)
		{
			// Check if this file exceeds the ending boundary
			if (offset + entry->size > block_end)
				throw Types::RuntimeException("INT file exceeds end of block");

			// Get string form of entry name
			char name[sizeof(entry->name) + 1];
			memcpy(name, entry->name, sizeof(entry->name));
			name[sizeof(entry->name)] = '\0';

			std::string name_string = name;

			// Insert file span
			// Track TIMs here since those are loaded automatically
			if (block->type == BlockHeader::Type::Type_TIM)
				tims.emplace_back(name_string);
			spans.emplace(name_string, Types::Span<char>(data.Data() + offset, entry->size));

			// Increment offset, ensure 4-byte alignment
			offset += entry->size;
			if (offset % 4)
				offset += 4 - (offset % 4);
		}

		data = data.Slice(block_end);
	}
}

INT::~INT()
{

}

bool INT::Contains(const char *name) const
{
	return spans.contains(name);
}

File *INT::Open(const char *name) const
{
	if (spans.contains(name))
		return new File(file, spans.at(name));
	return nullptr;
}

const std::vector<std::string> &INT::TIMs() const
{
	return tims;
}

}
