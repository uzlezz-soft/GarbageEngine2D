#include "Parser.h"
#include "State.h"
#include <sstream>
#include <ctime>
#include <chrono>
#include <iostream>
#include <random>
#include <codecvt>
#include <filesystem>

namespace Utils
{

	static bool IsAlpha(wchar_t c)
	{
		return (c >= L'a' && c <= L'z') || (c >= L'A' && c <= L'Z')
			|| (c >= L'à' && c <= L'ß') || (c >= L'À' && c <= L'ß');
	}

	static bool IsAlphaNumeric(wchar_t c)
	{
		return IsAlpha(c) || (c >= L'0' && c <= L'9');
	}

	static bool IsVariableFriendly(wchar_t c)
	{
		return IsAlphaNumeric(c) || c == L'_';
	}

	static std::wstring GetRandomIdentifier() 
	{
		static std::random_device dev;
		static std::mt19937 rng(dev());

		std::uniform_int_distribution<int> dist(0, 15);

		const char* v = "0123456789abcdef";
		const bool dash[] = { 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0 };

		std::wstring result;
		for (int i = 0; i < 16; i++)
		{
			if (dash[i]) result += L"_";

			result += v[dist(rng)];
			result += v[dist(rng)];
		}
		return result;
	}

	void ReplaceAll(std::wstring& str, const std::wstring& from, const std::wstring& to)
	{
		if (from.empty()) return;

		uint64 startPos = 0;
		while ((startPos = str.find(from, startPos)) != std::wstring::npos) 
		{
			str.replace(startPos, from.length(), to);
			startPos += to.length();
		}
	}

	std::string wstring_to_utf8(const std::wstring& str)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
		return conv.to_bytes(str);
	}

#ifndef USE_NEW_SYSTEM_THAT_DOESNT_SHIT_IN_INTELLISENSE
#define SERIALIZE_TYPE_(type) if (property.Type == L## #type) { out << "a << ArchiveManipulator::Key << \"" << property.Identifier << L"\" << ArchiveManipulator::Value << o->*(" << L## #type << L" ObjectBase::*)" << typeName << L"::Z_" << fileId << L"_GET_PROP_ADDRESS_" << property.Identifier << L"();"; }
#define SERIALIZE_TYPE(type) else if (property.Type == L## #type) { out << "a << ArchiveManipulator::Key << \"" << property.Identifier << L"\" << ArchiveManipulator::Value << o->*(" << L## #type << L" ObjectBase::*)" << typeName << L"::Z_" << fileId << L"_GET_PROP_ADDRESS_" << property.Identifier << L"();"; }
#else
#define SERIALIZE_TYPE_(type) if (property.Type == L## #type) { out << "a << ArchiveManipulator::Key << \"" << property.Identifier << L"\" << ArchiveManipulator::Value << o->*(" << L## #type << L" ObjectBase::*)Z_" << typeName << L"_" << fileId << L"_GET_PROP_ADDRESS_" << property.Identifier << L"();"; }
#define SERIALIZE_TYPE(type) else if (property.Type == L## #type) { out << "a << ArchiveManipulator::Key << \"" << property.Identifier << L"\" << ArchiveManipulator::Value << o->*(" << L## #type << L" ObjectBase::*)Z_" << typeName << L"_" << fileId << L"_GET_PROP_ADDRESS_" << property.Identifier << L"();"; }

