#include "GlobalIncludes.h"

#include <locale>
#include <codecvt>
#include <algorithm>
#include <stdlib.h>

namespace Inferno
{
	// IN: wstring - string to convert
	// OUT: string - converted string
	//
	// Converts a wide string to ascii string
	std::string ToAsciiString(const std::wstring str)
	{
		return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(str);
	}
	// IN: wchar_t* - string to convert
	// OUT: string  - converted string
	//
	// Converts a wide string to ascii string
	std::string ToAsciiString(const wchar_t* str)
	{
		return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(str);
	}
	// IN: wstring - string to convert
	// OUT: char*  - converted string
	//
	// Converts a wide string to ascii string. Calls new so needs to be cleaned up
	const char* ToAscii(const std::wstring str)
	{
		std::string bStr = ToAsciiString(str);
		char* newStr = new char[bStr.length() + 1];
		strcpy_s(newStr, bStr.length() + 1, bStr.c_str());

		return newStr;
	}
	// IN: wchar_t* - string to convert
	// OUT: char*   - converted string
	//
	// Converts a wide string to ascii string. Calls new so needs to be cleaned up
	const char* ToAscii(const wchar_t* str)
	{
		std::string bStr = ToAsciiString(str);
		char* newStr = new char[bStr.length() + 1];
		strcpy_s(newStr, bStr.length() + 1, bStr.c_str());

		return newStr;
	}

	// IN: string   - string to convert
	// OUT: wstring - converted wide string
	//
	// Converts a string to wide string
	std::wstring ToWideString(const std::string str)
	{
		return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().from_bytes(str);
	}
	// IN: char*    - string to convert
	// OUT: wstring - converted wide string
	//
	// Converts a string to wide string
	std::wstring ToWideString(const char* str)
	{
		return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().from_bytes(str);
	}
	// IN: string    - string to convert
	// OUT: wchar_t* - converted wide string
	//
	// Converts a string to wide string. Calls new so needs to be cleaned up
	wchar_t* ToWide(const std::string str)
	{
		std::wstring bStr = ToWideString(str);
		wchar_t* newStr = _wcsdup(bStr.c_str());

		return newStr;
	}
	// IN: char*     - string to convert
	// OUT: wchar_t* - converted wide string
	//
	// Converts a string to wide string. Calls new so needs to be cleaned up
	wchar_t* ToWide(const char* str)
	{
		std::wstring bStr = ToWideString(str);
		wchar_t* newStr = _wcsdup(bStr.c_str());

		return newStr;
	}

	// IN: string - string to parse
	//     char   - delimeter to use
	// OUT: vector<string> - array of strings parsed
	//
	// Splits the incoming string into seperate strings for each delimiter found and returns the array
	std::vector<std::string> StringParse(std::string str, char delim)
	{
		std::vector<std::string> elem;
		StringParse(str, delim, elem);

		return elem;
	}
	// IN: string          - string to parse
	//     char            - delimeter to use
	//     vector<string>& - vector to store the strings in
	// OUT: vector<string> - array of strings parsed
	//
	// Splits the incoming string into seperate strings for each delimiter found and returns the array
	std::vector<std::string>& StringParse(std::string str, char delim, std::vector<std::string>& elem)
	{
		std::stringstream stream(str);
		std::string string;
		while (std::getline(stream, string, delim))
			elem.push_back(string);

		return elem;
	}

	// IN: string - string to convert
	// OUT: bool - true or false based on string
	//
	// Converts the string to a true or false value;
	bool to_bool(std::string str)
	{
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
		std::istringstream is(str);

		bool boolean;
		is >> std::boolalpha >> boolean;

		return boolean;
	}
	// IN: int - int to convert
	// OUT: bool - false if 0, true otherwise
	//
	// Turns an int into a bool
	bool to_bool(int val)
	{
		if (val == 0)
			return false;
		
		return true;
	}

	char* GetEnvVar(char* Path, size_t SizeInBytes, const char* VariableName)
	{
		_dupenv_s(&Path, &SizeInBytes, VariableName);

		return Path;
	}

	// IN: char* - Name of the environment variable
	// OUT: string - Contents of the environment variable
	//
	// Returns the contents of an environment variable if it exists
	std::string GetEnvironmentVariablePath(const char* VariableName)
	{
		char env_var[300] = "\0";
		std::string stringPath(GetEnvVar(env_var, 300, VariableName));

		return stringPath;
	}
}