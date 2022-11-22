#pragma once

#include <string>
#include <iostream>

class State final
{
public:

	static void ThrowException(const std::wstring& what)
	{
		Get().m_what = Get().m_what.append(what).append(L"\n");
		Get().m_everythingOk = false;
	}

	static void ThrowWarning(const std::wstring& what)
	{
		std::wcout << what << "\n";
	}

	static bool IsEverythingOk() { return Get().m_everythingOk; }
	static std::wstring_view GetExceptionText() { return Get().m_what; }

private:

	bool m_everythingOk{ true };
	std::wstring m_what;

	static State& Get()
	{
		static State state;
		return state;
	}

};