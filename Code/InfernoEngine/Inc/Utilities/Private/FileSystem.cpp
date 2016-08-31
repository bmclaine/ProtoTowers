//*********************************************************************//
//	Author:			Brian McLaine
//	Description:	FileSystem is used to strore and retrieve commonly
//					used folder paths.
//*********************************************************************//

#include "FileSystem.h"
#include "GlobalIncludes.h"

//std::wstring FileSystem::s_ResourcesFolder = L"Resources/";
//std::wstring FileSystem::s_AudioFolder = L"Resources/Audio/";
//std::wstring FileSystem::s_AnimationsFolder = L"Resources/Animations/";
//std::wstring FileSystem::s_AnimationControllersFolder = L"Resources/AnimationControllers/";
//std::wstring FileSystem::s_AnimationSetsFolder = L"Resources/AnimationSets/";
//std::wstring FileSystem::s_GameObjectsFolder = L"Resources/GameObjects/";
//std::wstring FileSystem::s_IconsFolder = L"Resources/Icons/";
//std::wstring FileSystem::s_MaterialsFolder = L"Resources/Materials/";
//std::wstring FileSystem::s_MeshesFolder = L"Resources/Meshes/";
//std::wstring FileSystem::s_ParticlesFolder = L"Resources/Particles/";
//std::wstring FileSystem::s_ScenesFolder = L"Resources/Scenes/";
//std::wstring FileSystem::s_TexturesFolder = L"Resources/Textures/";
//std::wstring FileSystem::s_PrefabFolder = L"Resources/Prefabs/";
//std::wstring FileSystem::s_EngineShaderFolder = L"../InfernoEngine/Inc/Core/Private/Shaders/Compiled/";
//std::wstring FileSystem::s_DataFolder = Inferno::ToWideString(Inferno::GetEnvironmentVariablePath("USERPROFILE")) + L"\\Documents\\My Games\\ProtoTowers\\";

FileSystem::FileSystem()
{
#if !defined(SHIPPING)
	s_ResourcesFolder = L"Resources/";
	s_AudioFolder = L"Resources/Audio/";
	s_AnimationsFolder = L"Resources/Animations/";
	s_AnimationControllersFolder = L"Resources/AnimationControllers/";
	s_AnimationSetsFolder = L"Resources/AnimationSets/";
	s_GameObjectsFolder = L"Resources/GameObjects/";
	s_IconsFolder = L"Resources/Icons/";
	s_MaterialsFolder = L"Resources/Materials/";
	s_MeshesFolder = L"Resources/Meshes/";
	s_ParticlesFolder = L"Resources/Particles/";
	s_ScenesFolder = L"Resources/Scenes/";
	s_TexturesFolder = L"Resources/Textures/";
	s_PrefabFolder = L"Resources/Prefabs/";
	s_EngineShaderFolder = L"../InfernoEngine/Inc/Core/Private/Shaders/Compiled/";
	s_DataFolder = Inferno::ToWideString(Inferno::GetEnvironmentVariablePath("USERPROFILE")) + L"\\Documents\\My Games\\ProtoTowers\\";
#else
	s_ResourcesFolder = L"Data/";
	s_AudioFolder = L"Data/Audio/";
	s_AnimationsFolder = L"Data/Animations/";
	s_AnimationControllersFolder = L"Data/AnimationControllers/";
	s_AnimationSetsFolder = L"Data/AnimationSets/";
	s_GameObjectsFolder = L"Data/GameObjects/";
	s_IconsFolder = L"Data/Icons/";
	s_MaterialsFolder = L"Data/Materials/";
	s_MeshesFolder = L"Data/Meshes/";
	s_ParticlesFolder = L"Data/Particles/";
	s_ScenesFolder = L"Data/Scenes/";
	s_TexturesFolder = L"Data/Textures/";
	s_PrefabFolder = L"Data/Prefabs/";
	s_EngineShaderFolder = L"Data/Shaders/";
	s_DataFolder = Inferno::ToWideString(Inferno::GetEnvironmentVariablePath("USERPROFILE")) + L"\\Documents\\My Games\\ProtoTowers\\";
#endif
}

FileSystem::~FileSystem()
{

}