#endif

	void GenerateSerializer(std::wostream& out, std::list<GarbageHeaderTool::GProperty>& properties, std::wstring_view fileId,
		std::wstring_view typeName, std::vector<GarbageHeaderTool::GEnum>& enums)
	{
		for (auto& property : properties)
		{
			if (property.HasDecorator(L"DontSerialize")) continue;

			out << " \\\n\t\t";

			SERIALIZE_TYPE_(int8)
			SERIALIZE_TYPE(uint8)
			SERIALIZE_TYPE(int16)
			SERIALIZE_TYPE(uint16)
			SERIALIZE_TYPE(int32)
			SERIALIZE_TYPE(uint32)
			SERIALIZE_TYPE(int64)
			SERIALIZE_TYPE(uint64)
			SERIALIZE_TYPE(bool)
			SERIALIZE_TYPE(float)
			SERIALIZE_TYPE(double)
			SERIALIZE_TYPE(char*)
			SERIALIZE_TYPE(const char*)
			SERIALIZE_TYPE(std::string)
			SERIALIZE_TYPE(std::string_view)
			SERIALIZE_TYPE(const std::string)
			SERIALIZE_TYPE(const std::string_view)
			SERIALIZE_TYPE(std::string&)
			SERIALIZE_TYPE(std::string_view&)
			SERIALIZE_TYPE(const std::string&)
			SERIALIZE_TYPE(const std::string_view&)
			SERIALIZE_TYPE(int)
			SERIALIZE_TYPE(long)
			SERIALIZE_TYPE(unsigned)
			SERIALIZE_TYPE(unsigned long)
			SERIALIZE_TYPE(long int)
			SERIALIZE_TYPE(unsigned long int)
			SERIALIZE_TYPE(unsigned int)
			SERIALIZE_TYPE(unsigned long long)
			SERIALIZE_TYPE(long long)
			SERIALIZE_TYPE(Vector2)
			SERIALIZE_TYPE(Vector3)
			SERIALIZE_TYPE(Vector4)
			SERIALIZE_TYPE(Color)
			SERIALIZE_TYPE(Quaternion)
			else
			{
				for (auto& enumerator : enums)
				{
					if (property.Type == enumerator.Name)
					{
#ifndef USE_NEW_SYSTEM_THAT_DOESNT_SHIT_IN_INTELLISENSE
						out << "a << ArchiveManipulator::Key << \"" << property.Identifier << L"\" << ArchiveManipulator::Value << std::string(\"" << enumerator.Name << "::\").append(registry.FindEnum(\"" << enumerator.Name << "\")->ValueToString((int64)(o->*(" << enumerator.Name << " ObjectBase::*)" << typeName << L"::Z_" << fileId << L"_GET_PROP_ADDRESS_" << property.Identifier << L"())));";
#else
						out << "a << ArchiveManipulator::Key << \"" << property.Identifier << L"\" << ArchiveManipulator::Value << std::string(\"" << enumerator.Name << "::\").append(registry.FindEnum(\"" << enumerator.Name << "\")->ValueToString((int64)(o->*(" << enumerator.Name << " ObjectBase::*)Z_" << typeName << "_" << fileId << L"_GET_PROP_ADDRESS_" << property.Identifier << L"())));";
#endif
						break;
					}
				}
			}

			/*if (property.Type == L"int8" || property.Type == L"uint8" || property.Type == L"int16" || property.Type == L"uint16"
				|| property.Type == L"int32" || property.Type == L"uint32" || property.Type == L"int64" || property.Type == L"uint64"
				|| property.Type == L"bool" || property.Type == L"float" || property.Type == L"double"
				|| property.Type == L"char*" || property.Type == L"const char*" || property.Type == L"std::string"
				|| property.Type == L"std::string_view" || property.Type == L"const std::string" || property.Type == L"const std::string_view"
				|| property.Type == L"std::string&" || property.Type == L"std::string_view&" || property.Type == L"const std::string&"
				|| property.Type == L"const std::string_view&" || property.Type == L"int" || property.Type == L"long" || property.Type == L"unsigned"
				|| property.Type == L"unsigned long" || property.Type == L"long int" || property.Type == L"long long" || property.Type == L"unsigned long long"
				|| property.Type == L"Vector2" || property.Type == L"Vector3" || property.Type == L"Vector4" || property.Type == L"Color" || property.Type == L"Quaternion")
			{
				out << "a << ArchiveManipulator::Key << \"" << property.Identifier << L"\" << ArchiveManipulator::Value << o->*" << typeName << L"::Z_" << fileId << L"_GET_PROP_ADDRESS_" << property.Identifier << L"();";
			}
			else
			{
				for (auto& enumerator : enums)
				{
					if (property.Type == enumerator.Name)
					{
						out << "a << ArchiveManipulator::Key << \"" << property.Identifier << L"\" << ArchiveManipulator::Value << \"" << enumerator.Name << "::\" << o->*" << typeName << L"::Z_" << fileId << L"_GET_PROP_ADDRESS_" << property.Identifier << L"();";
						break;
					}
				}
			}*/
		}
	}

}

namespace GarbageHeaderTool
{

#define ENUM_TO_STRING(ENUM, X) case ENUM::X: return L## #X;
#define ENUM_TO_STRING_VALUE(ENUM, X, V) case ENUM::X: return L##V;

	std::wstring TokenTypeToString(TokenType type)
	{
		switch (type)
		{
			ENUM_TO_STRING(TokenType, Property);
			ENUM_TO_STRING(TokenType, Method);
			ENUM_TO_STRING(TokenType, Class);
			ENUM_TO_STRING(TokenType, Struct);
			ENUM_TO_STRING(TokenType, Enum);
			ENUM_TO_STRING(TokenType, StructKeyword);
			ENUM_TO_STRING(TokenType, ClassKeyword);
			ENUM_TO_STRING(TokenType, EnumKeyword);
			ENUM_TO_STRING_VALUE(TokenType, Colon, ",");

			ENUM_TO_STRING_VALUE(TokenType, LeftParen, "(");
			ENUM_TO_STRING_VALUE(TokenType, RightParen, ")");
			ENUM_TO_STRING_VALUE(TokenType, Hashtag, "#");
			ENUM_TO_STRING_VALUE(TokenType, LeftBracket, "[");
			ENUM_TO_STRING_VALUE(TokenType, RightBracket, "]");
			ENUM_TO_STRING_VALUE(TokenType, SemiColon, ";");
			ENUM_TO_STRING_VALUE(TokenType, Equal, "=");
			ENUM_TO_STRING_VALUE(TokenType, PlusEqual, "+=");
			ENUM_TO_STRING_VALUE(TokenType, MinusEqual, "-=");
			ENUM_TO_STRING_VALUE(TokenType, TimesEqual, "*=");
			ENUM_TO_STRING_VALUE(TokenType, DivideEqual, "/=");
			ENUM_TO_STRING_VALUE(TokenType, ModuleEqual, "%=");
			ENUM_TO_STRING_VALUE(TokenType, CaretEqual, "^=");
			ENUM_TO_STRING_VALUE(TokenType, AmpersandEqual, "&=");
			ENUM_TO_STRING_VALUE(TokenType, BarEqual, "|=");
			ENUM_TO_STRING_VALUE(TokenType, RightShiftEqual, ">>=");
			ENUM_TO_STRING_VALUE(TokenType, LeftShiftEqual, "<<=");
			ENUM_TO_STRING_VALUE(TokenType, EqualEqual, "==");
			ENUM_TO_STRING_VALUE(TokenType, BangEqual, "!=");
			ENUM_TO_STRING_VALUE(TokenType, LessThanEqual, "<");
			ENUM_TO_STRING_VALUE(TokenType, GreaterThanEqual, ">");

			ENUM_TO_STRING(TokenType, ConstKeyword);
			ENUM_TO_STRING(TokenType, Identifier);
			ENUM_TO_STRING(TokenType, AutoKeyword);

			ENUM_TO_STRING_VALUE(TokenType, Star, "*");
			ENUM_TO_STRING_VALUE(TokenType, Reference, "&");
			ENUM_TO_STRING_VALUE(TokenType, LeftAngleBracket, "{");
			ENUM_TO_STRING_VALUE(TokenType, RightAngleBracket, "}");

			ENUM_TO_STRING(TokenType, StringLiteral);
			ENUM_TO_STRING(TokenType, Number);
			ENUM_TO_STRING(TokenType, TrueKeyword);
			ENUM_TO_STRING(TokenType, FalseKeyword);

			ENUM_TO_STRING(TokenType, EndOfFile);
			ENUM_TO_STRING(TokenType, Error);
		}

		return L"UNKNOWN TOKEN";
	}

