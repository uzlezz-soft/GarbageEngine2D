#include "Scanner.h"
#include "Parser.h"
#include "State.h"
#include "Timer.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <algorithm>

static std::wstring StringToWideString(const std::string& string)
{
	std::wstring ws(string.size(), L' ');
	ws.resize(std::mbstowcs(&ws[0], string.c_str(), string.size()));
	return ws;
}

static void CreateIfNotExists(const std::wstring& path)
{
	if (!std::filesystem::exists(path)) std::wofstream out(path);
}

static void ReplaceAll(std::wstring& str, const std::wstring& from, const std::wstring& to)
{
	if (from.empty()) return;

	uint64 startPos = 0;
	while ((startPos = str.find(from, startPos)) != std::wstring::npos)
	{
		str.replace(startPos, from.length(), to);
		startPos += to.length();
	}
}

static std::wstring GetFileContents(const std::wstring& path)
{
	std::wifstream in(path);
	std::wstring content((std::istreambuf_iterator<wchar_t>(in)), (std::istreambuf_iterator<wchar_t>()));

	return content;
}

int main(int argc, char** argv)
{
	Timer timer;

	std::vector<std::filesystem::path> pathsToScanFor_;
	std::vector<std::filesystem::path> pathsToScanFor;
	std::filesystem::path pathToOutputGeneratedStuff = L"";
	std::wstring projectName;
	std::wstring projectApi;
	bool forceGenerate = false;

	if (argc < 5)
	{
		std::cerr << "Usage: GarbageHeaderTool.exe -pPROJECT_NAME -aPROJECT_API -oOUT_PATH [-sSCAN_PATH]\n";
		return EXIT_FAILURE;
	}

	for (int32 i = 1; i < argc; i++)
	{
		std::string arg = argv[i];

		if (arg.starts_with("-s")) pathsToScanFor_.push_back(arg.substr(2));
		else if (arg.starts_with("-o")) pathToOutputGeneratedStuff = arg.substr(2);
		else if (arg.starts_with("-p")) projectName = StringToWideString(arg.substr(2));
		else if (arg.starts_with("-force")) forceGenerate = true;
	}

	if (pathToOutputGeneratedStuff.empty())
	{
		std::cerr << "Please, specify the output path with -oOUT_PATH!\n";
		return EXIT_FAILURE;
	}

	if (pathsToScanFor_.empty())
	{
		std::cerr << "Please, specify at least one scan path with -sSCAN_PATH!\n";
		return EXIT_FAILURE;
	}

	if (projectName.empty())
	{
		std::cerr << "Please, specify project name with -pPROJECT_NAME!\n";
		return EXIT_FAILURE;
	}

	for (auto& scanPath : pathsToScanFor_)
	{
		if (std::filesystem::exists(scanPath))
		{
			//std::wcerr << L"Scan path " << scanPath << L" not exist!" << std::endl;
			//return EXIT_FAILURE;
			pathsToScanFor.push_back(scanPath);
		}
	}

	if (!std::filesystem::exists(pathToOutputGeneratedStuff))
	{
		if (!std::filesystem::create_directories(pathToOutputGeneratedStuff))
		{
			std::cerr << "Can't find or create output directory!\n";
			return EXIT_FAILURE;
		}
	}

	std::wofstream globalReflectionOut(pathToOutputGeneratedStuff / (projectName + L"Reflection.h"));
	globalReflectionOut << L"#pragma once\n\n";

	std::vector<std::wstring> filesToInclude;
	std::vector<std::wstring> fileIds;

	auto outputPathString = pathToOutputGeneratedStuff.generic_u16string();

	for (auto& scanPath : pathsToScanFor)
	{
		for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(scanPath))
		{
			auto path = entry.path();

			if (path.generic_u16string().starts_with(outputPathString)) continue;

			if (entry.is_regular_file())
			{
				auto extension = path.extension().generic_string();
				std::transform(extension.begin(), extension.end(), extension.begin(),
					[](unsigned char c) { return std::tolower(c); });
				if (extension == ".h" || extension == ".hpp" || extension == ".h++" || extension == ".hh" || extension == ".hxx")
				{
					auto stem = path.stem().generic_wstring();
					auto generatedFileName = stem + L".generated.h";
					auto outputPath = pathToOutputGeneratedStuff / generatedFileName;

					if (std::filesystem::exists(outputPath) && !forceGenerate
						&& std::filesystem::last_write_time(path) < std::filesystem::last_write_time(outputPath))
					{
						const auto fileContent = GetFileContents(outputPath);

						static uint64 length = std::wstring(L"#define CURRENT_FILE_ID ").length();

						auto position = fileContent.find(L"#define CURRENT_FILE_ID ");
						if (position != std::wstring::npos)
						{
							auto fileId = fileContent.substr(position + length, 36);

							std::wstring relativeFilePath = std::filesystem::relative(path, scanPath).generic_wstring();
							ReplaceAll(relativeFilePath, L"\\", L"/");
							filesToInclude.push_back(relativeFilePath);
							fileIds.push_back(fileId);

							continue;
						}
					}

					GarbageHeaderTool::Scanner scanner(path, stem);
					if (scanner.IsIncludesSelfGeneratedVersion())
					{
						auto tokens = scanner.ScanTokens();

						if (scanner.HasAnyReflectionAttributes())
						{
							GarbageHeaderTool::Parser parser(tokens, path.generic_wstring());
							parser.Parse();

							if (!State::IsEverythingOk())
							{
								std::wcerr << State::GetExceptionText() << "\n";
								return EXIT_FAILURE;
							}

							std::wstring fileId;

							std::wofstream out(outputPath);
							out << parser.GenerateHeaderFile(fileId, scanPath);
							out.close();

							std::wstring relativeFilePath = std::filesystem::relative(path, scanPath).generic_wstring();
							ReplaceAll(relativeFilePath, L"\\", L"/");
							filesToInclude.push_back(relativeFilePath);
							fileIds.push_back(fileId);
						}
					}
				}
			}
		}
	}

	globalReflectionOut << L"#include \"Core/Minimal.h\"\n";

	for (auto& fileToInclude : filesToInclude)
	{
		globalReflectionOut << L"#include \"" << fileToInclude << L"\"\n";
	}

	globalReflectionOut << L"\n";
	for (auto& fileId : fileIds)
	{
		globalReflectionOut << L"\n_" << fileId << L"_IMPLEMENTATION";
	}
	globalReflectionOut << L"\n";

	globalReflectionOut << L"\nnamespace " << projectName << L"Reflection\n{\n\n\tvoid Register()\n\t{\n\t\tMeta::Registry & registry = Meta::Registry::Get(); \n\t";

	for (auto& fileId : fileIds)
	{
		globalReflectionOut << L"\n\t\t_" << fileId << L"_REGISTER_ENUMS(registry)";
	}

	for (auto& fileId : fileIds)
	{
		globalReflectionOut << L"\n\t\t_" << fileId << L"_REGISTER_TYPES(registry)";
	}

	for (auto& fileId : fileIds)
	{
		globalReflectionOut << L"\n\t\t_" << fileId << L"_REGISTER_INHERITANCE(registry)";
	}

	for (auto& fileId : fileIds)
	{
		globalReflectionOut << L"\n\t\t_" << fileId << L"_REGISTER_PROPERTIES(registry)";
	}

	globalReflectionOut << L"\n\t}\n\n}\n";

	globalReflectionOut.close();

	std::wcout << "Generating reflection took " << timer.GetElapsedSeconds() << "s" << std::endl;

	return 0;
}