std::string	FileSystem::GetResourcesFolder()
{
	return Inferno::ToAsciiString(s_ResourcesFolder);
}

std::wstring FileSystem::GetResourcesFolderW()
{
	return s_ResourcesFolder;
}


std::string	FileSystem::GetAnimationsFolder()
{
	return Inferno::ToAsciiString(s_AnimationsFolder);
}

std::wstring FileSystem::GetAnimationsFolderW()
{
	return s_AnimationsFolder;
}

std::string	FileSystem::GetAnimationControllersFolder()
{
	return Inferno::ToAsciiString(s_AnimationControllersFolder);
}

std::wstring FileSystem::GetAnimationControllersFolderW()
{
	return s_AnimationControllersFolder;
}

std::string	FileSystem::GetAnimationSetsFolder()
{
	return Inferno::ToAsciiString(s_AnimationSetsFolder);
}

std::wstring FileSystem::GetAnimationSetsFolderW()
{
	return s_AnimationSetsFolder;
}

std::string	FileSystem::GetAudioFolder()
{
	return Inferno::ToAsciiString(s_AudioFolder);
}

std::wstring FileSystem::GetAudioFolderW()
{
	return s_AudioFolder;
}

std::string	FileSystem::GetGameObjectsFolder()
{
	return Inferno::ToAsciiString(s_GameObjectsFolder);
}

std::wstring FileSystem::GetGameObjectsFolderW()
{
	return s_GameObjectsFolder;
}

std::string	FileSystem::GetIconsFolder()
{
	return Inferno::ToAsciiString(s_IconsFolder);
}

std::wstring FileSystem::GetIconsFolderW()
{
	return s_IconsFolder;
}

std::string	FileSystem::GetMaterialsFolder()
{
	return Inferno::ToAsciiString(s_MaterialsFolder);
}

std::wstring FileSystem::GetMaterialsFolderW()
{
	return s_MaterialsFolder;
}

std::string	FileSystem::GetMeshesFolder()
{
	return Inferno::ToAsciiString(s_MeshesFolder);
}

std::wstring FileSystem::GetMeshesFolderW()
{
	return s_MeshesFolder;
}

std::string	FileSystem::GetParticlesFolder()
{
	return Inferno::ToAsciiString(s_ParticlesFolder);
}

std::wstring FileSystem::GetParticlesFolderW()
{
	return s_ParticlesFolder;
}

std::string	FileSystem::GetScenesFolder()
{
	return Inferno::ToAsciiString(s_ScenesFolder);
}

std::wstring FileSystem::GetScenesFolderW()
{
	return s_ScenesFolder;
}

std::string	FileSystem::GetTexturesFolder()
{
	return Inferno::ToAsciiString(s_TexturesFolder);
}

std::wstring FileSystem::GetTexturesFolderW()
{
	return s_TexturesFolder;
}

//--------------------------------------------------------------------------------

std::string FileSystem::GetShaderFolder()
{
	return Inferno::ToAsciiString(s_EngineShaderFolder);
}

std::wstring FileSystem::GetShaderFolderW()
{
	return s_EngineShaderFolder;
}

std::string FileSystem::GetPrefabsFolder(){
	return Inferno::ToAsciiString(s_PrefabFolder);
}

std::wstring FileSystem::GetPrefabsFolderW(){
	return s_PrefabFolder;
}

std::string FileSystem::GetDataFolder(){
	return Inferno::ToAsciiString(s_DataFolder);
}
std::wstring FileSystem::GetDataFolderW(){
	return s_DataFolder;
}

//--------------------------------------------------------------------------------

void FileSystem::SetResourcesFolder(const std::string& folder)
{
	s_ResourcesFolder = Inferno::ToWideString(folder);
}

void FileSystem::SetResourcesFolderW(const std::wstring& folder)
{
	s_ResourcesFolder = folder;
}

void FileSystem::SetAnimationsFolder(const std::string& folder)
{
	s_AnimationsFolder = Inferno::ToWideString(folder);
}

void FileSystem::SetAnimationsFolderW(const std::wstring& folder)
{
	s_AnimationsFolder = folder;
}