	std::wstring Parser::GetFileNameAsClassName(std::wstring fileName)
	{
		for (int i = 0; i < fileName.length(); i++)
		{
			if (i == 0 && !Utils::IsAlpha(fileName[i]))
			{
				fileName[i] = '_';
			}
			else if (!Utils::IsVariableFriendly(fileName[i]))
			{
				fileName[i] = '_';
			}
		}

		return fileName;
	}

	std::wstring Parser::GenerateHeaderFile(std::wstring& outFileId, const std::filesystem::path& outputPath)
	{
		std::wostringstream out;

		std::map<std::wstring, std::wstring> ids;

		for (auto& $class : m_classes)
		{
			ids.insert(std::make_pair($class.Name, Utils::GetRandomIdentifier()));
		}

		std::wstring baseFilename = std::filesystem::relative(m_path, outputPath).generic_wstring();
		std::wstring friendlyBaseFilename = baseFilename;
		Utils::ReplaceAll(friendlyBaseFilename, L".", L"_");
		Utils::ReplaceAll(friendlyBaseFilename, L"/", L"_");
		Utils::ReplaceAll(friendlyBaseFilename, L"\\", L"_");

		std::time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		out << L"// Generated by Garbage Header Tool\n// " << std::ctime(&time) << L"\n\n";
		out << L"#pragma once\n\n";
		

		auto fileId = Utils::GetRandomIdentifier();
		auto define = std::wstring(L"_META_" + friendlyBaseFilename + L"_" + fileId + L"_");

		outFileId = fileId;

		out << L"#ifdef " << define << L"\n";
		out << L"#error \"" << baseFilename << " already included, missing '#pragma once' in " << baseFilename << "\"\n";
		out << L"#endif\n";
		out << L"#define " << define << "\n\n";

		out << L"#undef CURRENT_FILE_ID\n";
		out << L"#define CURRENT_FILE_ID " << fileId << L"\n\n";

		out << L"#define _" << fileId << "_REGISTER_ENUMS(registry) \\\n{ \\\n";
		for (auto& enumerator : m_enums)
		{
			out << L"\tregistry.AddEnum(\"" << enumerator.Name << L"\") \\\n";
			for (auto& value : enumerator.Values)
			{
				out << L"\t\t.AddValue(\"" << value.Name << "\", " << value.Value << L") \\\n";
			}
			out << L"; \\\n";
		}
		out << L"}\n\n";

		out << L"#define _" << fileId << "_REGISTER_TYPES(registry) \\\n{ \\\n";
		for (auto& $class : m_classes)
		{
			out << L"\tregistry.AddType<" << $class.Name << L">(\"" << $class.Name << L"\"";

			if ($class.Properties.size() > 0)
			{
				out << L", [&registry](Archive& a, ObjectBase* o_) \\\n\t{";

				out << L" \\\n\t\t" << $class.Name << "* o = (" << $class.Name << "*)o_;";
				out << L" \\\n\t\t a << ArchiveManipulator::BeginMap;";

				Utils::GenerateSerializer(out, $class.Properties, fileId, $class.Name, m_enums);

				out << L" \\\n\t\t a << ArchiveManipulator::EndMap;";

				out << L" \\\n\t}";
			}
			else out << L", {}";

			if ($class.Methods.size() > 0)
			{
				for (auto& method : $class.Methods)
				{
					std::wcout << "Method in class " << $class.Name << ": " << method.Identifier << L", returns " << method.ReturnType << "\n";
				}
			}

			if ($class.Decorators.empty()) out << L"); \\\n";
			else
			{
				out << L", std::initializer_list<Meta::Decorator>{ ";

				for (uint8 i = 0; i < $class.Decorators.size(); i++)
				{
					if (i > 0) out << L", ";

					Decorator& decorator = $class.Decorators[i];
					out << L"{ \"" << decorator.Name << L"\", {";

					for (uint8 j = 0; j < decorator.Values.size(); j++)
					{
						if (j > 0) out << L", ";
						out << L"\"" << decorator.Values[j] << "\"";
					}

					out << L"} }";
				}

				out << L" }); \\\n";
			}
		}

		for (auto& structure : m_structs)
		{
			out << L"\tregistry.AddType<" << structure.Name << L">(\"" << structure.Name << L"\"";

			if (structure.Decorators.empty()) out << L"); \\\n";
			else
			{
				out << L", std::initializer_list<Meta::Decorator>{ ";

				for (uint8 i = 0; i < structure.Decorators.size(); i++)
				{
					if (i > 0) out << L", ";

					Decorator& decorator = structure.Decorators[i];
					out << L"{ \"" << decorator.Name << L"\", {";

					for (uint8 j = 0; j < decorator.Values.size(); j++)
					{
						if (j > 0) out << L", ";
						out << L"\"" << decorator.Values[j] << "\"";
					}

					out << L"} }";
				}

				out << L" }); \\\n";
			}
		}
		out << L"}\n\n";

		out << L"#define _" << fileId << "_REGISTER_INHERITANCE(registry) \\\n{ \\\n";
		for (auto& $class : m_classes)
		{
			out << L"\t((Meta::Type*)(registry.FindType(\"" << $class.Name << L"\")))";
			
			uint64 i = 0;
			for (auto& parent : $class.Parents)
			{
				if (!parent.empty() && parent != L"public" && parent != L"protected" && parent != L"private")
				{
					if (i == 0) out << L" \\\n\t\t->";
					else out << L" \\\n\t\t.";
					i++;

					out << L"AddParent(registry.FindType(\"" << parent << L"\"))";
				}
			}

			out << "; \\\n";
		}
		out << L"}\n\n";

		out << L"#define _" << fileId << "_REGISTER_PROPERTIES(registry) \\\n{ \\\n";
		for (auto& $class : m_classes)
		{
			if ($class.Properties.size() < 1) continue;

			out << L"\t((Meta::Type*)(registry.FindType(\"" << $class.Name << L"\")))";

			uint64 i = 0;
			for (auto& property : $class.Properties)
			{
				if (i == 0) out << L" \\\n\t\t->";
				else out << L" \\\n\t\t.";
				i++;

#ifndef USE_NEW_SYSTEM_THAT_DOESNT_SHIT_IN_INTELLISENSE
				out << L"AddProperty(\"" << property.Identifier << L"\", \"" << property.Type << L"\", " << $class.Name << L"::Z_" << fileId << L"_GET_PROP_ADDRESS_" << property.Identifier << L"()";
#else
				out << L"AddProperty(\"" << property.Identifier << L"\", \"" << property.Type << L"\", Z_" << $class.Name << L"_" << fileId << L"_GET_PROP_ADDRESS_" << property.Identifier << L"()";
#endif

				if (property.Decorators.empty()) out << L")";
				else
				{
					out << L", std::initializer_list<Meta::Decorator>{ ";

					for (uint8 i = 0; i < property.Decorators.size(); i++)
					{
						if (i > 0) out << L", ";
						
						Decorator& decorator = property.Decorators[i];
						out << L"{ \"" << decorator.Name << L"\", {";

						for (uint8 j = 0; j < decorator.Values.size(); j++)
						{
							if (j > 0) out << L", ";
							out << L"\"" << decorator.Values[j] << "\"";
						}

						out << L"} }";
					}

					out << L" })";
				}
			}

			out << L"; \\\n";
		}

		for (auto& structure : m_structs)
		{
			if (structure.Properties.size() < 1) continue;

			out << L"\t((Meta::Type*)(registry.FindType(\"" << structure.Name << L"\")))";

			uint64 i = 0;
			for (auto& property : structure.Properties)
			{
				if (i == 0) out << L" \\\n\t\t->";
				else out << L" \\\n\t\t.";
				i++;

#ifndef USE_NEW_SYSTEM_THAT_DOESNT_SHIT_IN_INTELLISENSE
				out << L"AddProperty(\"" << property.Identifier << L"\", \"" << property.Type << L"\", " << structure.Name << L"::Z_" << fileId << L"_GET_PROP_ADDRESS_" << property.Identifier << L"()";
#else
				out << L"AddProperty(\"" << property.Identifier << L"\", \"" << property.Type << L"\", Z_" << structure.Name << L"_" << fileId << L"_GET_PROP_ADDRESS_" << property.Identifier << L"()";
#endif
				if (property.Decorators.empty()) out << L")";
				else
				{
					out << L", std::initializer_list<Meta::Decorator>{ ";

					for (uint8 i = 0; i < property.Decorators.size(); i++)
					{
						if (i > 0) out << L", ";

						Decorator& decorator = property.Decorators[i];
						out << L"{ \"" << decorator.Name << L"\", {";

						for (uint8 j = 0; j < decorator.Values.size(); j++)
						{
							if (j > 0) out << L", ";
							out << L"\"" << decorator.Values[j] << "\"";
						}

						out << L"} }";
					}

					out << L" })";
				}
			}

			out << L"; \\\n";
		}
		out << L"}\n\n";

#ifdef USE_NEW_SYSTEM_THAT_DOESNT_SHIT_IN_INTELLISENSE
		for (auto& $class : m_classes)
		{
			out << L"const Meta::Type* Z_" << $class.Name << L"_" << fileId << "_GET_TYPE();\n";
		}

		for (auto& structure : m_structs)
		{
			out << L"const Meta::Type* Z_" << structure.Name << L"_" << fileId << "_GET_TYPE();\n";
		}
#endif

#ifndef USE_NEW_SYSTEM_THAT_DOESNT_SHIT_IN_INTELLISENSE
		for (auto& $class : m_classes)
		{
			out << "#define _" << fileId << "_" << $class.GeneratedBodyMacroLine << "_GENERATED_BODY";

			out << L" \\\nprivate: static Meta::Type* Z_m_type; friend Meta::Registry;";

			out << L" \\\npublic: virtual const Meta::Type* GetType() const override { return GetStaticType(); }";
			out << L" \\\nstatic const Meta::Type* GetStaticType() { return Z_m_type; }";

			for (auto& property : $class.Properties)
			{
				out << L" \\\npublic: static void* ObjectBase::* Z_" << fileId << L"_GET_PROP_ADDRESS_" << property.Identifier << L"() { return (void* ObjectBase::*)&" << $class.Name << L"::" << property.Identifier << L"; }";
			}

			out << L"\n\n";
		}

		if (m_structs.size() > 0)
		{
			for (auto& structure : m_structs)
			{
				out << "#define _" << fileId << "_" << structure.GeneratedBodyMacroLine << "_GENERATED_BODY";

				out << L" \\\nprivate: friend class Meta::Type; friend class Meta::Registry; static Meta::Type* Z_m_type; public:";

				for (auto& property : structure.Properties)
				{
					out << L" \\\npublic: static void* ObjectBase::* Z_" << fileId << L"_GET_PROP_ADDRESS_" << property.Identifier << L"() { return (void* ObjectBase::*)&" << structure.Name << L"::" << property.Identifier << L"; }";
				}

				out << L"\n\n";
			}


			out << "#define _" << fileId << "_IMPLEMENTATION";

			for (auto& structure : m_structs)
			{
				out << L" \\\nMeta::Type* " << structure.Name << "::Z_m_type = nullptr;";
			}
		}
		else
		{
			out << "#define _" << fileId << "_IMPLEMENTATION";
		}

		for (auto& $class : m_classes)
		{
			out << L" \\\nMeta::Type* " << $class.Name << L"::Z_m_type = nullptr;";
		}
#else
		for (auto& $class : m_classes)
		{
			out << "#define _" << fileId << "_" << $class.GeneratedBodyMacroLine << "_GENERATED_BODY";

			out << L" \\\nprivate: friend Meta::Registry;";

			out << L" \\\npublic: virtual const Meta::Type* GetType() const override { return GetStaticType(); }";
			out << L" \\\nstatic const Meta::Type* GetStaticType() { return Z_" << $class.Name << L"_" << fileId << "_GET_TYPE(); }";
			out << L" \\\nvirtual ~" << $class.Name << "() = default;";

			for (auto& property : $class.Properties)
			{
				out << L" \\\nfriend void* ObjectBase::* Z_" << $class.Name << L"_" << fileId << L"_GET_PROP_ADDRESS_" << property.Identifier << L"();"; // { return (void* ObjectBase::*)&" << $class.Name << L"::" << property.Identifier << L"; }";
			}

			out << L"\n\n";
		}

		if (m_structs.size() > 0)
		{
			for (auto& structure : m_structs)
			{
				out << "#define _" << fileId << "_" << structure.GeneratedBodyMacroLine << "_GENERATED_BODY";

				out << L" \\\nprivate: friend class Meta::Type; friend class Meta::Registry;";

				out << L" \\\npublic: const Meta::Type* GetType() const { return GetStaticType(); }";
				out << L" \\\nstatic const Meta::Type* GetStaticType() { return Z_" << structure.Name << L"_" << fileId << "_GET_TYPE(); }";

				for (auto& property : structure.Properties)
				{
					out << L" \\\nfriend void* ObjectBase::* Z_" << structure.Name << L"_" << fileId << L"_GET_PROP_ADDRESS_" << property.Identifier << L"();"; // { return (void* ObjectBase::*)&" << $class.Name << L"::" << property.Identifier << L"; }";
				}

				out << L" public:\n\n";
			}


			out << "#define _" << fileId << "_IMPLEMENTATION";

			for (auto& structure : m_structs)
			{
				out << L" \\\nconst Meta::Type* Z_" << structure.Name << L"_" << fileId << "_GET_TYPE() { static const Meta::Type* _t = Meta::Registry::Get().FindType(\"" << structure.Name << L"\"); return _t; }";

				for (auto& property : structure.Properties)
				{
					out << L" \\\nvoid* ObjectBase::* Z_" << structure.Name << L"_" << fileId << L"_GET_PROP_ADDRESS_" << property.Identifier << L"() { return (void* ObjectBase::*)&" << structure.Name << L"::" << property.Identifier << L"; }";
				}
			}
		}
		else
		{
			out << "#define _" << fileId << "_IMPLEMENTATION";
		}

		for (auto& $class : m_classes)
		{
			out << L" \\\nconst Meta::Type* Z_" << $class.Name << L"_" << fileId << "_GET_TYPE() { static const Meta::Type* _t = Meta::Registry::Get().FindType(\"" << $class.Name << L"\"); return _t; }";

			for (auto& property : $class.Properties)
			{
				out << L" \\\nvoid* ObjectBase::* Z_" << $class.Name << L"_" << fileId << L"_GET_PROP_ADDRESS_" << property.Identifier << L"() { return (void* ObjectBase::*)&" << $class.Name << L"::" << property.Identifier << L"; }";
			}
		}
#endif

		for (auto& enumerator : m_enums)
		{
			if (enumerator.EnumClass)
			{
				out << L" \\\nFORCEINLINE " << enumerator.Name << L" operator|(" << enumerator.Name << L" a, " << enumerator.Name << L" b)";
				out << L" \\\n{ \\\n\treturn static_cast<" << enumerator.Name << L">(static_cast<int64>(a) | static_cast<int64>(b));";
				out << L" \\\n}";
			}
		}

		return out.str();
	}

