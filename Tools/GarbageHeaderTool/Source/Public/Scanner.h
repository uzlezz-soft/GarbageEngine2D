#pragma once

#include "Base.h"

#include <string>
#include <vector>
#include <map>

namespace GarbageHeaderTool
{

	enum class TokenType
	{
		Property, Method, Class, Struct, Enum,
		StructKeyword, ClassKeyword, EnumKeyword, Colon, Comma,

		LeftParen, RightParen, Hashtag, LeftBracket, RightBracket,
		SemiColon, Equal, PlusEqual, MinusEqual, TimesEqual, DivideEqual,
		ModuleEqual, CaretEqual, AmpersandEqual, BarEqual, RightShiftEqual,
		LeftShiftEqual, EqualEqual, BangEqual, LessThanEqual, GreaterThanEqual,

		ConstKeyword, Identifier, AutoKeyword,

		Star, Reference, LeftAngleBracket, RightAngleBracket,

		StringLiteral, Number, TrueKeyword, FalseKeyword, 
		GeneratedBodyMacro,

		EndOfFile,
		Error
	};

	struct Token
	{
		int64 Line;
		int64 Column;
		TokenType Type;
		std::wstring Lexeme;

		bool operator==(const Token& other) const
		{
			return Line == other.Line && Column == other.Column && Type == other.Type;
		}
	};

	class Scanner
	{
	public:

		Scanner(const std::wstring& path, const std::wstring& stem);

		std::vector<Token> ScanTokens();

		bool HasAnyReflectionAttributes() const { return m_hasAnyReflectionAttributes; }
		bool IsIncludesSelfGeneratedVersion() const { return m_includesSelfGeneratedVersion; }

	private:

		const std::map<std::wstring, TokenType> keywords =
		{
			{ L"GPROPERTY",      TokenType::Property },
			{ L"GCLASS",         TokenType::Class },
			{ L"GSTRUCT",        TokenType::Struct },
			{ L"GMETHOD",        TokenType::Method },
			{ L"GENUM",          TokenType::Enum },
			{ L"auto",           TokenType::AutoKeyword },
			{ L"const",          TokenType::ConstKeyword },
			{ L"class",          TokenType::ClassKeyword },
			{ L"struct",         TokenType::StructKeyword },
			{ L"enum",           TokenType::EnumKeyword },
			{ L"GENERATED_BODY", TokenType::GeneratedBodyMacro }
		};

		bool m_hasAnyReflectionAttributes{ false };
		bool m_includesSelfGeneratedVersion{ false };

		std::wstring m_fileContents;
		std::wstring m_path;

		int64 m_cursor = 0;
		int64 m_line = 1;
		int64 m_column = 0;
		int64 m_start = 0;

		Token ScanToken();
		Token PropertyIdentifier();
		Token Number();
		Token String();

		wchar_t Advance();
		wchar_t Peek();
		wchar_t PeekNext();
		wchar_t PeekNextNext();
		bool Match(wchar_t expected);

		inline bool IsDigit(wchar_t c) const { return c >= L'0' && c <= L'9'; }
		inline bool IsAlphaNumeric(wchar_t c) const { return IsAlpha(c) || IsDigit(c); }
		inline bool IsAlpha(wchar_t c) const {
			return (c >= L'a' && c <= L'z') || (c >= L'A' && c <= L'Z')
				|| (c >= L'à' && c <= L'ß') || (c >= L'À' && c <= L'ß');
		}
		inline bool AtEnd() const { return m_cursor == m_fileContents.size(); }
		inline Token GenerateToken(TokenType type, const std::wstring& lexeme) { return Token{ m_line, m_column, type, lexeme }; }
		inline Token GenerateErrorToken() { return Token{ -1, -1, TokenType::Error, L"" }; }



	};

}