void FileSystem::SetAnimationControllersFolder(const std::string& folder)
{
	s_AnimationControllersFolder = Inferno::ToWideString(folder);
}

void FileSystem::SetAnimationControllersFolderW(const std::wstring& folder)
{
	s_AnimationControllersFolder = folder;
}

void FileSystem::SetAnimationSetsFolder(const std::string& folder)
{
	s_AnimationSetsFolder = Inferno::ToWideString(folder);
}

void FileSystem::SetAnimationSetsFolderW(const std::wstring& folder)
{
	s_AnimationSetsFolder = folder;
}

void FileSystem::SetAudioFolder(const std::string& folder)
{
	s_AudioFolder = Inferno::ToWideString(folder);
}

void FileSystem::SetAudioFolderW(const std::wstring& folder)
{
	s_AudioFolder = folder;
}

void FileSystem::SetGameObjectsFolder(const std::string& folder)
{
	s_GameObjectsFolder = Inferno::ToWideString(folder);
}

void FileSystem::SetGameObjectsFolderW(const std::wstring& folder)
{
	s_GameObjectsFolder = folder;
}

void FileSystem::SetIconsFolder(const std::string& folder)
{
	s_IconsFolder = Inferno::ToWideString(folder);
}

void FileSystem::SetIconsFolderW(const std::wstring& folder)
{
	s_IconsFolder = folder;
}

void FileSystem::SetMaterialsFolder(const std::string& folder)
{
	s_MaterialsFolder = Inferno::ToWideString(folder);
}

void FileSystem::SetMaterialsFolderW(const std::wstring& folder)
{
	s_MaterialsFolder = folder;
}

void FileSystem::SetMeshesFolder(const std::string& folder)
{
	s_MeshesFolder = Inferno::ToWideString(folder);
}

void FileSystem::SetMeshesFolderW(const std::wstring& folder)
{
	s_MeshesFolder = folder;
}

void FileSystem::SetParticlesFolder(const std::string& folder)
{
	s_ParticlesFolder = Inferno::ToWideString(folder);
}

void FileSystem::SetParticlesFolderW(const std::wstring& folder)
{
	s_ParticlesFolder = folder;
}

void FileSystem::SetScenesFolder(const std::string& folder)
{
	s_ScenesFolder = Inferno::ToWideString(folder);
}

void FileSystem::SetScenesFolderW(const std::wstring& folder)
{
	s_ScenesFolder = folder;
}

void FileSystem::SetTexturesFolder(const std::string& folder)
{
	s_TexturesFolder = Inferno::ToWideString(folder);
}

void FileSystem::SetTexturesFolderW(const std::wstring& folder)
{
	s_TexturesFolder = folder;
}

void FileSystem::SetShaderFolder(const std::string& _folder)
{
	SetShaderFolderW(Inferno::ToWideString(_folder));
}
void FileSystem::SetShaderFolderW(const std::wstring& _folder)
{
	s_EngineShaderFolder = _folder;
}

void FileSystem::SetPrefabsFolder(const std::string& _folder){
	s_PrefabFolder = Inferno::ToWideString(_folder);
}

void FileSystem::SetPrefabsFolderW(const std::wstring& _folder){
	s_PrefabFolder = _folder;
}

void FileSystem::SetDataFolder(const std::string& _folder)
{
	s_DataFolder = Inferno::ToWideString(_folder);
}
void FileSystem::SetDataFolderW(const std::wstring& _folder)
{
	s_DataFolder = _folder;
}

//*********************************************************************//
//	In:				_file - a path to the file being checked
//	Out:			true - if the file exists
//					false - if the file does not exist
//
//	Description:	Checks for the specified file and returns a bool
//					based on whether the file is found
//*********************************************************************//
bool FileSystem::FileExists(const std::string& file)
{
	return FileExistsW(Inferno::ToWideString(file));
}