	void Parser::DebugPrint()
	{
		/*for (auto it = m_structs.begin(); it != m_structs.end(); it++)
		{
			std::wcout << L"struct " << it->Name << L" {\n";

			for (auto propIter = it->Properties.begin(); propIter != it->Properties.end(); propIter++)
			{
				for (uint8 i = 0; i < propIter->Decorators.size(); i++)
				{
					if (i > 0) std::wcout << L", ";
					std::wcout << propIter->Decorators[i];
				}

				std::wcout << L"	Type<" << propIter->Type << L"> " << propIter->Identifier << L"\n";
			}
			std::wcout << L"}\n";
		}

		for (auto it = m_classes.begin(); it != m_classes.end(); it++)
		{
			std::wcout << L"class " << it->Name << L" {\n";

			for (auto propIter = it->Properties.begin(); propIter != it->Properties.end(); propIter++)
			{
				for (uint8 i = 0; i < propIter->Decorators.size(); i++)
				{
					if (i > 0) std::wcout << L", ";
					std::wcout << propIter->Decorators[i];
				}

				std::wcout << L"	Type<" << propIter->Type << L"> " << propIter->Identifier << L"\n";
			}
			std::wcout << L"}\n";
		}*/
	}

	void Parser::Parse()
	{
		m_currentToken = 0;
		m_currentIter = m_tokens.begin();

		do
		{
			if (Match(TokenType::Struct))
			{
				Expect(TokenType::LeftParen);

				auto decorators = ParseDecorators();

				Expect(TokenType::RightParen);
				Match(TokenType::SemiColon);
				Expect(TokenType::StructKeyword);
				ParseStruct(decorators);
			}
			else if (Match(TokenType::Class))
			{
				Expect(TokenType::LeftParen);

				auto decorators = ParseDecorators();

				Expect(TokenType::RightParen);
				Match(TokenType::SemiColon);
				Expect(TokenType::ClassKeyword);
				ParseClass(decorators);
			}
			else if (Match(TokenType::Enum))
			{
				Expect(TokenType::LeftParen);
				Expect(TokenType::RightParen);
				Match(TokenType::SemiColon);
				Expect(TokenType::EnumKeyword);
				ParseEnum();
			}
			else
			{
				m_currentToken++;
				m_currentIter++;
			}
		} while ((uint64)m_currentToken < m_tokens.size() && m_currentIter->Type != TokenType::EndOfFile);
	}

