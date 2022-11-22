#pragma once

#include "Base.h"
#include "Scanner.h"
#include <list>
#include <unordered_map>
#include <filesystem>

namespace GarbageHeaderTool
{

	struct Decorator
	{
		std::wstring Name;
		std::vector<std::wstring> Values;
	};

	struct GProperty
	{
		std::wstring Type;
		std::wstring Identifier;
		std::vector<Decorator> Decorators;

		bool HasDecorator(std::wstring_view name) const
		{
			for (auto& decorator : Decorators)
			{
				if (decorator.Name == name) return true;
			}

			return false;
		}
	};

	struct GMethod
	{
		std::wstring ReturnType;
		std::wstring Identifier;
		std::vector<std::wstring> Arguments;
		std::vector<Decorator> Decorators;

		bool HasDecorator(std::wstring_view name) const
		{
			for (auto& decorator : Decorators)
			{
				if (decorator.Name == name) return true;
			}

			return false;
		}
	};

	struct GClass
	{
		std::wstring Name;
		std::wstring FullPath;
		std::list<GProperty> Properties;
		std::vector<std::wstring> Parents;
		std::vector<Decorator> Decorators;
		int64 GeneratedBodyMacroLine;
		std::list<GMethod> Methods;
	};

	struct GStruct
	{
		std::wstring Name;
		const std::wstring& FullPath;
		std::list<GProperty> Properties;
		std::vector<Decorator> Decorators;
		std::list<GMethod> Methods;
		int64 GeneratedBodyMacroLine;
	};

	struct EnumValue
	{
		std::wstring Name;
		int64 Value;
	};

	struct GEnum
	{
		std::wstring Name;
		const std::wstring& FullPath;
		std::vector<EnumValue> Values;
		bool EnumClass = false;
	};

	class Parser
	{
	public:

		Parser(std::vector<Token>& tokens, const std::filesystem::path& path) : m_tokens(tokens), m_path(path) {}

		std::wstring GenerateHeaderFile(std::wstring& outFileId, const std::filesystem::path& outputPath);
		void DebugPrint();
		void Parse();

		bool CanGenerateHeaderFile() const { return m_structs.size() > 0 || m_classes.size() > 0 || m_enums.size() > 0; }

		std::vector<GStruct> GetStructs() { return m_structs; }
		std::vector<GClass> GetClasses() { return m_classes; }
		std::vector<GEnum> GetEnums() { return m_enums; }

		static std::wstring GetFileNameAsClassName(std::wstring fileName);

	private:

		int64 m_currentToken{ 0 };
		std::vector<Token>::iterator m_currentIter;
		std::filesystem::path m_path;

		std::vector<Token>& m_tokens;
		std::vector<GClass> m_classes;
		std::vector<GStruct> m_structs;
		std::vector<GEnum> m_enums;

		void ParseClass(std::vector<Decorator> decorators);
		void ParseStruct(std::vector<Decorator> decorators);
		void ParseEnum();
		GProperty ParseProperty(std::vector<Decorator>& decorators);
		GMethod ParseMethod(std::vector<Decorator>& decorators);
		const Token& Expect(TokenType type);
		bool Match(TokenType type);

		inline Token GenerateErrorToken() { return Token{ -1, -1, TokenType::Error, L"" }; }

		std::vector<Decorator> ParseDecorators();

	};

}