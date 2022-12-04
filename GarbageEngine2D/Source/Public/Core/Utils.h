#pragma once

#include "Core/Base.h"
#include <string>

namespace Utils
{

	GARBAGE_API std::string GetFileContents(std::string_view name);

	GARBAGE_API std::string ConvertBytesQuantityToHumanReadableFormat(uint64 amount);

	GARBAGE_API uint32 ConvertUTF8ToUnicode(std::string_view text, uint64& i);

}