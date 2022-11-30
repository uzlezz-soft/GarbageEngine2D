#include "Core/Utils.h"
#include "Core/Assert.h"
#include <fstream>

namespace Utils
{

	std::string GetFileContents(std::string_view name)
	{
		std::string result;
		std::ifstream in(name.data(), std::ios::in | std::ios::binary);

		if (in.is_open())
		{
			in.seekg(0, std::ios::end);
			size_t size = in.tellg();
			if (size != -1)
			{
				result.resize(size);
				in.seekg(0, std::ios::beg);
				in.read(&result[0], size);
			}
			else
			{
				GARBAGE_CORE_ASSERT(false, "Can't read file contents at path {}", name.data());
			}
		}
		else
		{
			GARBAGE_CORE_ASSERT(false, "Can't open file at path {}", name.data());
		}

		return result;
	}

}