//*********************************************************************//
//	In:				_file - a path to the file being checked
//	Out:			true - if the file exists
//					false - if the file does not exist
//
//	Description:	Checks for the specified file and returns a bool
//					based on whether the file is found
//*********************************************************************//
bool FileSystem::FileExistsW(const std::wstring& file)
{
	DWORD dwAttrib = GetFileAttributes(file.c_str());
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

//*********************************************************************//
//	In:				_file1 - a path to the file being checked
//					_file2 - a path to the file being checked against
//	Out:			true - if the file1 is newer than file2
//					false - if file1 is not new than file2
//
//	Description:	Checks for the specified file and returns a bool
//					based on whether the file is found
//*********************************************************************//
bool FileSystem::FileIsNewer(const std::string& file1, const std::string& file2)
{
		return FileIsNewerW(Inferno::ToWideString(file1), Inferno::ToWideString(file2));
}

//*********************************************************************//
//	In:				_file1 - a path to the file being checked
//					_file2 - a path to the file being checked against
//	Out:			true - if the file1 is newer than file2
//					false - if file1 is not new than file2
//
//	Description:	Checks for the specified file and returns a bool
//					based on whether the file is found
//*********************************************************************//
bool FileSystem::FileIsNewerW(const std::wstring& file1, const std::wstring& file2)
{
	HANDLE handle1 = INVALID_HANDLE_VALUE;
	HANDLE handle2 = INVALID_HANDLE_VALUE;

	handle1 = CreateFileW(file1.c_str(), GENERIC_READ, FILE_SHARE_READ,
		nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	handle2 = CreateFileW(file2.c_str(), GENERIC_READ, FILE_SHARE_READ,
		nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	FILETIME fileTime1;
	FILETIME fileTime2;

	GetFileTime(handle1, nullptr, nullptr, &fileTime1);
	GetFileTime(handle2, nullptr, nullptr, &fileTime2);

	CloseHandle(handle1);
	CloseHandle(handle2);

	if (fileTime1.dwHighDateTime > fileTime2.dwHighDateTime)
	{
		return true;
	}
	else if (fileTime1.dwHighDateTime == fileTime2.dwHighDateTime)
	{
		return (fileTime1.dwLowDateTime >= fileTime2.dwLowDateTime);
	}

	return false;
}

//*********************************************************************//
//	In:				directory - a path to the directory to check
//
//	Out:			true - the directory exists
//					false - the directory doesn't exist or an invalid path was submitted
//
//	Description:	Checks the specified directory to see if it exists
//*********************************************************************//
bool FileSystem::DirectoryExists(std::string directory)
{
	return DirectoryExistsW(Inferno::ToWideString(directory));
}

//*********************************************************************//
//	In:				directory - a path to the directory to check
//
//	Out:			true - the directory exists
//					false - the directory doesn't exist or an invalid path was submitted
//
//	Description:	Checks the specified directory to see if it exists
//*********************************************************************//
bool FileSystem::DirectoryExistsW(std::wstring directory)
{
	DWORD dir = GetFileAttributes(directory.c_str());

	// If the path does not exist
	if (dir == INVALID_FILE_ATTRIBUTES)
		return false;

	// Checks to see if this is a file directory
	if (dir & FILE_ATTRIBUTE_DIRECTORY)
		return true;

	// The passed in string is not a directory
	return false;
}

void FileSystem::GetFilesInDirectory(std::string folderPath, std::string fileExtension, std::vector<std::string>& fileNames)
{
	std::string search_path = folderPath + "*" + fileExtension;

	WIN32_FIND_DATAA findData;
	HANDLE hFind = ::FindFirstFileA(search_path.c_str(), &findData);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				std::string currFile = findData.cFileName;
				currFile = currFile.substr(0, currFile.find_last_of('.'));
				fileNames.push_back(currFile);
			}
		} while (::FindNextFileA(hFind, &findData));
		::FindClose(hFind);
	}
}

void FileSystem::GetFilesInDirectoryW(std::wstring folderPath, std::wstring fileExtension, std::vector<std::wstring>& fileNames)
{
	std::string search_path = Inferno::ToAsciiString(folderPath + L"*" + fileExtension);

	WIN32_FIND_DATAA findData;
	HANDLE hFind = ::FindFirstFileA(search_path.c_str(), &findData);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				std::wstring currFile = Inferno::ToWideString(findData.cFileName);
				currFile = currFile.substr(0, currFile.find_last_of('.'));
				fileNames.push_back(currFile);
			}
		} while (::FindNextFileA(hFind, &findData));
		::FindClose(hFind);
	}
}