	void Parser::ParseClass(std::vector<Decorator> decorators)
	{
		Token classType = Expect(TokenType::Identifier);
		if (Match(TokenType::Identifier)) classType = *(m_currentIter - 1); // *_API
		if (Match(TokenType::Identifier)) classType = *(m_currentIter - 2); // final

		std::vector<std::wstring> parents;
		if (Match(TokenType::Colon))
		{
			do
			{
				m_currentToken++;
				m_currentIter++;
				if (m_currentIter->Type != TokenType::LeftBracket) parents.push_back(m_currentIter->Lexeme);
			} while (!Match(TokenType::LeftBracket));
		}
		else
		{
			Expect(TokenType::LeftBracket);
		}

		GClass clazz = GClass{ classType.Lexeme, m_path.generic_wstring(), std::list<GProperty>(), parents, decorators };

		int64 level = 1;
		while (m_currentIter->Type != TokenType::EndOfFile)
		{
			if (Match(TokenType::LeftBracket))
			{
				level++;
			}
			else if (Match(TokenType::RightBracket))
			{
				level--;
				if (level <= 0)
				{
					Expect(TokenType::SemiColon);
					break;
				}
			}
			else if (Match(TokenType::GeneratedBodyMacro))
			{
				clazz.GeneratedBodyMacroLine = m_currentIter->Line;
				Expect(TokenType::LeftParen);
				Expect(TokenType::RightParen);
				Match(TokenType::SemiColon);
			}
			else if (Match(TokenType::Property))
			{
				Expect(TokenType::LeftParen);

				// Finding decorators
				auto propertyDecorators = ParseDecorators();

				Expect(TokenType::RightParen);
				Match(TokenType::SemiColon); // Consume a semicolon if it is there, this is to help with indentation
				clazz.Properties.push_back(ParseProperty(propertyDecorators));
			}
			else if (Match(TokenType::Method))
			{
				Expect(TokenType::LeftParen);

				// Finding decorators
				auto methodDecorators = ParseDecorators();

				Expect(TokenType::RightParen);
				Match(TokenType::SemiColon); // Consume a semicolon if it is there, this is to help with indentation
				clazz.Methods.push_back(ParseMethod(methodDecorators));
			}
			else
			{
				m_currentToken++;
				m_currentIter++;
			}
		}

		m_classes.push_back(clazz);
	}

