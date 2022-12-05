#pragma once

#include "Core/Base.h"

class GARBAGE_API MemoryStatistics
{
public:

	static uint64 GetVRamUsedForTextures() { return Get().m_vramUsedForTextures; }

private:

	uint64 m_vramUsedForTextures;

	MemoryStatistics() = default;

	static MemoryStatistics& Get();

	friend class Texture;

};