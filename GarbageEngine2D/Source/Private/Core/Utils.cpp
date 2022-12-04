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

	GARBAGE_API std::string ConvertBytesQuantityToHumanReadableFormat(uint64 amount)
	{
		static std::vector<const char*> suffix = { "B", "KiB", "MiB", "GiB", "TiB" };
		static auto length = suffix.size();

		double dblBytes = (double)amount;

		int i = 0;
		if (amount > 1024) 
		{
			for (i = 0; (amount / 1024) > 0 && i < length - 1; i++, amount /= 1024)
			{
				dblBytes = amount / 1024.0;
			}
		}

		return fmt::format("{:.2f} {}", dblBytes, suffix[i]);
	}

	// https://habr.com/ru/post/282191/
	GARBAGE_API uint32 ConvertUTF8ToUnicode(std::string_view text, uint64& i)
	{
		unsigned int a = text[i++];

		if ((a & 0x80) == 0) return a;

		if ((a & 0xE0) == 0xC0)
		{
			a = (a & 0x1F) << 6;
			a |= text[i++] & 0x3F;
		}
		else if ((a & 0xF0) == 0xE0)
		{
			a = (a & 0xF) << 12;
			a |= (text[i++] & 0x3F) << 6;
			a |= text[i++] & 0x3F;
		}
		else if ((a & 0xF8) == 0xF0)
		{
			a = (a & 0x7) << 18;
			a |= (a & 0x3F) << 12;
			a |= (text[i++] & 0x3F) << 6;
			a |= text[i++] & 0x3F;
		}

		return a;
	}

}