	void Parser::ParseStruct(std::vector<Decorator> decorators)
	{
		Token structType = Expect(TokenType::Identifier);
		if (Match(TokenType::Identifier)) structType = *(m_currentIter - 1); // *_API
		Expect(TokenType::LeftBracket);

		GStruct structure = GStruct{ structType.Lexeme, m_path.generic_wstring(), std::list<GProperty>(), decorators, std::list<GMethod>() };

		int64 level = 1;
		while (m_currentIter->Type != TokenType::EndOfFile)
		{
			if (Match(TokenType::LeftBracket))
			{
				level++;
			}
			else if (Match(TokenType::RightBracket))
			{
				level--;
				if (level <= 1)
				{
					Expect(TokenType::SemiColon);
					break;
				}
			}
			else if (Match(TokenType::GeneratedBodyMacro))
			{
				structure.GeneratedBodyMacroLine = m_currentIter->Line;
				Expect(TokenType::LeftParen);
				Expect(TokenType::RightParen);
				Match(TokenType::SemiColon);
			}
			else if (Match(TokenType::Property))
			{
				Expect(TokenType::LeftParen);

				// Finding decorators
				auto propertyDecorators = ParseDecorators();

				Expect(TokenType::RightParen);
				Match(TokenType::SemiColon); // Consume a semicolon if it is there, this is to help with indentation
				structure.Properties.push_back(ParseProperty(propertyDecorators));
			}
			// TODO: This might need another else statement like class parser
		}

		m_structs.push_back(structure);
	}

	void Parser::ParseEnum()
	{
		bool enumClass = Match(TokenType::ClassKeyword);
		Token enumType = Expect(TokenType::Identifier);
		if (Match(TokenType::Identifier)) enumType = *(m_currentIter - 1); // *_API
		Expect(TokenType::LeftBracket);

		GEnum enumerator = GEnum{ enumType.Lexeme, m_path.generic_wstring(), std::vector<EnumValue>(), enumClass };
		int64 lastValue = -1;

		do
		{
			auto token = Expect(TokenType::Identifier);
			if (token.Type != TokenType::Error)
			{
				EnumValue value;
				value.Name = token.Lexeme;

				if (Match(TokenType::Equal))
				{
					Token number = Expect(TokenType::Number);
					value.Value = std::stoi(number.Lexeme);
					lastValue = value.Value;
				}
				else
				{
					value.Value = ++lastValue;
				}

				enumerator.Values.push_back(value);
				Match(TokenType::Comma);
			}
		} while (m_currentIter->Type != TokenType::EndOfFile && m_currentIter->Type != TokenType::RightBracket);
		Expect(TokenType::RightBracket);
		Expect(TokenType::SemiColon);

		m_enums.push_back(enumerator);
	}

