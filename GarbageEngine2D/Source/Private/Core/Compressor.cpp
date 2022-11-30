#include "Core/Compressor.h"
#include "Core/Assert.h"
/*#pragma warning(push, 0)
#include <zlib/zlib.h>
#pragma warning(pop)

static int GarbageEngineCompressionLevelToZLibCompressionLevel(Compressor::Level level)
{
    switch (level)
    {
        case Compressor::Default: return Z_DEFAULT_COMPRESSION;
        case Compressor::BestSpeed: return Z_BEST_SPEED;
        case Compressor::BestCompression: return Z_BEST_COMPRESSION;
    }

    GARBAGE_CORE_ASSERT(level == Compressor::Level::Default || level == Compressor::Level::BestSpeed || level == Compressor::Level::BestCompression);
    return -1;
}*/

//Compressor::Data Compressor::Compress(const void* data, uint64 size, Level level /*= Compressor::level::BestCompression */)
/*{
    uint64 compressedDataSize = compressBound(size);
    void* compressedData = new void*[compressedDataSize];
    if (!compressedData)
    {
        GARBAGE_CORE_ERROR("Failed to allocate {} bytes of memory for compressedData", compressedDataSize);
        return {};
    }

    auto res = compress2((Bytef*)compressedData, (uLongf*)&compressedDataSize, (const Bytef*)data, size, GarbageEngineCompressionLevelToZLibCompressionLevel(level));
    if (res != Z_OK)
    {
        GARBAGE_CORE_ERROR("Failed to compress data: {}", res);
        return {};
    }

    Compressor::Data result;
    result.Size = compressedDataSize;
    result.Ptr = compressedData;

    return result;
}

Compressor::Data Compressor::Decompress(const void* data, uint64 size, uint64 decompressedDataSize)
{
    Compressor::Data result;
    result.Size = decompressedDataSize;
    result.Ptr = new void*[decompressedDataSize];

    auto res = uncompress((Bytef*)result.Ptr, (uLongf*)&result.Size, (const Bytef*)data, size);
    if (res != Z_OK)
    {
        GARBAGE_CORE_ERROR("Failed to decompress data: {}", res);
        return {};
    }

    return result;
}*/