// Author: Jonathan Gyurkovics
//
// A assert based class that can print out messages to the console, debug log, or even create message prompts. The Asserst were referenced from the Full Sail SGD Wrappers.

#pragma once

#define ASSERT(expression, message) Inferno::Assert(expression, message)
#define ALERT(message)				Inferno::Alert(message)
#define PRINT(message)				Inferno::Print(message)

#include <sstream>

namespace Inferno
{
	// IN: string - message to display
	// OUT: void
	//
	// Creates a message box with the sent in string as well as writes the message to the debug log
	void Alert(const char* message);
	// IN: wide string - message to display
	// OUT: void
	//
	// Creates a message box with the sent in string as well as writes the message to the debug log
	void Alert(const wchar_t* message);

	// IN: string - message to display
	// OUT: bool - State of the expression
	//
	// Creates a message box with the sent in string as well as writes the message to the debug log
	// If in Debug, a break will be triggered
	bool Assert(bool expression, const char* message);
	// IN: wide string - message to display
	// OUT: bool - State of the expression
	//
	// Creates a message box with the sent in string as well as writes the message to the debug log
	// If in Debug, a break will be triggered
	bool Assert(bool expression, const wchar_t* message);

	// IN: string - message to print
	// OUT: void
	//
	// Prints the message to the debug log and will print to the debug console if in Debug
	void Print(const char* message);
	// IN: string - message to print
	// OUT: void
	//
	// Prints the message to the debug log and will print to the debug console if in Debug
	void Print(std::string message);
	// IN: wide string - message to print
	// OUT: void
	//
	// Prints the message to the debug log and will print to the debug console if in Debug
	void Print(const wchar_t* message);
	// IN: wide string - message to print
	// OUT: void
	//
	// Prints the message to the debug log and will print to the debug console if in Debug
	void Print(std::wstring message);
	

	void InitDumpFile();

	class LogFile
	{
	public:
		LogFile();
		~LogFile();

		// IN: string - message to print
		// OUT: void
		//
		// Prints the passed in message to the debug log
		void Print(const std::string s);
		// IN: string - message to print
		// OUT: void
		//
		// Prints the passed in message to the debug log
		void Print(const char* c);

		std::ofstream& Open();
		void Close();

		std::ofstream& operator<<(const char* c);
		std::ofstream& operator<<(const wchar_t* wc);
		std::ofstream& operator<<(const std::string s);
		std::ofstream& operator<<(const double d);
		std::ofstream& operator<<(const float f);
		std::ofstream& operator<<(const int i);
		std::ofstream& operator<<(const bool b);
	};
}

static Inferno::LogFile debug;