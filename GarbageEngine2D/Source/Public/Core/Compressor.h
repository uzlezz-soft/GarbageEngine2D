#pragma once

#include "Core/Base.h"
#include "Core/Assert.h"

class GARBAGE_API Compressor final
{
public:

	enum Level
	{
		Default, BestSpeed, BestCompression
	};

	struct Data
	{
		void* Ptr{ nullptr };
		uint64 Size{ 0 };
	};

	Compressor() = default;

	Data Compress(const void* data, uint64 size, Level level = Level::BestCompression) { GARBAGE_CORE_ASSERT(false); return {}; }

	Data Decompress(const void* data, uint64 size, uint64 decompressedDataSize) { GARBAGE_CORE_ASSERT(false); return {}; }

};