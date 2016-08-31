#include "AssertHelper.h"

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <iostream>
#include <fstream>
#include <ctime>

#include "FileSystem.h"

#include <Dbghelp.h>	// For Dump Files
#pragma comment(lib,"Dbghelp.lib")		//For MiniDumpWriteDump 

namespace Inferno
{
	// IN: string - message to display
	// OUT: void
	//
	// Creates a message box with the sent in string as well as writes the message to the debug log
	void Alert(const char* message)
	{
		Print(message);
		Print("\n");

		char title[128];
		GetWindowTextA(GetActiveWindow(), title, 128);
		MessageBoxA(GetActiveWindow(), message, title, MB_OK | MB_ICONEXCLAMATION);
	}
	// IN: wide string - message to display
	// OUT: void
	//
	// Creates a message box with the sent in string as well as writes the message to the debug log
	void Alert(const wchar_t* message)
	{
		Print(message);
		Print("\n");

		wchar_t title[128];
		GetWindowTextW(GetActiveWindow(), title, 128);
		MessageBoxW(GetActiveWindow(), message, title, MB_OK | MB_ICONEXCLAMATION);
	}

	// IN: string - message to display
	// OUT: bool - State of the expression
	//
	// Creates a message box with the sent in string as well as writes the message to the debug log
	// If in Debug, a break will be triggered
	bool Assert(bool expression, const char* message)
	{
		if (expression == false)
		{
			Alert(message);
#if defined(DEBUG) || defined(_DEBUG)
			DebugBreak();
#endif
		}

		return expression;
	}
	// IN: wide string - message to display
	// OUT: bool - State of the expression
	//
	// Creates a message box with the sent in string as well as writes the message to the debug log
	// If in Debug, a break will be triggered
	bool Assert(bool expression, const wchar_t* message)
	{
		if (expression == true)
		{
			Alert(message);
#if defined(DEBUG) || defined(_DEBUG)
			DebugBreak();
#endif
		}

		return expression;
	}

	// IN: string - message to print
	// OUT: void
	//
	// Prints the message to the debug log and will print to the debug console if in Debug
	void Print(const char* message)
	{
#if defined(DEBUG) || defined(_DEBUG)
		std::cout << message;
#endif
		debug.Print(message);
	}
	// IN: string - message to print
	// OUT: void
	//
	// Prints the message to the debug log and will print to the debug console if in Debug
	void Print(std::string message)
	{
#if defined(DEBUG) || defined(_DEBUG)
		std::cout << message;
#endif
		debug.Print(message);
	}
	// IN: wide string - message to print
	// OUT: void
	//
	// Prints the message to the debug log and will print to the debug console if in Debug
	void Print(const wchar_t* message)
	{
#if defined(DEBUG) || defined(_DEBUG)
		std::cout << message;
#endif
		debug.Print(Inferno::ToAsciiString(message));
	}
	// IN: wide string - message to print
	// OUT: void
	//
	// Prints the message to the debug log and will print to the debug console if in Debug
	void Print(std::wstring message)
	{
#if defined(DEBUG) || defined(_DEBUG)
		std::cout << message.c_str();
#endif
		debug.Print(Inferno::ToAsciiString(message));
	}

	std::ofstream out;

	LONG WINAPI WriteDumpFIle(_EXCEPTION_POINTERS *_exceptionInfo) {
		FileSystem fs;
		std::wstring dumpFile = fs.GetDataFolderW() + L"Dump Files\\";

		// Check to see if the dump folder exists
		if (!fs.DirectoryExistsW(dumpFile))
		{
			if (_wmkdir(dumpFile.c_str()) != 0)
			{
				ALERT("Unable to create the dump file directory! No dump file will be created!");
				return 0;
			}
		}

		time_t t = time(0);   // get time now
		struct tm now;
		localtime_s(&now, &t);

		dumpFile += std::to_wstring(now.tm_mday);
		dumpFile += L"_";
		dumpFile += std::to_wstring(now.tm_mon + 1);
		dumpFile += L"_";
		dumpFile += std::to_wstring(1900 + now.tm_year);
		dumpFile += L"_at_";
		dumpFile += std::to_wstring(now.tm_hour);
		dumpFile += L".";
		dumpFile += std::to_wstring(now.tm_min);
		dumpFile += L".";
		dumpFile += std::to_wstring(now.tm_sec);
		dumpFile += L".mdmp";

		Print("Dump File Made: "); Print(dumpFile.c_str());
		HANDLE hFile = ::CreateFile(dumpFile.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE != hFile) {
			_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

			ExInfo.ThreadId = ::GetCurrentThreadId();
			ExInfo.ExceptionPointers = _exceptionInfo;
			ExInfo.ClientPointers = FALSE;
			MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, nullptr);
			::CloseHandle(hFile);
		}

		return 0;
	}

	void InitDumpFile()
	{
		SetUnhandledExceptionFilter(WriteDumpFIle);
	}

	// IN: string - message to print
	// OUT: void
	//
	// Prints the passed in message to the debug log
	LogFile::LogFile() { std::ofstream out("LogFile.txt"); out.close(); }

	// IN: string - message to print
	// OUT: void
	//
	// Prints the passed in message to the debug log
	LogFile::~LogFile()
	{
	}

	void LogFile::Print(const std::string s)
	{
		out.open("LogFile.txt", std::ios_base::app);
		out << s.c_str() << std::endl;
		out.close();
	}

	void LogFile::Print(const char* c)
	{
		out.open("LogFile.txt", std::ios_base::app);
		out << c << std::endl;
		out.close();
	}

	std::ofstream& Open()
	{
		out.open("LogFile.txt", std::ios_base::app);
		return out;
	}
	void Close()
	{
		out.close();
	}

	std::ofstream& LogFile::operator<<(const char* c)
	{
		if (out.is_open())
			out << c;

		return out;
	}

	std::ofstream& LogFile::operator<<(const wchar_t* wc)
	{
		if (out.is_open())
			out << wc;

		return out;
	}

	std::ofstream& LogFile::operator<<(const std::string s)
	{
		if (out.is_open())
			out << s.c_str();

		return out;
	}

	std::ofstream& LogFile::operator<<(double d)
	{
		if (out.is_open())
			out << d;

		return out;
	}

	std::ofstream& LogFile::operator<<(float f)
	{
		if (out.is_open())
			out << f;

		return out;
	}

	std::ofstream& LogFile::operator<<(int i)
	{
		if (out.is_open())
			out << i;

		return out;
	}

	std::ofstream& LogFile::operator<<(bool b)
	{
		if (out.is_open())
		{
			if (b)
				out << "True";
			else
				out << "False";
		}

		return out;
	}
}