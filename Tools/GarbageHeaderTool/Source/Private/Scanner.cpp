#include "Scanner.h"
#include <fstream>
#include <iostream>

namespace GarbageHeaderTool
{

	namespace Utils
	{
		static std::wstring GetFileContents(const std::wstring& path)
		{
			std::wifstream in(path);
			std::wstring content((std::istreambuf_iterator<wchar_t>(in)), (std::istreambuf_iterator<wchar_t>()));

			return content;
		}
	}

	Scanner::Scanner(const std::wstring& path, const std::wstring& stem) : m_path(path)
	{
		m_fileContents = Utils::GetFileContents(path);

		m_includesSelfGeneratedVersion = m_fileContents.find(stem) != std::wstring::npos;
	}

	std::vector<Token> Scanner::ScanTokens()
	{
		auto tokens = std::vector<Token>();

		m_cursor = 0;
		while (!AtEnd())
		{
			m_start = m_cursor;
			Token token = ScanToken();

			if (token.Type != TokenType::Error) tokens.push_back(token);
		}

		tokens.emplace_back(Token{ m_line, m_column, TokenType::EndOfFile, L"EOF" });
		return tokens;
	}

	Token Scanner::ScanToken()
	{
		wchar_t c = Advance();

		switch (c)
		{
			// Single character tokens
		case L'<': return GenerateToken(TokenType::LeftAngleBracket, L"<");
		case L'>': return GenerateToken(TokenType::RightAngleBracket, L">");
		case L'*': return GenerateToken(TokenType::Star, L"*");
		case L'&': return GenerateToken(TokenType::Reference, L"&");
		case L'(': return GenerateToken(TokenType::LeftParen, L"(");
		case L')': return GenerateToken(TokenType::RightParen, L")");
		case L'#': return GenerateToken(TokenType::Hashtag, L"#");
		case L'{': return GenerateToken(TokenType::LeftBracket, L"{");
		case L'}': return GenerateToken(TokenType::RightBracket, L"}");
		case L';': return GenerateToken(TokenType::SemiColon, L";");
		case L'=': return GenerateToken(TokenType::Equal, L"=");
		case L':': return GenerateToken(TokenType::Colon, L":");
		case L',': return GenerateToken(TokenType::Comma, L",");
		case L'"': return String();
			// Whitespace
		case L'/':
		{
			if (Match(L'/'))
			{
				while (Peek() != L'\n' && !AtEnd()) Advance();

				return GenerateErrorToken();
			}
			else if (Match(L'*'))
			{
				while (!AtEnd() && Peek() != L'*' && PeekNext() != L'/')
				{
					c = Advance();
					if (c == L'\n')
					{
						m_column = 0;
						m_line++;
					}
				}

				// Consume */
				if (!AtEnd()) Match(L'*');
				if (!AtEnd()) Match(L'/');
				return GenerateErrorToken();
			}
			break;
		}
		case L' ':
		case L'\r':
		case L'\t':
			// Ignore whitespace
			return GenerateErrorToken();
		case L'\n':
			m_column = 0;
			m_line++;
			return GenerateErrorToken();
		default:
			if (IsAlpha(c))
			{
				return PropertyIdentifier();
			}
			else if (IsDigit(c))
			{
				return Number();
			}
			break;
		}

		return GenerateErrorToken();
	}

	Token Scanner::PropertyIdentifier()
	{
		while (IsAlphaNumeric(Peek()) || Peek() == L'_') Advance();

		std::wstring text = m_fileContents.substr(m_start, m_cursor - m_start);
		TokenType type = TokenType::Identifier;
		auto it = keywords.find(text);
		if (it != keywords.end())
		{
			type = it->second;
			
			if (type == TokenType::Property || type == TokenType::Method 
				|| type == TokenType::Class || type == TokenType::Struct || type == TokenType::Enum) m_hasAnyReflectionAttributes = true;
		}

		return Token{ m_line, m_column, type, text };
	}

	Token Scanner::Number()
	{
		while (IsDigit(Peek())) Advance();

		bool hasE = false;
		if (Peek() == L'.' && (IsDigit(PeekNext()) || (PeekNext() == L'e' && IsDigit(PeekNextNext()))
			|| (PeekNext() == L'E' && IsDigit(PeekNextNext()))))
		{
			Advance();

			while (IsDigit(Peek()))
			{
				Advance();
			}

			if ((Peek() == L'e' || Peek() == L'E') && (IsDigit(PeekNext()) ||
				((PeekNext() == L'-' && IsDigit(PeekNextNext())) || (PeekNext() == L'+' && IsDigit(PeekNextNext())))))
			{
				Advance();
				while (IsDigit(Peek())) Advance();

				if ((Peek() == L'-' || Peek() == L'+') && IsDigit(PeekNext()))
				{
					Advance();
					while (IsDigit(Peek())) Advance();
				}

				if (Peek() == '.')
				{
					std::wcerr << L"Unexpected number literal at line " << m_line << L" col " << m_column << std::endl;
					return GenerateErrorToken();
				}
			}
		}

		if ((Peek() == L'e' || Peek() == L'E') && (IsDigit(PeekNext()) ||
			((PeekNext() == L'-' && IsDigit(PeekNextNext())) || (PeekNext() == L'+' && IsDigit(PeekNextNext())))))
		{
			Advance();
			while (IsDigit(Peek())) Advance();

			if ((Peek() == L'-' || Peek() == L'+') && IsDigit(PeekNext()))
			{
				Advance();
				while (IsDigit(Peek())) Advance();
			}

			if (Peek() == L'.')
			{
				std::wcerr << L"Unexpected number literal at line " << m_line << L" col " << m_column << std::endl;
				return GenerateErrorToken();
			}
		}

		return Token{ m_line, m_column, TokenType::Number, m_fileContents.substr(m_start, m_cursor - m_start) };
	}

	Token Scanner::String()
	{
		while (Peek() != L'"' && !AtEnd())
		{
			if (Peek() == L'\n')
			{
				m_line++;
				m_column = -1;
			}
			Advance();
		}

		if (AtEnd())
		{
			std::wcerr << L"Unexpected string literal at line " << m_line << L" col " << m_column << std::endl;
			return GenerateErrorToken();
		}

		Advance();

		std::wstring value = m_fileContents.substr(m_start, m_cursor - m_start);
		return Token{ m_column, m_line, TokenType::StringLiteral, value };
	}

	wchar_t Scanner::Advance()
	{
		wchar_t c = m_fileContents[m_cursor++];
		m_column++;
		return c;
	}

	wchar_t Scanner::Peek()
	{
		if (AtEnd()) return L'\0';

		return m_fileContents[m_cursor];
	}

	wchar_t Scanner::PeekNext()
	{
		if (AtEnd() || m_cursor == m_fileContents.size() - 1) return L'\0';

		return m_fileContents[m_cursor + 1];
	}

	wchar_t Scanner::PeekNextNext()
	{
		if (AtEnd() || m_cursor == m_fileContents.size() - 1 || m_cursor == m_fileContents.size() - 2) return '\0';

		return m_fileContents[m_cursor + 1];
	}

	bool Scanner::Match(wchar_t expected)
	{
		if (AtEnd() || m_fileContents[m_cursor] != expected) return false;

		m_cursor++;
		m_column++;
		return true;
	}

}