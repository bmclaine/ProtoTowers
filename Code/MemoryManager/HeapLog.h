///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Each heap in MemoryManager will contain a HeapLog.
//              If the program terminates with leaked memory,
//              it will output said logs into a file.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once


// STL Includes
#include <Windows.h>
#include <vector>
#include <concrt.h>


struct LogInfo
{
	char file[512];
	int line;
	int size;
	LogInfo& operator = (const LogInfo& _assign)
	{
		memcpy(file, _assign.file, strlen(_assign.file) + 1);
		line = _assign.line;
		size = _assign.size;
		return *this;
	}
};


class HeapLog
{
private:
	CRITICAL_SECTION     m_criticalSection;// -> The critical section to protect against multithreaded-nonesense
	std::vector<std::pair<void*, LogInfo>> m_loggedInfo;     // -> A container of logged info for a specific heap


	// IN: _path - The file path to check
	//
	// OUT: bool - If the file exists or not
	//
	// Description: Returns true if the file exists
	bool FileExists(std::string& _path) const;


public:
	// Defaults
	HeapLog ( void );
	~HeapLog( void );




	// IN: _block - The data to log
	//     _info - The info of that data
	//
	// OUT: VOID
	//
	// Description: Makes a pair of the data and the info, then keeps track of it
	void Add( void* _data, LogInfo& _info );




	// IN: _data - The block of data paired with its data
	//
	// OUT: VOID
	//
	// Description: Removes logged info related to the block of data
	void Remove( void* _data );




	// IN: _heapName - The name of the heap
	//
	// OUT: VOID
	//
	// Description: Dumps all the logged info for the specified heap to a file
	void WriteLogFile( const char* _heapName );
};


