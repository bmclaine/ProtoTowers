//*********************************************************************//
//	Author:			Brian McLaine
//	Description:	GlobalIncludes store commonly used includes and
//					helper functions.
//*********************************************************************//

#pragma once

#define SAFE_RELEASE(x) if(x) { x->Release(); x = nullptr; }
#define SAFE_DELETE(x) if(x) { delete x; x = nullptr; }
#define SAFE_ARRAY_DELETE(x) if(x) { delete[] x; x = nullptr; }

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <vector>
#include <string>

#include "AssertHelper.h"
#include "Flags.h"

namespace Inferno
{
	// IN: wstring - string to convert
	// OUT: string - converted string
	//
	// Converts a wide string to ascii string
	std::string ToAsciiString(const std::wstring str);
	// IN: wchar_t* - string to convert
	// OUT: string  - converted string
	//
	// Converts a wide string to ascii string
	std::string ToAsciiString(const wchar_t* str);
	// IN: wstring - string to convert
	// OUT: char*  - converted string
	//
	// Converts a wide string to ascii string. Calls new so needs to be cleaned up
	const char* ToAscii(const std::wstring str);
	// IN: wchar_t* - string to convert
	// OUT: char*   - converted string
	//
	// Converts a wide string to ascii string. Calls new so needs to be cleaned up
	const char* ToAscii(const wchar_t* str);

	// IN: string   - string to convert
	// OUT: wstring - converted wide string
	//
	// Converts a string to wide string
	std::wstring ToWideString(const std::string str);
	// IN: char*    - string to convert
	// OUT: wstring - converted wide string
	//
	// Converts a string to wide string
	std::wstring ToWideString(const char* str);
	// IN: string    - string to convert
	// OUT: wchar_t* - converted wide string
	//
	// Converts a string to wide string. Calls new so needs to be cleaned up
	wchar_t* ToWide(const std::string str);
	// IN: char*     - string to convert
	// OUT: wchar_t* - converted wide string
	//
	// Converts a string to wide string. Calls new so needs to be cleaned up
	wchar_t* ToWide(const char* str);

	// IN: string - string to parse
	//     char   - delimeter to use
	// OUT: vector<string> - array of strings parsed
	//
	// Splits the incoming string into seperate strings for each delimiter found and returns the array
	std::vector<std::string> StringParse(std::string str, char delim);
	// IN: string          - string to parse
	//     char            - delimeter to use
	//     vector<string>& - vector to store the strings in
	// OUT: vector<string> - array of strings parsed
	//
	// Splits the incoming string into seperate strings for each delimiter found and returns the array
	std::vector<std::string>& StringParse(std::string str, char delim, std::vector<std::string>& elem);

	// IN: string - string to convert
	// OUT: bool - true or false based on string
	//
	// Converts the string to a true or false value;
	bool to_bool(std::string str);
	// IN: int - int to convert
	// OUT: bool - false if 0, true otherwise
	//
	// Turns an int into a bool
	bool to_bool(int val);

	// IN: char* - Name of the environment variable
	// OUT: string - Contents of the environment variable
	//
	// Returns the contents of an environment variable if it exists
	std::string GetEnvironmentVariablePath(const char* VariableName);

	template<typename Type, typename Less>
	void Merge(Type* arr, Type* temp, int low, int high, int mid, Less op)
	{
		int i, j, k;
		i = low; k = low; j = mid + 1;

		while (i <= mid && j <= high)
		{
			if (op(arr[i], arr[j]))
			{
				temp[k] = arr[i];
				k++; i++;
			}
			else
			{
				temp[k] = arr[j];
				k++; j++;
			}
		}

		while (i <= mid)
		{
			temp[k] = arr[i];
			k++; i++;
		}

		while (j <= high)
		{
			temp[k] = arr[j];
			k++; j++;
		}

		for (i = low; i < k; i++)
			arr[i] = temp[i];
	}

	template<typename Type, typename Less>
	void MSort(Type* arr, Type* temp, UINT low, UINT high, Less op)
	{
		if (low < high)
		{
			int mid = (low + high) >> 1;
			MSort(arr, temp, low, mid, op);
			MSort(arr, temp, mid + 1, high, op);
			Merge(arr, temp, low, high, mid, op);
		}
	}

	// IN: Type* - Array of type to sort
	//     int   - first element to sort by
	//     int   - last element to sort by
	// OUT: void
	//
	// Performs a merge sort on the passed in data
	template<typename Type, typename Less>
	void MergeSort(Type* arr, UINT low, UINT high, Less op)
	{
		if (arr == nullptr)
		{
			ALERT("Invalid array for sorting!");

			return;
		}

		if (high <= low)
			return;

		Type* temp = new Type[high - low];
		MSort(arr, temp, low, high, op);
		delete[] temp;
	}

	template<typename Type, typename Less>
	void MergeSort(std::vector<Type>& arr, Less op)
	{
		if (arr.size() > 0)
		{
			Type* temp = new Type[arr.size()];
			MSort(&arr[0], temp, 0, (UINT)arr.size() - 1, op);
			delete[] temp;
		}
	}
}