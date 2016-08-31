//*********************************************************************//
//	Author:			Brian McLaine
//	Description:	FileSystem is used to strore and retrieve commonly
//					used folder paths.
//*********************************************************************//

#pragma once

#include "GlobalIncludes.h"

class FileSystem
{
public:
	FileSystem();
	~FileSystem();

	std::string	GetResourcesFolder();
	std::wstring GetResourcesFolderW();

	std::string	GetAnimationsFolder();
	std::wstring GetAnimationsFolderW();

	std::string GetAnimationSetsFolder();
	std::wstring GetAnimationSetsFolderW();

	std::string GetAnimationControllersFolder();
	std::wstring GetAnimationControllersFolderW();

	std::string	GetAudioFolder();
	std::wstring GetAudioFolderW();

	std::string	GetGameObjectsFolder();
	std::wstring GetGameObjectsFolderW();

	std::string	GetIconsFolder();
	std::wstring GetIconsFolderW();

	std::string	GetMaterialsFolder();
	std::wstring GetMaterialsFolderW();

	std::string	GetMeshesFolder();
	std::wstring GetMeshesFolderW();

	std::string	GetParticlesFolder();
	std::wstring GetParticlesFolderW();

	std::string	GetScenesFolder();
	std::wstring GetScenesFolderW();

	std::string	GetTexturesFolder();
	std::wstring GetTexturesFolderW();

	std::string GetShaderFolder();
	std::wstring GetShaderFolderW();

	std::string GetPrefabsFolder();
	std::wstring GetPrefabsFolderW();

	std::string GetDataFolder();
	std::wstring GetDataFolderW();

	void SetResourcesFolder(const std::string& _folder);
	void SetResourcesFolderW(const std::wstring& _folder);

	void SetAudioFolder(const std::string& _folder);
	void SetAudioFolderW(const std::wstring& _folder);

	void SetAnimationsFolder(const std::string& _folder);
	void SetAnimationsFolderW(const std::wstring& _folder);

	void SetAnimationSetsFolder(const std::string& _folder);
	void SetAnimationSetsFolderW(const std::wstring& _folder);

	void SetAnimationControllersFolder(const std::string& _folder);
	void SetAnimationControllersFolderW(const std::wstring& _folder);

	void SetGameObjectsFolder(const std::string& _folder);
	void SetGameObjectsFolderW(const std::wstring& _folder);

	void SetIconsFolder(const std::string& _folder);
	void SetIconsFolderW(const std::wstring& _folder);

	void SetMaterialsFolder(const std::string& _folder);
	void SetMaterialsFolderW(const std::wstring& _folder);

	void SetMeshesFolder(const std::string& _folder);
	void SetMeshesFolderW(const std::wstring& _folder);

	void SetParticlesFolder(const std::string& _folder);
	void SetParticlesFolderW(const std::wstring& _folder);

	void SetScenesFolder(const std::string& _folder);
	void SetScenesFolderW(const std::wstring& _folder);

	void SetTexturesFolder(const std::string& _folder);
	void SetTexturesFolderW(const std::wstring& _folder);

	void SetShaderFolder(const std::string& _folder);
	void SetShaderFolderW(const std::wstring& _folder);

	void SetPrefabsFolder(const std::string& _folder);
	void SetPrefabsFolderW(const std::wstring& _folder);

	void SetDataFolder(const std::string& _folder);
	void SetDataFolderW(const std::wstring& _folder);

	//*********************************************************************//
	//	In:				_file - a path to the file being checked
	//
	//	Out:			true - if the file exists
	//					false - if the file does not exist
	//
	//	Description:	Checks for the specified file and returns a bool
	//					based on whether the file is found
	//*********************************************************************//
	bool FileExists(const std::string& _file);

	//*********************************************************************//
	//	In:				_file - a path to the file being checked
	//
	//	Out:			true - if the file exists
	//					false - if the file does not exist
	//
	//	Description:	Checks for the specified file and returns a bool
	//					based on whether the file is found
	//*********************************************************************//
	bool FileExistsW(const std::wstring& _file);

	//*********************************************************************//
	//	In:				_file1 - a path to the file being checked
	//					_file2 - a path to the file being checked against
	//
	//	Out:			true - if the file1 is newer than file2
	//					false - if file1 is not new than file2
	//
	//	Description:	Checks for the specified file and returns a bool
	//					based on whether the file is found
	//*********************************************************************//
	bool FileIsNewer(const std::string& _file1, const std::string& _file2);

	//*********************************************************************//
	//	In:				_file1 - a path to the file being checked
	//					_file2 - a path to the file being checked against
	//
	//	Out:			true - if the file1 is newer than file2
	//					false - if file1 is not new than file2
	//
	//	Description:	Checks for the specified file and returns a bool
	//					based on whether the file is found
	//*********************************************************************//
	bool FileIsNewerW(const std::wstring& _file1, const std::wstring& _file2);

	//*********************************************************************//
	//	In:				directory - a path to the directory to check
	//
	//	Out:			true - the directory exists
	//					false - the directory doesn't exist or an invalid path was submitted
	//
	//	Description:	Checks the specified directory to see if it exists
	//*********************************************************************//
	bool DirectoryExists(std::string directory);

	//*********************************************************************//
	//	In:				directory - a path to the directory to check
	//
	//	Out:			true - the directory exists
	//					false - the directory doesn't exist or an invalid path was submitted
	//
	//	Description:	Checks the specified directory to see if it exists
	//*********************************************************************//
	bool DirectoryExistsW(std::wstring directory);

	//*********************************************************************//
	//	In:				_folderPath - the path to the folder we're looking in
	//					_fileExtension - the type of file we're looking for
	//
	//	Out:			_fileNames - names of file found without their
	//					extensions
	//
	//	Description:	Checks the passed in folder path for any files
	//					matching the file extension type. If any are
	//					found, the filename is added to the out vector
	//*********************************************************************//
	void GetFilesInDirectory(std::string _folderPath, std::string _fileExtension, std::vector<std::string>& _fileNames);

	//*********************************************************************//
	//	In:				_folderPath - the path to the folder we're looking in
	//					_fileExtension - the type of file we're looking for
	//
	//	Out:			_fileNames - names of file found without their
	//					extensions
	//
	//	Description:	Checks the passed in folder path for any files
	//					matching the file extension type. If any are
	//					found, the filename is added to the out vector
	//*********************************************************************//
	void GetFilesInDirectoryW(std::wstring _folderPath, std::wstring _fileExtension, std::vector<std::wstring>& _fileNames);

private:
	std::wstring s_ResourcesFolder;
	std::wstring s_AnimationsFolder;
	std::wstring s_AnimationSetsFolder;
	std::wstring s_AnimationControllersFolder;
	std::wstring s_AudioFolder;
	std::wstring s_GameObjectsFolder;
	std::wstring s_IconsFolder;
	std::wstring s_MaterialsFolder;
	std::wstring s_MeshesFolder;
	std::wstring s_ParticlesFolder;
	std::wstring s_ScenesFolder;
	std::wstring s_TexturesFolder;
	std::wstring s_EngineShaderFolder;
	std::wstring s_GameShaderFolder;
	std::wstring s_PrefabFolder;
	std::wstring s_DataFolder;
};