	GProperty Parser::ParseProperty(std::vector<Decorator>& decorators)
	{
		std::vector<Token> allTokensBeforeSemiColon = std::vector<Token>();
		std::vector<Token>::iterator current;
		int64 afterIdentifierIndex = -1;

		do
		{
			current = m_tokens.begin();
			std::advance(current, m_currentToken);

			if (m_currentIter->Type == TokenType::SemiColon && afterIdentifierIndex == -1)
			{
				afterIdentifierIndex = allTokensBeforeSemiColon.size();
			}
			else if (m_currentIter->Type == TokenType::Equal)
			{
				afterIdentifierIndex = allTokensBeforeSemiColon.size();
			}
			else if (m_currentIter->Type == TokenType::Property || m_currentIter->Type == TokenType::Class
				|| m_currentIter->Type == TokenType::Struct || m_currentIter->Type == TokenType::Method)
			{
				std::wstringstream ss;
				
				ss << L"Excepted '" << TokenTypeToString(TokenType::SemiColon) << "' but instead got '"
					<< TokenTypeToString(m_currentIter->Type) << "' at line " << m_currentIter->Line << L" column " << m_currentIter->Column << L"\n";
				ss << L"File: " << m_path.generic_wstring() << L"\n";

				State::ThrowException(ss.str());

				return GProperty{ L"ERROR", GenerateErrorToken().Lexeme, decorators };
			}

			allTokensBeforeSemiColon.push_back(*current);
			m_currentToken++;
			m_currentIter++;
		} while (current->Type != TokenType::EndOfFile && current->Type != TokenType::SemiColon);

		Token variableIdentifier = GenerateErrorToken();
		if (afterIdentifierIndex != -1)
		{
			if (afterIdentifierIndex <= 1)
			{
				std::wstringstream ss;

				ss << L"Warning: Weird equal sign placement at line " << m_currentIter->Line << L" column " << m_currentIter->Column << L"\n";
				ss << L"File: " << m_path.generic_wstring() << L"\n";

				State::ThrowWarning(ss.str());
			}
			auto it = allTokensBeforeSemiColon.begin();
			std::advance(it, afterIdentifierIndex - 1);
			variableIdentifier = *it;
		}
		else
		{
			std::wstringstream ss;

			ss << L"Cannot find property identifier at line " << m_currentIter->Line << L" column " << m_currentIter->Column << L"\n";
			ss << L"File: " << m_path.generic_wstring() << L"\n";

			State::ThrowException(ss.str());

			return GProperty{ L"ERROR", variableIdentifier.Lexeme, decorators };
		}

		std::wstringstream typeIdentifier;
		auto endIter = allTokensBeforeSemiColon.begin();
		std::advance(endIter, afterIdentifierIndex - 1);
		for (auto iter = allTokensBeforeSemiColon.begin(); iter != endIter; iter++)
		{
			if (iter->Lexeme == variableIdentifier.Lexeme) break;

			typeIdentifier << iter->Lexeme;
		}

		return GProperty{ typeIdentifier.str(), variableIdentifier.Lexeme, decorators };
	}

	GMethod Parser::ParseMethod(std::vector<Decorator>& decorators)
	{
		std::vector<Token> allTokensBeforeLeftBraceOrSemiColon = std::vector<Token>();
		std::vector<Token>::iterator current;
		int64 afterIndex = -1;

		do
		{
			current = m_tokens.begin();
			std::advance(current, m_currentToken);

			if (m_currentIter->Type == TokenType::SemiColon && afterIndex == -1)
			{
				afterIndex = allTokensBeforeLeftBraceOrSemiColon.size();
			}
			else if (m_currentIter->Type == TokenType::Property || m_currentIter->Type == TokenType::Class
				|| m_currentIter->Type == TokenType::Struct || m_currentIter->Type == TokenType::Method)
			{
				std::wstringstream ss;

				ss << L"Excepted '" << TokenTypeToString(TokenType::SemiColon) << "' but instead got '"
					<< TokenTypeToString(m_currentIter->Type) << "' at line " << m_currentIter->Line << L" column " << m_currentIter->Column << L"\n";
				ss << L"File: " << m_path.generic_wstring() << L"\n";

				State::ThrowException(ss.str());

				return GMethod{ L"ERROR", GenerateErrorToken().Lexeme, {}, decorators };
			}

			allTokensBeforeLeftBraceOrSemiColon.push_back(*current);
			m_currentToken++;
			m_currentIter++;
		} while (current->Type != TokenType::EndOfFile && current->Type != TokenType::SemiColon && current->Type != TokenType::LeftBracket);

		std::vector<std::wstring> arguments;
		std::wstring returnType;

		Token identifier = GenerateErrorToken();
		if (afterIndex != -1)
		{
			if (afterIndex <= 1)
			{
				std::wstringstream ss;

				ss << L"Warning: Weird things going on at line " << m_currentIter->Line << L" column " << m_currentIter->Column << L"\n";
				ss << L"File: " << m_path.generic_wstring() << L"\n";

				State::ThrowWarning(ss.str());
			}

			auto it = allTokensBeforeLeftBraceOrSemiColon.end() - 1;

			int64 index = afterIndex - 1;

			do
			{
				it = allTokensBeforeLeftBraceOrSemiColon.begin() + index;

				if (it->Lexeme == L"noexcept" || it->Lexeme == L"const" || it->Lexeme == L"override")
				{
					index--;
					continue;
				}

				if (it->Type == TokenType::RightParen)
				{
					it = allTokensBeforeLeftBraceOrSemiColon.begin() + --index;
					
					do
					{
						it = allTokensBeforeLeftBraceOrSemiColon.begin() + index--;

						int64 argumentStartIndex = -1;
						std::wstring argument;

						while (it->Type != TokenType::Comma && it->Type != TokenType::LeftParen)
						{
							argumentStartIndex = index--;
							it = allTokensBeforeLeftBraceOrSemiColon.begin() + index;
						}

						if (argumentStartIndex == -1) break;
					} while (it->Type != TokenType::LeftParen && index > 0);
					it = allTokensBeforeLeftBraceOrSemiColon.begin() + --index;

					identifier = *it;

					int64 identifierIndex = index;
					index = 0;
					while (index < identifierIndex)
					{
						it = allTokensBeforeLeftBraceOrSemiColon.begin() + index++;
						returnType += it->Lexeme;
					}

					break;
				}
			} while (index > 0);

		}
		else
		{
			std::wstringstream ss;

			ss << L"Cannot find method identifier at line " << m_currentIter->Line << L" column " << m_currentIter->Column << L"\n";
			ss << L"File: " << m_path.generic_wstring() << L"\n";

			State::ThrowException(ss.str());

			return GMethod{ L"ERROR", identifier.Lexeme, {}, decorators };
		}

		return GMethod{ returnType, identifier.Lexeme, arguments, decorators};
	}

