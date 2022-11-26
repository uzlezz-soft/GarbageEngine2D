#include "Core/Core.h"
#include "Core/Log.h"
#include "Core/Registry.h"
#include "GarbageEngineReflection.h"

#ifdef GARBAGE_PLATFORM_WINDOWS
extern "C"
{
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

namespace GarbageEngine
{

	void Init()
	{
		Log::Init();
		GarbageEngineReflection::Register();
	}

}