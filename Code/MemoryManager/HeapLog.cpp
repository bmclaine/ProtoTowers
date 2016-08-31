///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Each heap in MemoryManager will contain a HeapLog.
//              If the program terminates with leaked memory,
//              it will output said logs into a file.
//
///////////////////////////////////////////////////////////////////////////////////////////////////


// STL Includes
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <sys/stat.h>
using namespace std;


// Engine Includes
#include "HeapLog.h"


//////////////////////////////////////////
//           THE FILE LOCATION          //
//////////////////////////////////////////
#define FILE_OUTPUT_LOCATION	"LeakedMemory/Leaked Memory.txt"


HeapLog::HeapLog()
{
	InitializeCriticalSection(&m_criticalSection);
}



HeapLog::~HeapLog()
{
	m_loggedInfo.clear();
	DeleteCriticalSection(&m_criticalSection);
}




// IN: _block - The data to log
//     _info - The info of that data
//
// OUT: VOID
//
// Description: Makes a pair of the data and the info, then keeps track of it
void HeapLog::Add(void* _data, LogInfo& _info)
{
	// ASSERT(_block != nullptr, "Tried to add nullptr to HeapLog");
	EnterCriticalSection(&m_criticalSection);

	pair<void*, LogInfo> temp;
	temp.first  = _data;
	temp.second = _info;

	m_loggedInfo.push_back(temp);

	LeaveCriticalSection(&m_criticalSection);
}




// IN: _data - The block of data paired with its data
//
// OUT: VOID
//
// Description: Removes logged info related to the block of data
void HeapLog::Remove(void* _data)
{
	EnterCriticalSection(&m_criticalSection);
	
	for (unsigned int i = 0; i < m_loggedInfo.size(); ++i)
	{
		if (m_loggedInfo[i].first == _data)
		{
			m_loggedInfo.erase(m_loggedInfo.begin() + i);
			break;
		}
	}

	LeaveCriticalSection(&m_criticalSection);
}




// IN: _heapName - The name of the heap
//
// OUT: VOID
//
// Description: Dumps all the logged info for the specified heap to a file
void HeapLog::WriteLogFile(const char* _heapName)
{

	if (m_loggedInfo.size() == 0)
		return;

	string fileName(FILE_OUTPUT_LOCATION);
	fileName.insert(fileName.length() - 4, " - ");
	fileName.insert(fileName.length() - 4, _heapName);

	if (FileExists(fileName))
		fileName.insert(fileName.length() - 4, "(0)");
	int i = 0;
	while (FileExists(fileName))
	{
		++i;
		fileName.erase(fileName.end() - 7, fileName.end() - 4);
		fileName.insert(fileName.length() - 4, "()");
		string number = to_string(i);
		fileName.insert(fileName.length() - 5, number.c_str());
	}

	tr2::sys::path leakPath = tr2::sys::path(FILE_OUTPUT_LOCATION);
	if (tr2::sys::exists(leakPath))
		tr2::sys::remove(leakPath);

	if (leakPath.has_parent_path() && !tr2::sys::exists(leakPath.parent_path()))
		tr2::sys::create_directories(leakPath.parent_path());

		
	fstream fout(fileName.c_str(), ios_base::out);
	
	if (fout.is_open())
	{
		fout << "******************************************************\n";
		for (unsigned int i = 0; i < m_loggedInfo.size(); ++i)
		{
			fout << "File: " << m_loggedInfo[i].second.file << "\n";
			fout << "Line: " << m_loggedInfo[i].second.line << "\n";
			fout << "Size: " << m_loggedInfo[i].second.size << " bytes\n";
			fout << "******************************************************\n";
		}
		fout.close();
		fout.clear();
	}
	else // fout.is_open() == false
	{
		fout.close();
		fout.clear();
	}
}




// IN: _path - The file path to check
//
// OUT: bool - If the file exists or not
//
// Description: Returns true if the file exists
bool HeapLog::FileExists(std::string& _path) const
{
	struct stat buffer;
	return (stat(_path.c_str(), &buffer) == 0);
}