	const Token& Parser::Expect(TokenType type)
	{
		if (m_currentIter->Type != type)
		{
			std::wstringstream ss;

			ss << L"Excepted '" << TokenTypeToString(type) << L"' but instead got '" << TokenTypeToString(m_currentIter->Type) << L"' at line "
				<< m_currentIter->Line << L" column " << m_currentIter->Column << L"\n";
			ss << L"File: " << m_path.generic_wstring() << L"\n";
			State::ThrowException(ss.str());

			m_tokens.push_back(GenerateErrorToken());
			return m_tokens.back();
		}

		auto tokenToReturn = m_currentIter;
		m_currentToken++;
		m_currentIter++;
		return *tokenToReturn;
	}

	bool Parser::Match(TokenType type)
	{
		if (m_currentIter->Type == type)
		{
			m_currentToken++;
			m_currentIter++;
			return true;
		}

		return false;
	}

	std::vector<Decorator> Parser::ParseDecorators()
	{
		std::vector<Decorator> decorators;
		if (Match(TokenType::Identifier))
		{
			Token& token = *(m_currentIter - 1);
			m_currentToken--;
			Decorator decorator;
			decorator.Name = token.Lexeme;

			if (Match(TokenType::LeftParen))
			{
				std::vector<std::wstring> values;

				while (true)
				{
					token = *(m_currentIter); m_currentToken++;
					values.push_back(token.Lexeme);
					token = *(++m_currentIter); m_currentToken++;

					// Skip float
					if (token.Type == TokenType::Identifier && (token.Lexeme == L"f" || token.Lexeme == L"F"))
					{
						token = *(++m_currentIter);
						m_currentToken++;
					}

					if (Match(TokenType::RightParen)) break;

					Expect(TokenType::Comma);
				}

				decorator.Values = values;
				decorators.push_back(decorator);
			}

			while (Match(TokenType::Identifier))
			{
				Token& token = *(m_currentIter - 1);
				m_currentToken--;
				decorator.Name = token.Lexeme;
				decorator.Values = {};

				if (Match(TokenType::LeftParen))
				{
					std::vector<std::wstring> values;

					while (true)
					{
						token = *(m_currentIter); m_currentToken++;
						values.push_back(token.Lexeme);
						token = *(++m_currentIter); m_currentToken++;

						// Skip float
						if (token.Type == TokenType::Identifier && (token.Lexeme == L"f" || token.Lexeme == L"F"))
						{
							token = *(++m_currentIter);
							m_currentToken++;
						}

						if (Match(TokenType::RightParen)) break;

						Expect(TokenType::Comma);
					}

					decorator.Values = values;
					decorators.push_back(decorator);
				}
			}

			while (Match(TokenType::Comma))
			{
				Token& token = *(m_currentIter - 1);
				m_currentToken--;
				Decorator decorator;
				decorator.Name = token.Lexeme;

				if (Match(TokenType::LeftParen))
				{
					std::vector<std::wstring> values;

					while (true)
					{
						token = *(m_currentIter); m_currentToken++;
						values.push_back(token.Lexeme);
						token = *(++m_currentIter); m_currentToken++;

						// Skip float
						if (token.Type == TokenType::Identifier && (token.Lexeme == L"f" || token.Lexeme == L"F")) token = *(++m_currentIter);

						if (Match(TokenType::RightParen)) break;

						Expect(TokenType::Comma);
					}

					decorator.Values = values;
					decorators.push_back(decorator);
				}

				while (Match(TokenType::Identifier))
				{
					Token& token = *(m_currentIter - 1);
					m_currentToken--;
					decorator.Name = token.Lexeme;
					decorator.Values = {};

					if (Match(TokenType::LeftParen))
					{
						std::vector<std::wstring> values;

						while (true)
						{
							token = *(m_currentIter); m_currentToken++;
							values.push_back(token.Lexeme);
							token = *(++m_currentIter); m_currentToken++;

							// Skip float
							if (token.Type == TokenType::Identifier && (token.Lexeme == L"f" || token.Lexeme == L"F"))
							{
								token = *(++m_currentIter);
								m_currentToken++;
							}

							if (Match(TokenType::RightParen)) break;

							Expect(TokenType::Comma);
						}

						decorator.Values = values;
						decorators.push_back(decorator);
					}
				}
			}
		}

		return decorators;
	}

}