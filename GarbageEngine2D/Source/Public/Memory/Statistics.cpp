#include "Statistics.h"

MemoryStatistics& MemoryStatistics::Get()
{
	static MemoryStatistics instance;
	return instance;
}
