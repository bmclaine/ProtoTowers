#include "Rendering\RenderingResources.h"
#include "FileSystem.h"
#include "GlobalIncludes.h"

#include "Rendering\Shaders\DX11VertexShader.h"
#include "Rendering\Shaders\DX11GeometryShader.h"
#include "Rendering\Shaders\DX11PixelShader.h"
#include "Rendering\Shaders\DX11ComputeShader.h"

#include "Rendering\Buffers\DX11VertexBuffer.h"
#include "Rendering\Buffers\DX11IndexBuffer.h"
#include "Rendering\Buffers\DX11ConstantBuffer.h"
#include "Rendering\Buffers\DX11StructuredBuffer.h"
#include "EventSystem.h"

#include <fstream>
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"

#include <d3dCompiler.h>
#include <d3d11shader.h>
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "dxguid.lib")

RenderingResources::RenderingResources()
{
	m_vShaders.reserve(20);
	m_vBuffers.reserve(40);
	m_vInputLayouts.reserve(10);
	m_vImages.reserve(300);
	m_vRenderTargets.reserve(10);
	m_vDepthStecilViews.reserve(10);
	m_vSampleStates.reserve(10);
	m_vBlendStates.reserve(10);
	m_vRasterizerStates.reserve(10);
	m_vViewports.reserve(10);

	EventSystem::GetInstance()->RegisterForEvent("AnisotropicChanged", this, BindEventFunction(&RenderingResources::AnisotropicChange, this));
}

RenderingResources::~RenderingResources()
{
	size_t index = 0;
	for (; index < m_vShaders.size(); index++)
		delete m_vShaders[index];
	m_vShaders.clear();

	for (index = 0; index < m_vBuffers.size(); index++)
		delete m_vBuffers[index];
	m_vBuffers.clear();

	for (index = 0; index < m_vInputLayouts.size(); index++)
		delete m_vInputLayouts[index];
	m_vInputLayouts.clear();

	for (index = 0; index < m_vImages.size(); index++)
		delete m_vImages[index];
	m_vImages.clear();

	for (index = 0; index < m_vUAVs.size(); index++)
		delete m_vUAVs[index];
	m_vUAVs.clear();

	for (index = 0; index < m_vRenderTargets.size(); index++)
		delete m_vRenderTargets[index];
	m_vRenderTargets.clear();

	for (index = 0; index < m_vDepthStecilViews.size(); index++)
		delete m_vDepthStecilViews[index];
	m_vDepthStecilViews.clear();

	for (index = 0; index < m_vSampleStates.size(); index++)
		delete m_vSampleStates[index];
	m_vSampleStates.clear();

	for (index = 0; index < m_vRasterizerStates.size(); index++)
		delete m_vRasterizerStates[index];
	m_vRasterizerStates.clear();

	for (index = 0; index < m_vBlendStates.size(); index++)
		delete m_vBlendStates[index];
	m_vBlendStates.clear();

	for (index = 0; index < m_vDpethStencilStates.size(); index++)
		delete m_vDpethStencilStates[index];
	m_vDpethStencilStates.clear();

	m_vViewports.clear();

	EventSystem::GetInstance()->UnregisterFromEvent("AnisotropicChanged", this, BindEventFunction(&RenderingResources::AnisotropicChange, this));
}

// IN: ID3D11Device* - pointer to the device
// OUT: void
//
// Stores a local pointer to the device
void RenderingResources::SetDevice(ID3D11Device* device)
{
	m_pDevice = device;
}
// IN: ID3D11DeviceContext* - pointer to the context
// OUT: void
//
// Stores a local pointer to the device context
void RenderingResources::SetContext(ID3D11DeviceContext* context)
{
	m_pContext = context;
}

// IN: void* - pointer to the new anisotropic value
// OUT: void
//
// Function to be called by the event system when the anisotropic value is changed
void RenderingResources::AnisotropicChange(void* pData)
{
	UINT newAnostropic = *static_cast<UINT*>(pData);

	auto iter = m_vSampleStates.begin();
	for (; iter != m_vSampleStates.end(); ++iter)
	{
		// Grab the current Sampler State
		ID3D11SamplerState* ss = (*iter)->m_pSamplerState;

		if (ss == nullptr)
			continue;

		// Grab the descriptor
		D3D11_SAMPLER_DESC desc;
		ss->GetDesc(&desc);

		// Make sure the Sampler State is using Anisotropic Filtering since it would do nothing for any
		// other filter type
		if (desc.Filter == D3D11_FILTER_ANISOTROPIC)
		{
			// Update our anisotropic value
			desc.MaxAnisotropy = newAnostropic;

			// Release our old Sampler State and recreate it using the new value
			SAFE_RELEASE(ss);
			m_pDevice->CreateSamplerState(&desc, &ss);
			(*iter)->m_pSamplerState = ss;
		}
	}
}

// IN: ShaderType - the type of shader being loaded
//     wchar_t*   - the name of the shader
// OUT: int - ID of loaded shader, -1 for error
//
// Loads the shader if it doesn't already exist, otherwise returns the ID of the shader
int RenderingResources::LoadShader(ShaderType type, const wchar_t* shaderName)
{
	// Make sure this shader doesn't exist before we try to load it
	for (size_t shader = 0; shader < m_vShaders.size(); shader++)
	{
		if (m_vShaders[shader]->GetType() == type && m_vShaders[shader]->m_sShaderName == shaderName)
			return (int)shader;
	}

	FileSystem fs;
	std::wstring filepath = fs.GetShaderFolderW() + shaderName + L".cso";

	HANDLE hFile = CreateFileW(filepath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	// Check to see if we failed to open the file
	if (hFile == INVALID_HANDLE_VALUE)
	{
		std::wstring error = std::wstring(L"Failed to load shader ") + shaderName + L"!\n";
		PRINT(error.c_str());
		return -1;
	}

	// Get the file size
	LARGE_INTEGER FileSize = { 0 };

#if (_WIN32_WINNT >= _WIN32_WINNT_VISTA)
	FILE_STANDARD_INFO fileInfo;
	if (!GetFileInformationByHandleEx(hFile, FileStandardInfo, &fileInfo, sizeof(fileInfo))) {
		CloseHandle(hFile);
		return -1;
	}
	FileSize = fileInfo.EndOfFile;
#else
	GetFileSizeEx(hFile, &FileSize);
#endif

	// Make sure our file is not too big for a 32-bit allocation
	if (FileSize.HighPart > 0)
	{
		std::wstring error = std::wstring(L"Shader file ") + shaderName + L" too large to load!\n";
		PRINT(error.c_str());
		CloseHandle(hFile);
	}

	char* shaderData = new char[FileSize.LowPart];

	if (!ASSERT(shaderData != nullptr, "Failed to allocate a buffer for the shader file!"))
		return -1;

	// Try to read the whole file
	DWORD BytesRead = 0;
	if (!ReadFile(hFile, shaderData, FileSize.LowPart, &BytesRead, nullptr))
	{
		std::wstring error = std::wstring(L"Failed to read the shader file ") + shaderName + L"!\n";
		PRINT(error.c_str());

		CloseHandle(hFile);
		delete[] shaderData;
		return -1;
	}

	// Close the file since we have now read it
	CloseHandle(hFile);

	// Make sure we were able to read the whole file
	if (BytesRead < FileSize.LowPart)
	{
		std::wstring error = std::wstring(L"Failed to read the whole shader file ") + shaderName + L"!\n";
		PRINT(error.c_str());

		delete[] shaderData;
		return -1;
	}

	ID3DBlob* blob;
	if (FAILED(D3DCreateBlob(FileSize.LowPart, &blob)))
	{
		std::wstring error = std::wstring(L"Failed to create a D3DBlob of size ") + std::to_wstring(FileSize.LowPart);
		ALERT(error.c_str());

		delete[] shaderData;
		return -1;
	}

	memcpy(blob->GetBufferPointer(), shaderData, FileSize.LowPart);
	delete[] shaderData;

	HRESULT hr;
	DX11Shader* shader;

	switch (type)
	{
	case VERTEX_SHADER:
	{
		ID3D11VertexShader* vShader = nullptr;
		hr = m_pDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &vShader);
		SET_DEBUG_NAME(vShader, Inferno::ToAsciiString(shaderName).c_str())
		shader = new DX11VertexShader(vShader);
		break;
	}
	case GEOMETRY_SHADER:
	{
		ID3D11GeometryShader* gShader = nullptr;
		hr = m_pDevice->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &gShader);
		SET_DEBUG_NAME(gShader, Inferno::ToAsciiString(shaderName).c_str())
		shader = new DX11GeometryShader(gShader);
		break;
	}
	case PIXEL_SHADER:
	{
		ID3D11PixelShader* pShader = nullptr;
		hr = m_pDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pShader);
		SET_DEBUG_NAME(pShader, Inferno::ToAsciiString(shaderName).c_str())
		shader = new DX11PixelShader(pShader);
		break;
	}
	case COMPUTE_SHADER:
	{
		ID3D11ComputeShader* cShader = nullptr;
		hr = m_pDevice->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &cShader);
		SET_DEBUG_NAME(cShader, Inferno::ToAsciiString(shaderName).c_str())
		shader = new DX11ComputeShader(cShader);
		break;
	}
	}

	if (FAILED(hr))
	{
		std::wstring error = std::wstring(L"Failed to create shader ") + shaderName + L" from blob!\n";
		PRINT(error.c_str());

		blob->Release();
		delete shader;
		return -1;
	}

	shader->m_sShaderName = shaderName;
	shader->m_pCompiledShader = blob;
	m_vShaders.push_back(shader);

	return (int)m_vShaders.size() - 1;
}

// IN: ShaderType - the type of shader being loaded
//     wchar_t*   - the name of the shader
//     wchar_t*   - the entry function name
//     wchar_t*   - the model name, ex. vs_4_0
// OUT: int - ID of loaded shader, -1 for error
//
// Loads the shader if it doesn't already exist, otherwise returns the ID of the shader
int RenderingResources::LoadUncompiledShader(ShaderType type, const wchar_t* shaderName, const wchar_t* function, const wchar_t* model)
{
	ID3DBlob* compiledShader = nullptr;

	char aFunction[100];
	char aModel[100];
	WideCharToMultiByte(CP_ACP, 0, function, -1, aFunction, 100, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, model, -1, aModel, 100, NULL, NULL);

	UINT flags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	FileSystem fs;
	std::wstring filepath = fs.GetShaderFolderW() + shaderName + L".hlsl";

	HANDLE hFile = CreateFileW(filepath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	// Check to see if we failed to open the file
	if (hFile == INVALID_HANDLE_VALUE)
	{
		std::wstring error = std::wstring(L"Failed to load shader ") + shaderName + L"!\n";
		PRINT(error.c_str());
		return -1;
	}

	// Get the file size
	LARGE_INTEGER FileSize = { 0 };

#if (_WIN32_WINNT >= _WIN32_WINNT_VISTA)
	FILE_STANDARD_INFO fileInfo;
	if (!GetFileInformationByHandleEx(hFile, FileStandardInfo, &fileInfo, sizeof(fileInfo))) {
		CloseHandle(hFile);
		return -1;
	}
	FileSize = fileInfo.EndOfFile;
#else
	GetFileSizeEx(hFile, &FileSize);
#endif

	// Make sure our file is not too big for a 32-bit allocation
	if (FileSize.HighPart > 0)
	{
		std::wstring error = std::wstring(L"Shader file ") + shaderName + L" too large to load!\n";
		PRINT(error.c_str());
		CloseHandle(hFile);
	}

	char* shaderData = new char[FileSize.LowPart];

	if (!ASSERT(shaderData != nullptr, "Failed to allocate a buffer for the shader file!"))
		return -1;

	// Try to read the whole file
	DWORD BytesRead = 0;
	if (!ReadFile(hFile, shaderData, FileSize.LowPart, &BytesRead, nullptr))
	{
		std::wstring error = std::wstring(L"Failed to read the shader file ") + shaderName + L"!\n";
		PRINT(error.c_str());

		CloseHandle(hFile);
		delete[] shaderData;
		return -1;
	}

	// Close the file since we have now read it
	CloseHandle(hFile);

	if (FAILED(D3DCompile(shaderData,
		FileSize.LowPart,
		nullptr,
		nullptr,
		nullptr,
		aFunction,
		aModel,
		flags,
		0,
		&compiledShader,
		nullptr)))
	{
		std::wstring error = std::wstring(L"Error compiling shader ") + shaderName + L"!\n";
		PRINT(error.c_str());

		delete[] shaderData;
		return -1;
	}

	delete[] shaderData;
	
	HRESULT hr;
	DX11Shader* shader;

	switch (type)
	{
	case VERTEX_SHADER:
	{
		ID3D11VertexShader* vShader = nullptr;
		hr = m_pDevice->CreateVertexShader(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), nullptr, &vShader);
		SET_DEBUG_NAME(vShader, Inferno::ToAsciiString(shaderName).c_str())
		shader = new DX11VertexShader(vShader);
		break;
	}
	case GEOMETRY_SHADER:
	{
		ID3D11GeometryShader* gShader = nullptr;
		hr = m_pDevice->CreateGeometryShader(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), nullptr, &gShader);
		SET_DEBUG_NAME(gShader, Inferno::ToAsciiString(shaderName).c_str())
		shader = new DX11GeometryShader(gShader);
		break;
	}
	case PIXEL_SHADER:
	{
		ID3D11PixelShader* pShader = nullptr;
		hr = m_pDevice->CreatePixelShader(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), nullptr, &pShader);
		SET_DEBUG_NAME(pShader, Inferno::ToAsciiString(shaderName).c_str())
		shader = new DX11PixelShader(pShader);
		break;
	}
	case COMPUTE_SHADER:
	{
		ID3D11ComputeShader* cShader = nullptr;
		hr = m_pDevice->CreateComputeShader(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), nullptr, &cShader);
		SET_DEBUG_NAME(cShader, Inferno::ToAsciiString(shaderName).c_str())
		shader = new DX11ComputeShader(cShader);
		break;
	}
	}

	if (FAILED(hr))
	{
		std::wstring error = std::wstring(L"Failed to create shader ") + shaderName + L" from blob!\n";
		PRINT(error.c_str());

		compiledShader->Release();
		delete shader;
		return -1;
	}

	shader->m_sShaderName = shaderName;
	shader->m_pCompiledShader = compiledShader;
	m_vShaders.push_back(shader);

	return (int)m_vShaders.size() - 1;
}

void RenderingResources::CreateGeometryShaderWithStreamOutput(int ID, D3D11_SO_DECLARATION_ENTRY* entry, UINT numEntries, UINT* strides, UINT numStrides)
{
	DX11GeometryShader* gShader = (DX11GeometryShader*)GetShaderByID(ShaderType::GEOMETRY_SHADER, ID);
	std::wstring shaderName = gShader->m_sShaderName;

	if (!gShader)
		return;

	SAFE_RELEASE(gShader->m_pShader);

	m_pDevice->CreateGeometryShaderWithStreamOutput(gShader->m_pCompiledShader->GetBufferPointer(), gShader->m_pCompiledShader->GetBufferSize(),
		entry, numEntries, strides, numStrides, 0, nullptr, &gShader->m_pShader);

	SET_DEBUG_NAME(gShader->m_pShader, Inferno::ToAsciiString(shaderName).c_str())
}

// IN: void
// OUT: void
//
// Loops through all our shaders and applies any constant buffers they need based on reflection
bool RenderingResources::ApplyConstantBuffers()
{
	FileSystem fs;

#if defined(DEBUG) || defined(_DEBUG)
	std::wstring reflectionFilePath = fs.GetShaderFolderW() + L"ReflectionData.srd";
	bool fileExists = fs.FileExistsW(reflectionFilePath);
	bool fileNewer = false;

	// If the file exists
	if (fileExists)
	{
		// Loop through each loaded shader and check to see if any of the shaders are newer then the reflection data file
		for (size_t shader = 0; shader < m_vShaders.size(); shader++)
		{
			std::wstring filepath = fs.GetShaderFolderW() + m_vShaders[shader]->m_sShaderName + L".cso";

			if (fs.FileIsNewerW(filepath, reflectionFilePath))
			{
				fileNewer = true;
				break;
			}
		}
	}

	if (fileExists && !fileNewer)
	{
		// If the file exists and is no shader file is newer then the reflection data file, load from the file
		std::ifstream bin;
		bin.open(fs.GetShaderFolder() + "ReflectionData.srd", std::ios_base::in | std::ios_base::binary);

		if (ASSERT(bin.is_open(), "Failed to open the shader reflection data file!"))
		{
			// Create a shader name buffer
			wchar_t* shaderName = new wchar_t[50];
			wchar_t* cBufferName = new wchar_t[50];

			while (true)
			{
				// Read in the length of the shader name
				// If file is empty or last shader was read in, this will cause the eof flag to pop
				UINT nameLength = 0;
				bin.read((char*)&nameLength, sizeof(UINT));

				// Check to make sure we aren't at the end of the file
				if (bin.eof())
					break;

				// Read in the shader name
				
				bin.read((char*)shaderName, sizeof(wchar_t) * nameLength);

				// Read in how many constant buffers we have
				UINT numBoundResources = 0;
				bin.read((char*)&numBoundResources, sizeof(UINT));

				// Grab the shader we will be storing the data into
				// If it equals null, we will still need to go through the file so we won't break if it is
				DX11Shader* shader = GetShader(shaderName);

				for (UINT cBuffer = 0; cBuffer < numBoundResources; cBuffer++)
				{
					UINT bufferNameLength = 0;
					UINT bufferSlot = 0;
					UINT bufferSize = 0;
					D3D_SHADER_INPUT_TYPE type;

					bin.read((char*)&type, sizeof(D3D_SHADER_INPUT_TYPE));

					if (type != D3D_CT_CBUFFER && type != D3D_CT_TBUFFER)
						continue;

					bin.read((char*)&bufferNameLength, sizeof(UINT));
					bin.read((char*)cBufferName, sizeof(wchar_t) * bufferNameLength);
					bin.read((char*)&bufferSlot, sizeof(UINT));
					bin.read((char*)&bufferSize, sizeof(UINT));

					// Make sure we actually got a shader and if so add the constant buffer to it
					if (shader)
						shader->AddConstantBuffer(bufferSlot, GetConstantBufferByID(CreateConstantBuffer(cBufferName, bufferSize)));
				}
			}

			// clean up our buffer data
			delete[] shaderName;
			delete[] cBufferName;

			bin.close();

			return true;
		}
	}
	else
	{
		// Create a file to write our reflection data to
		std::ofstream bout;
		bout.open(fs.GetShaderFolder() + "ReflectionData.srd", std::ios_base::out | std::ios_base::binary);

		// If the file failed to open, report the error and stop trying to get the reflect data
		if (!ASSERT(bout.is_open(), "Failed to create the shader reflection file!"))
			return false;

		// If our file doesn't exist or at least one of the shaders are newer then our reflection data, we need to recreate the file
		for (size_t index = 0; index < m_vShaders.size(); index++)
		{
			DX11Shader* shader = m_vShaders[index];

			ID3D11ShaderReflection* Reflector = nullptr;
			HRESULT hr = D3DReflect(shader->m_pCompiledShader->GetBufferPointer(), shader->m_pCompiledShader->GetBufferSize(),
				IID_ID3D11ShaderReflection, (void**)&Reflector);

			if (FAILED(hr))
			{
				std::wstring error = std::wstring(L"Failed to reflect shader ") + shader->m_sShaderName + L"!\n";
				PRINT(Inferno::ToAsciiString(error).c_str());

				continue;
			}

			// If we were able to reflect the shader, get the shader description
			D3D11_SHADER_DESC shaderDesc;
			Reflector->GetDesc(&shaderDesc);

			// Write out the length of the shader name, the shader name, and the number of constant buffers
			UINT size = (UINT)shader->m_sShaderName.size() + 1;
			bout.write((char*)&size, sizeof(UINT));
			bout.write((char*)shader->m_sShaderName.c_str(), sizeof(wchar_t) * size);
			bout.write((char*)&shaderDesc.BoundResources, sizeof(UINT));

			// Loop through each constant buffer the shader has
			for (UINT resource = 0; resource < shaderDesc.BoundResources; resource++)
			{
				D3D11_SHADER_INPUT_BIND_DESC bindDesc;
				Reflector->GetResourceBindingDesc(resource, &bindDesc);

				bout.write((char*)&bindDesc.Type, sizeof(D3D_SHADER_INPUT_TYPE));

				// Make sure we are grabbing a constant buffer
				if (bindDesc.Type == D3D_CT_CBUFFER || bindDesc.Type == D3D_CT_TBUFFER)
				{
					ID3D11ShaderReflectionConstantBuffer* cbuffer = Reflector->GetConstantBufferByName(bindDesc.Name);
					D3D11_SHADER_BUFFER_DESC bufferDesc;
					cbuffer->GetDesc(&bufferDesc);

					std::wstring bufferName = Inferno::ToWideString(bindDesc.Name);
					UINT bufferSize = bufferDesc.Size;
					
					// Write out the buffer name size, buffer name, and the size of the buffer
					size = (UINT)bufferName.size() + 1;
					bout.write((char*)&size, sizeof(UINT));
					bout.write((char*)bufferName.c_str(), sizeof(wchar_t) * size);
					bout.write((char*)&bindDesc.BindPoint, sizeof(UINT));
					bout.write((char*)&bufferSize, sizeof(UINT));

					// Create the constant buffer and add it to the current shaders constant buffer list
					shader->AddConstantBuffer(bindDesc.BindPoint, GetConstantBufferByID(CreateConstantBuffer(bufferName, bufferSize)));
				}
			}

			Reflector->Release();
		}

		bout.close();

		return true;
	}
#else
	if (!ASSERT(fs.FileExists(fs.GetShaderFolder() + "ReflectionData.srd"), "Reflection data file does not exist!"))
		return false;

	// If the file exists and is no shader file is newer then the reflection data file, load from the file
	std::ifstream bin;
	bin.open(fs.GetShaderFolder() + "ReflectionData.srd", std::ios_base::in | std::ios_base::binary);

	if (ASSERT(bin.is_open(), "Failed to open the shader reflection data file!"))
	{
		// Create a shader name buffer
		wchar_t* shaderName = new wchar_t[50];
		wchar_t* cBufferName = new wchar_t[50];

		while (true)
		{
			// Read in the length of the shader name
			// If file is empty or last shader was read in, this will cause the eof flag to pop
			UINT nameLength = 0;
			bin.read((char*)&nameLength, sizeof(UINT));

			// Check to make sure we aren't at the end of the file
			if (bin.eof())
				break;

			// Read in the shader name

			bin.read((char*)shaderName, sizeof(wchar_t) * nameLength);

			// Read in how many constant buffers we have
			UINT numConstBuffers = 0;
			bin.read((char*)&numConstBuffers, sizeof(UINT));

			// Grab the shader we will be storing the data into
			// If it equals null, we will still need to go through the file so we won't break if it is
			DX11Shader* shader = GetShaderByName(shaderName);

			for (UINT cBuffer = 0; cBuffer < numConstBuffers; cBuffer++)
			{
				UINT bufferNameLength = 0;
				UINT bufferSlot = 0;
				UINT bufferSize = 0;
				D3D_SHADER_INPUT_TYPE type;

				bin.read((char*)&type, sizeof(D3D_SHADER_INPUT_TYPE));

				if (type != D3D_CT_CBUFFER && type != D3D_CT_TBUFFER)
					continue;

				bin.read((char*)&bufferNameLength, sizeof(UINT));
				bin.read((char*)cBufferName, sizeof(wchar_t) * bufferNameLength);
				bin.read((char*)&bufferSlot, sizeof(UINT));
				bin.read((char*)&bufferSize, sizeof(UINT));

				// Make sure we actually got a shader and if so add the constant buffer to it
				if (shader)
					shader->AddConstantBuffer(bufferSlot, GetConstantBufferByID(CreateConstantBuffer(cBufferName, bufferSize)));
			}
		}

		// clean up our buffer data
		delete[] shaderName;
		delete[] cBufferName;

		bin.close();

		return true;
	}
#endif

	return false;
}

// IN: UINT - size of the constant buffer
// OUT: int - ID of the buffer, -1 for error
//
// Creates a new constant buffer if one of the same name doesn't exist, otherwise returns the buffer ID if it does exist
int RenderingResources::CreateConstantBuffer(std::wstring bufferName, UINT bufferSize)
{
	// First check to see if we have any constant buffers by the same name, and if so return it
	for (size_t buffer = 0; buffer < m_vBuffers.size(); buffer++)
	{
		if (m_vBuffers[buffer]->GetType() == CONSTANT_BUFFER && m_vBuffers[buffer]->m_sName == bufferName)
			return (int)buffer;
	}

	D3D11_BUFFER_DESC cBuffer;
	ZeroMemory(&cBuffer, sizeof(D3D11_BUFFER_DESC));
	cBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBuffer.ByteWidth = bufferSize;
	cBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cBuffer.Usage = D3D11_USAGE_DYNAMIC;

	ID3D11Buffer* buffer;
	HRESULT hr = m_pDevice->CreateBuffer(&cBuffer, nullptr, &buffer);

	if (FAILED(hr))
	{
		std::wstring error = std::wstring(L"Failed to create constant buffer ") + bufferName + L"!\n";
		PRINT(error.c_str());

		return -1;
	}

	SET_DEBUG_NAME(buffer, Inferno::ToAsciiString(bufferName).c_str())

	DX11Buffer* newBuffer = new DX11ConstantBuffer(buffer);
	newBuffer->m_sName = bufferName;
	m_vBuffers.push_back(newBuffer);
	return (int)m_vBuffers.size() - 1;
}

// IN: wstring            - name of the buffer
//     D3D11_BUFFER_DESC* - pointer to the descriptor
// OUT: int - ID of the buffer, -1 on error
//
// Creates a Vertex Buffer based on the incoming information
int RenderingResources::CreateVertexBuffer(std::wstring bufferName, D3D11_BUFFER_DESC* desc)
{
	// Check to see if we have a buffer by this name already
	int exists = GetBufferByName(VERTEX_BUFFER, bufferName);
	if (exists != -1)
		return exists;

	if (!(desc->BindFlags & D3D11_BIND_VERTEX_BUFFER))
	{
		ALERT("Invalid binding used for creating a vertex buffer!");
		return -1;
	}

	ID3D11Buffer* vBuffer;

	if (FAILED(m_pDevice->CreateBuffer(desc, nullptr, &vBuffer)))
		return -1;

	SET_DEBUG_NAME(vBuffer, Inferno::ToAsciiString(bufferName).c_str())

	DX11Buffer* buffer = new DX11VertexBuffer(vBuffer);
	buffer->m_sName = bufferName;
	m_vBuffers.push_back(buffer);
	return (int)m_vBuffers.size() - 1;
}

// IN: wstring            - name of the buffer
//     D3D11_BUFFER_DESC* - pointer to the descriptor
// OUT: int - ID of the buffer, -1 on error
//
// Creates an Index Buffer based on the incoming information
int RenderingResources::CreateIndexBuffer(std::wstring bufferName, D3D11_BUFFER_DESC* desc)
{
	// Check to see if we have a buffer by this name already
	int exists = GetBufferByName(INDEX_BUFFER, bufferName);
	if (exists != -1)
		return exists;

	if (!(desc->BindFlags & D3D11_BIND_INDEX_BUFFER))
	{
		ALERT("Invalid binding used for creating an index buffer!");
		return -1;
	}

	ID3D11Buffer* iBuffer;

	if (FAILED(m_pDevice->CreateBuffer(desc, nullptr, &iBuffer)))
		return -1;

	SET_DEBUG_NAME(iBuffer, Inferno::ToAsciiString(bufferName).c_str())

	DX11Buffer* buffer = new DX11IndexBuffer(iBuffer);
	buffer->m_sName = bufferName;
	m_vBuffers.push_back(buffer);
	return (int)m_vBuffers.size() - 1;
}

// IN: wstring            - name of the buffer
//     D3D11_BUFFER_DESC* - pointer to the descriptor
// OUT: int - ID of the buffer, -1 on error
//
// Creates a Structured Buffer based on the incoming information
int RenderingResources::CreateStructuredBuffer(std::wstring bufferName, D3D11_BUFFER_DESC* desc)
{
	// Check to see if we have a buffer by this name already
	int exists = GetBufferByName(STRUCTURED_BUFFER, bufferName);
	if (exists != -1)
		return exists;

	//if (!(desc->MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED))
	//{
	//	ALERT("Invalid misc flags used for creating a structured buffer!");
	//	return -1;
	//}

	ID3D11Buffer* sBuffer;

	HRESULT hr;
	if (FAILED(hr = m_pDevice->CreateBuffer(desc, nullptr, &sBuffer)))
		return -1;

	SET_DEBUG_NAME(sBuffer, Inferno::ToAsciiString(bufferName).c_str())

	DX11Buffer* buffer = new DX11StructuredBuffer(sBuffer);
	buffer->m_sName = bufferName;
	m_vBuffers.push_back(buffer);
	return (int)m_vBuffers.size() - 1;
}

// IN: D3D11_INPUT_ELEMENT_DESC* - pointer to the elements list
//     UINT                      - number of elements
// OUT: int - ID of the created layout, -1 on error
//
// Creates a layout and returns the ID
int RenderingResources::CreateInputLayout(std::wstring name, D3D11_INPUT_ELEMENT_DESC* elems, UINT elemCount, UINT ShaderID)
{
	DX11Shader* shader = GetShaderByID(VERTEX_SHADER, ShaderID);
	ID3D11InputLayout* inputLayout;

	// Make sure a valid shader was found
	if (!shader)
		return -1;

	if (FAILED(m_pDevice->CreateInputLayout(elems, elemCount, shader->m_pCompiledShader->GetBufferPointer(),
		shader->m_pCompiledShader->GetBufferSize(), &inputLayout)))
		return -1;

	SET_DEBUG_NAME(inputLayout, Inferno::ToAsciiString(name).c_str())

	DX11InputLayout* newLayout = new DX11InputLayout(inputLayout);
	newLayout->m_sName = name;

	m_vInputLayouts.push_back(newLayout);
	return (int)m_vInputLayouts.size() - 1;
}

// IN: string - name of the image to load including the type (ex. png, bmp, etc..)
// OUT: int - ID of the image, -1 on error
//
// Creates an image based on incoming name and returns the ID
int RenderingResources::CreateImage(std::string imageName)
{
	return CreateImage(Inferno::ToWideString(imageName));
}
// IN: wstring - name of the image to load including the type (ex. png, bmp, etc..)
// OUT: int - ID of the image, -1 on error
//
// Creates an image based on incoming name and returns the ID
int RenderingResources::CreateImage(std::wstring imageName)
{
	FileSystem fs;
	std::wstring filepath = fs.GetTexturesFolderW() + imageName;

	ID3D11ShaderResourceView* SRV;

	// Check to see if the image is a DDS texture or not
	if (imageName[imageName.length() - 2] == L'd')
	{
		if (FAILED(CreateDDSTextureFromFile(m_pDevice, filepath.c_str(), nullptr, &SRV)))
			return -1;
	}
	else // If it's not a dds, load through WICLoader
	{
		// === ***** CAUSING HEAP CORRUPTION SOME HOW *****

//		if (FAILED(DirectX::CreateWICTextureFromFile(m_pDevice, filepath.c_str(), nullptr, &SRV)))
			return -1;
	}

	SET_DEBUG_NAME(SRV, Inferno::ToAsciiString(imageName).c_str())

	DX11ShaderResourceView* newSRV = new DX11ShaderResourceView(SRV);
	newSRV->m_sName = imageName;
	newSRV->m_eType = SHADER_RESOURCE_IMAGE;

	m_vImages.push_back(newSRV);
	return (int)m_vImages.size() - 1;
}
// IN: wstring - name of the image
// OUT: int - ID of the slot
//
// Creates an empty texture slot for later use
int RenderingResources::RequestTextureSlot(std::wstring imageName)
{
	DX11ShaderResourceView* newSRV = new DX11ShaderResourceView(nullptr);
	newSRV->m_sName = imageName;
	newSRV->m_eType = SHADER_RESOURCE_IMAGE;

	m_vImages.push_back(newSRV);
	return (int)m_vImages.size() - 1;
}
// IN: UINT   - ID of the slot the data will go into
//     void*  - Pointer to the memory location of the image data
//     size_t - Size of the data in bytes
// OUT: void
//
// Create an image based on the data stored in memory and store it in the slot requested
void RenderingResources::CreateImageFromMemory(UINT imageID, void* image, size_t sizeInBytes)
{
	if (image == nullptr || imageID >= m_vImages.size() || m_vImages[imageID]->m_pSRV != nullptr)
		return;

	ID3D11ShaderResourceView* SRV;
	DX11ShaderResourceView* currSRV = m_vImages[imageID];

	HRESULT hr = CreateDDSTextureFromMemory(m_pDevice, (uint8_t*)image, sizeInBytes, nullptr, &SRV);
	if (FAILED(hr))
		return;

	SET_DEBUG_NAME(SRV, Inferno::ToAsciiString(currSRV->m_sName).c_str())
	currSRV->m_pSRV = SRV;
}
// IN: wstring - name of the image
//     void*   - pointer to a Shader Resource View
// OUT: int - ID of the Image added
//
// Adds the shader resource view to the active list of images
int RenderingResources::AddImage(std::wstring name, void* image)
{
	ID3D11ShaderResourceView* SRV = static_cast<ID3D11ShaderResourceView*>(image);

	if (SRV)
	{
		SET_DEBUG_NAME(SRV, Inferno::ToAsciiString(name).c_str())

		DX11ShaderResourceView* newSRV = new DX11ShaderResourceView(SRV);
		newSRV->m_sName = name;
		newSRV->m_eType = SHADER_RESOURCE_IMAGE;
		m_vImages.push_back(newSRV);

		return (int)m_vImages.size() - 1;
	}

	return -1;
}
// IN: wstring  - Name for the ShaderResource
//     wstring* - Pointer to the array of image names
//     uint     - number of image names
// OUT: int - ID to the ShaderResourceView
int RenderingResources::CreateTextureArray(std::wstring name, std::wstring* textureNames, UINT numTextures)
{
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.Width = 512;
	desc.Height = 512;
	desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.ArraySize = numTextures;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	D3D11_SUBRESOURCE_DATA srData[255] = {};
	ID3D11Texture2D* textures[255] = {};

	ID3D11ShaderResourceView* temp = nullptr;
	for (UINT texture = 0; texture < numTextures; texture++)
	{
		CreateDDSTextureFromFile(m_pDevice, textureNames[texture].c_str(), reinterpret_cast<ID3D11Resource**>(&textures[texture]), &temp);
		SAFE_RELEASE(temp);

		D3D11_TEXTURE2D_DESC test;
		textures[texture]->GetDesc(&test);

		D3D11_MAPPED_SUBRESOURCE resource;
		m_pContext->Map(textures[texture], 0, D3D11_MAP_READ, 0, &resource);
		m_pContext->Unmap(textures[texture], 0);

		srData[texture].pSysMem = textures[texture];
		srData[texture].SysMemPitch = desc.Width * 4;
		srData[texture].SysMemSlicePitch = desc.Width * desc.Height * 4;
	}

	ID3D11Texture2D* textArray = nullptr;
	
	if (!SUCCEEDED(m_pDevice->CreateTexture2D(&desc, srData, &textArray)))
	{
		for (UINT texture = 0; texture < numTextures; texture++)
			SAFE_RELEASE(textures[texture]);

		return -1;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = desc.Format;
	srvDesc.Texture2DArray.MipLevels = 1;
	srvDesc.Texture2DArray.MostDetailedMip = 0;
	srvDesc.Texture2DArray.FirstArraySlice = 0;
	srvDesc.Texture2DArray.ArraySize = numTextures;
	srvDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DARRAY;

	if (!SUCCEEDED(m_pDevice->CreateShaderResourceView(textArray, &srvDesc, &temp)))
	{
		for (UINT texture = 0; texture < numTextures; texture++)
			SAFE_RELEASE(textures[texture]);

		SAFE_RELEASE(textArray);

		return -1;
	}

	for (UINT texture = 0; texture < numTextures; texture++)
		SAFE_RELEASE(textures[texture]);

	DX11ShaderResourceView* newSRV = new DX11ShaderResourceView(temp);
	newSRV->m_eType = SHADER_RESOURCE_IMAGE;
	newSRV->m_sName = name;
	m_vImages.push_back(newSRV);
	return (int)m_vImages.size() - 1;
}

// IN: int - ID of the buffer to create a Shader Resource for
// OUT: int - ID of the Shader Resource, -1 on error
//
// Creates a Shader Resource for the requested buffer and names it the same name as the buffer
int RenderingResources::CreateBufferSRV(int BufferID, DXGI_FORMAT format, D3D11_BUFFER_DESC *desc)
{
	DX11Buffer* buffer = GetBufferByID(BufferID);

	if (buffer->GetType() != STRUCTURED_BUFFER)
	{
		ALERT("Trying to create a Shader Resource for an invalid buffer type!");
		return -1;
	}

	D3D11_BUFFER_DESC buffDesc;
	if (desc == nullptr)
	{
		ZeroMemory(&buffDesc, sizeof(D3D11_BUFFER_DESC));
		buffer->GetBuffer()->GetDesc(&buffDesc);
	}
	else
		buffDesc = *desc;

	D3D11_SHADER_RESOURCE_VIEW_DESC rescDesc;
	ZeroMemory(&rescDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	rescDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	rescDesc.BufferEx.FirstElement = 0;

	rescDesc.Format = format;
	rescDesc.BufferEx.NumElements = buffDesc.ByteWidth / buffDesc.StructureByteStride;

	ID3D11ShaderResourceView* SRV;
	HRESULT hr;
	if (!SUCCEEDED(hr = m_pDevice->CreateShaderResourceView(buffer->GetBuffer(), &rescDesc, &SRV)))
	{
		std::wstring error = std::wstring(L"Failed to create the Shader Resource View for the buffer ") + buffer->m_sName + L"!";
		ALERT(error.c_str());

		return -1;
	}

	SET_DEBUG_NAME(SRV, Inferno::ToAsciiString(buffer->m_sName).c_str())

	DX11ShaderResourceView* newSRV = new DX11ShaderResourceView(SRV);
	newSRV->m_sName = buffer->m_sName;
	newSRV->m_eType = SHADER_RESOURCE_STRUCTURED_BUFFER;
	m_vImages.push_back(newSRV);
	return (int)m_vImages.size() - 1;
}

// IN: int - ID of the buffer to create a Unordered Access View
// OUT: int - ID of the Unordered Access View
//
// Creates an unordered access view for the requested buffer and names it the same name as the buffer
int RenderingResources::CreateUnorderedAccessView(int BufferID, UINT Flags, DXGI_FORMAT Format, D3D11_BUFFER_DESC *desc)
{
	DX11Buffer* buffer = GetBufferByID(BufferID);

	D3D11_BUFFER_DESC buffDesc;
	ZeroMemory(&buffDesc, sizeof(D3D11_BUFFER_DESC));
	buffer->GetBuffer()->GetDesc(&buffDesc);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;

	uavDesc.Format = Format;
	if (buffDesc.StructureByteStride)
		uavDesc.Buffer.NumElements = buffDesc.ByteWidth / buffDesc.StructureByteStride;
	else if (desc)
		uavDesc.Buffer.NumElements = desc->ByteWidth / desc->StructureByteStride;
	uavDesc.Buffer.Flags = Flags;

	ID3D11UnorderedAccessView* UAV = nullptr;

	if (!SUCCEEDED(m_pDevice->CreateUnorderedAccessView(buffer->GetBuffer(), &uavDesc, &UAV)))
	{
		std::wstring error = std::wstring(L"Failed to create the Unordered Access View for the buffer ") + buffer->m_sName + L"!";
		ALERT(error.c_str());

		return -1;
	}

	SET_DEBUG_NAME(UAV, Inferno::ToAsciiString(buffer->m_sName).c_str())

	DX11UnorderedAccessView* newUAV = new DX11UnorderedAccessView(UAV);
	newUAV->m_sName = buffer->m_sName;
	m_vUAVs.push_back(newUAV);
	return (int)m_vUAVs.size() - 1;
}
// IN: int                  - ID of the shader resource view
//     UINT                 - Size of each element in bytes
//     UINT                 - Flags to apply to the UAV
// OUT: int - ID of the created UAV, -1 on error
//
// Creates an unordered access view for the requested shader resource view
int RenderingResources::CreateUnorderedAccessViewFromTexture(int textureID, UINT Flags, DXGI_FORMAT format)
{
	DX11ShaderResourceView* dxSRV = GetImageByID(textureID);

	if (!dxSRV)
		return -1;

	ID3D11ShaderResourceView* SRV = dxSRV->m_pSRV;

	if (!SRV)
		return -1;

	ID3D11Texture2D* texture;
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	SRV->GetResource(reinterpret_cast<ID3D11Resource**>(&texture));

	if (!texture)
		return -1;

	texture->GetDesc(&desc);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	if (format == DXGI_FORMAT_UNKNOWN)
		uavDesc.Format = desc.Format;
	else
		uavDesc.Format = format;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = desc.Height * desc.Width;
	uavDesc.Buffer.Flags = Flags;

	ID3D11UnorderedAccessView* UAV = nullptr;

	if (!SUCCEEDED(m_pDevice->CreateUnorderedAccessView(texture, &uavDesc, &UAV)))
	{
		std::wstring error = std::wstring(L"Failed to create the Unordered Access View for the buffer ") + dxSRV->m_sName + L"!";
		ALERT(error.c_str());

		SAFE_RELEASE(texture);

		return -1;
	}

	SAFE_RELEASE(texture);

	SET_DEBUG_NAME(UAV, Inferno::ToAsciiString(dxSRV->m_sName).c_str())

	DX11UnorderedAccessView* newUAV = new DX11UnorderedAccessView(UAV);
	newUAV->m_sName = dxSRV->m_sName;
	m_vUAVs.push_back(newUAV);
	return (int)m_vUAVs.size() - 1;
}

// IN: ID3D11Texture2D*               - pointer to the texture to use
//     D3D11_RENDER_TARGET_VIEW_DESC* - pointer to the descriptor
// OUT: int - ID of the RTV, -1 on error
//
// Creates a Render Target View based on the incoming information and returns the ID
int RenderingResources::CreateRenderTargetView(std::wstring name, ID3D11Texture2D* texture, D3D11_RENDER_TARGET_VIEW_DESC* desc)
{
	ID3D11RenderTargetView* RTV;

	if (FAILED(m_pDevice->CreateRenderTargetView(texture, desc, &RTV)))
		return -1;

	SET_DEBUG_NAME(RTV, Inferno::ToAsciiString(name).c_str())

	DX11RenderTargetView* newRTV = new DX11RenderTargetView(RTV);
	newRTV->m_sName = name;

	m_vRenderTargets.push_back(newRTV);
	return (int)m_vRenderTargets.size() - 1;
}
// IN: UINT                           - ID of image to use
//     D3D11_RENDER_TARGET_VIEW_DESC* - pointer to the descriptor
// OUT: int - ID of RTV, -1 on error
//
// Creates a Render Target View based on the incoming information and returns the ID
int RenderingResources::CreateRenderTargetView(std::wstring name, UINT imageID, D3D11_RENDER_TARGET_VIEW_DESC* desc)
{
	ID3D11RenderTargetView* RTV;
	ID3D11Texture2D* Tex2D;

	DX11ShaderResourceView* SRV = GetImageByID(imageID);

	// Make sure a valid image was found
	if (!SRV)
		return -1;

	SRV->m_pSRV->QueryInterface(&Tex2D);

	// Make sure there was a vaild Texture 2D in the Shader Resource View
	if (!Tex2D)
		return -1;

	if (FAILED(m_pDevice->CreateRenderTargetView(Tex2D, desc, &RTV)))
	{
		SAFE_RELEASE(Tex2D);
		return -1;
	}

	SAFE_RELEASE(Tex2D);

	SET_DEBUG_NAME(RTV, Inferno::ToAsciiString(name).c_str())

	DX11RenderTargetView* newRTV = new DX11RenderTargetView(RTV);
	newRTV->m_sName = name;

	m_vRenderTargets.push_back(newRTV);
	return (int)m_vRenderTargets.size() - 1;
}

// IN: ID3D11Texture2D*               - pointer to the texture to use
//     D3D11_DEPTH_STENCIL_VIEW_DESC* - pointer to the descriptor
// OUT: int - ID of DSV, -1 on error
//
// Creates a Depth Stencil View based on the incoming information
int RenderingResources::CreateDepthStencilView(std::wstring name, ID3D11Texture2D* texture, D3D11_DEPTH_STENCIL_VIEW_DESC* desc)
{
	ID3D11DepthStencilView* DSV;

	if (FAILED(m_pDevice->CreateDepthStencilView(texture, desc, &DSV)))
		return -1;

	SET_DEBUG_NAME(DSV, Inferno::ToAsciiString(name).c_str())

	DX11DepthStencilView* newDSV = new DX11DepthStencilView(DSV);
	newDSV->m_sName = name;

	m_vDepthStecilViews.push_back(newDSV);
	return (int)m_vDepthStecilViews.size() - 1;
}
// IN: UINT                           - ID of image to use
//     D3D11_DEPTH_STENCIL_VIEW_DESC* - pointer to the descriptor
// OUT: int - ID of DSV, -1 on error
//
// Creates a Depth Stencil View based on the incoming information
int RenderingResources::CreateDepthStencilView(std::wstring name, UINT imageID, D3D11_DEPTH_STENCIL_VIEW_DESC* desc)
{
	ID3D11DepthStencilView* DSV;
	ID3D11Texture2D* Tex2D;

	DX11ShaderResourceView* SRV = GetImageByID(imageID);

	// Make sure a valid image was found
	if (!SRV)
		return -1;

	SRV->m_pSRV->QueryInterface(&Tex2D);

	// Make sure there was a vaild Texture 2D in the Shader Resource View
	if (!Tex2D)
		return -1;

	if (FAILED(m_pDevice->CreateDepthStencilView(Tex2D, desc, &DSV)))
	{
		SAFE_RELEASE(Tex2D);
		return -1;
	}

	SAFE_RELEASE(Tex2D);

	SET_DEBUG_NAME(DSV, Inferno::ToAsciiString(name).c_str())

	DX11DepthStencilView* newDSV = new DX11DepthStencilView(DSV);
	newDSV->m_sName = name;

	m_vDepthStecilViews.push_back(newDSV);
	return (int)m_vDepthStecilViews.size() - 1;
}

// IN: D3D11_SAMPLER_DESC* - pointer to the descriptor
// OUT: int - ID of the Sampler State, -1 on error
//
// Creates a sampler state base on the incoming information
int RenderingResources::CreateSamplerState(std::wstring name, D3D11_SAMPLER_DESC* desc)
{
	ID3D11SamplerState* SS;

	if (FAILED(m_pDevice->CreateSamplerState(desc, &SS)))
		return -1;

	SET_DEBUG_NAME(SS, Inferno::ToAsciiString(name).c_str())

	DX11SampleState* newSS = new DX11SampleState(SS);
	newSS->m_sName = name;

	m_vSampleStates.push_back(newSS);
	return (int)m_vSampleStates.size() - 1;
}

// IN: wstring                - name of the sampler state
//     D3D11_RASTERIZER_DESC* - pointer to the descriptor
// OUT: int - ID of the Rasterizer State, -1 on error
//
// Creates a rasterizer state based on the incoming information
int RenderingResources::CreateRasterizerState(std::wstring name, D3D11_RASTERIZER_DESC* desc)
{
	ID3D11RasterizerState* RS;

	if (FAILED(m_pDevice->CreateRasterizerState(desc, &RS)))
		return -1;

	SET_DEBUG_NAME(RS, Inferno::ToAsciiString(name).c_str())

	DX11RasterizerState* newRS = new DX11RasterizerState(RS);
	newRS->m_sName = name;

	m_vRasterizerStates.push_back(newRS);
	return (int)m_vRasterizerStates.size() - 1;
}

// IN: D3D11_BLEND_DESC* - pointer to the descriptor
// OUT: int - ID of the Blend State, -1 on error
//
// Creates a blend state base on the incoming information
int RenderingResources::CreateBlendState(std::wstring name, D3D11_BLEND_DESC* desc)
{
	ID3D11BlendState* BS;

	if (FAILED(m_pDevice->CreateBlendState(desc, &BS)))
		return -1;

	SET_DEBUG_NAME(BS, Inferno::ToAsciiString(name).c_str())

	DX11BlendState* newBS = new DX11BlendState(BS);
	newBS->m_sName = name;
	newBS->m_bAlphaBlending = desc->RenderTarget[0].BlendEnable == TRUE ? true : false;

	m_vBlendStates.push_back(newBS);
	return (int)m_vBlendStates.size() - 1;
}

// IN: D3D11_DEPTH_STENCIL_DESC* - pointer to the descriptor
// OUT: int - ID of the Depth Stencil State, -1 on error
//
// Creates a depth stencil state base on the incoming information
int RenderingResources::CreateDepthStencilState(std::wstring name, D3D11_DEPTH_STENCIL_DESC* desc)
{
	ID3D11DepthStencilState* DSS;

	if (FAILED(m_pDevice->CreateDepthStencilState(desc, &DSS)))
		return -1;

	SET_DEBUG_NAME(DSS, Inferno::ToAsciiString(name).c_str())

	DX11DepthStencilState* newDSS = new DX11DepthStencilState(DSS);
	newDSS->m_sName = name;

	m_vDpethStencilStates.push_back(newDSS);
	return (int)m_vDpethStencilStates.size() - 1;
}

// IN: D3D11_VIEWPORT - viewport to add
// OUT: UINT - ID of the added viewport
//
// Adds the incoming viewport to the list of viewports
UINT RenderingResources::AddViewport(D3D11_VIEWPORT viewport)
{
	m_vViewports.push_back(viewport);

	return (UINT)m_vViewports.size() - 1;
}

// IN: UINT - ID to use
// OUT: DX11ContantBuffer* - Pointer to the requested buffer, nullptr if error
//
// Returns the constant buffer corresponding to the corresponding ID
DX11ConstantBuffer* RenderingResources::GetConstantBufferByID(UINT ID)
{
	// If the ID is out of range, or the requested buffer is not a constant buffer return nullptr
	if (ID >= m_vBuffers.size() || m_vBuffers[ID]->GetType() != CONSTANT_BUFFER)
		return nullptr;

	return (DX11ConstantBuffer*)m_vBuffers[ID];
}

// IN: string - name of buffer
// OUT: int - ID of the requested buffer, nullptr if error
//
// Returns the id to the constant buffer corresponding to the incoming name
int RenderingResources::GetConstantBufferByName(std::string name)
{
	return GetConstantBufferByName(Inferno::ToWideString(name));
}
// IN: wstring - name of buffer
// OUT: int - ID of the requested buffer, nullptr if error
//
// Returns the id to the constant buffer corresponding to the incoming name
int RenderingResources::GetConstantBufferByName(std::wstring name)
{
	// Loop through all our buffers and see if we can find a constant buffer with the passed in name
	for (size_t buffer = 0; buffer < m_vBuffers.size(); buffer++)
	{
		if (m_vBuffers[buffer]->GetType() == CONSTANT_BUFFER && m_vBuffers[buffer]->m_sName == name)
			return (int)buffer;
	}

	return -1;
}

// IN: wstring - name of requested shader
// OUT: DX11Shader* - pointer to the requested shader, nullptr on error
//
// Returns the shader of corresponding name. This is mainly for the reflection to use.
DX11Shader* RenderingResources::GetShader(std::wstring name)
{
	for (size_t index = 0; index < m_vShaders.size(); index++)
	{
		if (m_vShaders[index]->m_sShaderName == name)
			return m_vShaders[index];
	}

	return nullptr;
}
// IN: ShaderType - type of shader
//     UINT       - ID of requested shader
// OUT: DX11Shader* - pointer to requested shader, nullptr on error
//
// Returns the shader at the requested ID
DX11Shader* RenderingResources::GetShaderByID(ShaderType type, UINT ID)
{
	// Check to see if our ID is in bounds and the requested shader is of the same type
	if (ID >= m_vShaders.size() || m_vShaders[ID]->GetType() != type)
		return nullptr;

	return m_vShaders[ID];
}

// IN: wchar_t* - name of the shader
// OUT: DX11Shader* - pointer to the shader with the corresponding name
//
// Returns the first shader with the corresponding name if one exists
DX11Shader* RenderingResources::GetShaderByName(const wchar_t* shaderName)
{
	for (size_t index = 0; index < m_vShaders.size(); index++)
	{
		if (m_vShaders[index]->m_sShaderName == shaderName)
			return m_vShaders[index];
	}

	return nullptr;
}

// IN: ShaderType - type of shader
//     string     - name of requested shader
// OUT: int - ID of requested shader, -1 on error
//
// Returns the shader with the corresponding name
int RenderingResources::GetShaderByName(ShaderType type, std::string name)
{
	return GetShaderByName(type, Inferno::ToWideString(name));
}
// IN: ShaderType - type of shader
//     wstring    - name of requested shader
// OUT: int - ID of requested shader, -1 on error
//
// Returns the shader with the corresponding name
int RenderingResources::GetShaderByName(ShaderType type, std::wstring name)
{
	for (size_t index = 0; index < m_vShaders.size(); index++)
	{
		if (m_vShaders[index]->GetType() == type && m_vShaders[index]->m_sShaderName == name)
			return (int)index;
	}

	return -1;
}

// IN: UINT - ID of buffer
// OUT: DX11Buffer* - Pointer to requested buffer, nullptr on error
//
// Returns the requested buffer for the passed in type and ID
DX11Buffer* RenderingResources::GetBufferByID(UINT ID)
{
	if (ID >= m_vBuffers.size())
		return nullptr;

	return m_vBuffers[ID];
}
// IN: BufferType - Type to look for
//     UINT       - ID of buffer
// OUT: DX11Buffer* - Pointer to requested buffer, nullptr on error
//
// Returns the requested buffer for the passed in type and ID
DX11Buffer* RenderingResources::GetBufferByID(BufferType type, UINT ID)
{
	if (ID >= m_vBuffers.size() || m_vBuffers[ID]->GetType() != type)
		return nullptr;

	return m_vBuffers[ID];
}
// IN: BufferType - Type to look for
//     string     - name of buffer
// OUT: DX11Buffer* - Pointer to requested buffer, nullptr on error
//
// Returns the requested buffer for the passed in name and type
int RenderingResources::GetBufferByName(BufferType type, std::string name)
{
	return GetBufferByName(type, Inferno::ToWideString(name));
}
// IN: BufferType - Type to look for
//     wstring    - name of buffer
// OUT: DX11Buffer* - Pointer to requested buffer, nullptr on error
//
// Returns the requested buffer for the passed in name and type
int RenderingResources::GetBufferByName(BufferType type, std::wstring name)
{
	for (size_t buffer = 0; buffer < m_vBuffers.size(); buffer++)
	{
		if (m_vBuffers[buffer]->GetType() == type && m_vBuffers[buffer]->m_sName == name)
			return (int)buffer;
	}

	return -1;
}

// IN: UINT - ID of Input Layout
// OUT: ID3D11InputLayout* - pointer to Input Layout, nullptr on error
//
// Returns the requested Input Layout
DX11InputLayout* RenderingResources::GetInputLayouByID(UINT ID)
{
	if (ID >= m_vInputLayouts.size())
		return nullptr;

	return m_vInputLayouts[ID];
}
// IN: string - name of the layout
// OUT: int - id of the layout, -1 on error
//
// Returns the ID for the requested Input Layout
int RenderingResources::GetInputLayoutByName(std::string name)
{
	return GetInputLayoutByName(Inferno::ToWideString(name));
}
// IN: wstring - name of the layout
// OUT: int - id of the layout, -1 on error
//
// Returns the ID for the requested Input Layout
int RenderingResources::GetInputLayoutByName(std::wstring name)
{
	for (size_t layout = 0; layout < m_vInputLayouts.size(); layout++)
	{
		if (m_vInputLayouts[layout]->m_sName == name)
			return (int)layout;
	}

	return -1;
}

// IN: UINT - ID of Image
// OUT: ID3D11ShaderResoureView* - pointer to image, nullptr on error
//
// Returns the requested image
DX11ShaderResourceView* RenderingResources::GetImageByID(UINT ID)
{
	if (ID >= m_vImages.size())
		return nullptr;

	return m_vImages[ID];
}
// IN: string - name of image
// OUT: int - ID of image
//
// Returns the requested image ID
int RenderingResources::GetImageByName(std::string name)
{
	return GetImageByName(Inferno::ToWideString(name));
}
// IN: wstring - name of image
// OUT: int - ID of image
//
// Returns the requested image ID
int RenderingResources::GetImageByName(std::wstring name)
{
	for (size_t image = 0; image < m_vImages.size(); image++)
	{
		if (m_vImages[image]->m_sName == name && m_vImages[image]->GetType() == SHADER_RESOURCE_IMAGE)
			return (int)image;
	}

	return -1;
}

// IN: SHADER_RESOURCE_TYPE - type of resource to get
//     UINT                 - ID of Shader Resource
// OUT: ID3D11ShaderResoureView* - pointer to Shader Resource, nullptr on error
//
// Returns the requested shader resource
DX11ShaderResourceView* RenderingResources::GetShaderResourceByID(SHADER_RESOURCE_TYPE type, UINT ID)
{
	if (ID >= m_vImages.size() || m_vImages[ID]->GetType() != type)
		return nullptr;

	return m_vImages[ID];
}
// IN: SHADER_RESOURCE_TYPE - type of resource to get
//     string               - name of Shader Resource
// OUT: int - ID of Shader Resource
//
// Returns the requested shader resource ID
int RenderingResources::GetShaderResourceByName(SHADER_RESOURCE_TYPE type, std::string name)
{
	return GetShaderResourceByName(type, Inferno::ToWideString(name));
}
// IN: SHADER_RESOURCE_TYPE - type of resource to get
//     wstring              - name of Shader Resource
// OUT: int - ID of Shader Resource
//
// Returns the requested shader resource ID
int RenderingResources::GetShaderResourceByName(SHADER_RESOURCE_TYPE type, std::wstring name)
{
	for (size_t srv = 0; srv < m_vImages.size(); srv++)
	{
		if (m_vImages[srv]->m_sName == name && m_vImages[srv]->GetType() == type)
			return (int)srv;
	}

	return -1;
}

// IN: UINT - ID of Unordered Access View
// OUT: DX11UnorderedAccessView* - pointer to Unordered Access View, nullptr on error
//
// Returns the requested Unordered Access View
DX11UnorderedAccessView* RenderingResources::GetUnorderedAccessViewByID(UINT ID)
{
	if (ID >= m_vUAVs.size())
		return nullptr;

	return m_vUAVs[ID];
}
// IN: string - name of Unordered Access View
// OUT: int - ID of Unordered Access View
//
// Returns the requested Unordered Access View ID
int RenderingResources::GetUnorderedAccessViewByName(std::string name)
{
	return GetUnorderedAccessViewByName(Inferno::ToWideString(name));
}
// IN: wstring - name of Unordered Access View
// OUT: int - ID of Unordered Access View
//
// Returns the requested Unordered Access View ID
int RenderingResources::GetUnorderedAccessViewByName(std::wstring name)
{
	for (size_t uav = 0; uav < m_vUAVs.size(); uav++)
	{
		if (m_vUAVs[uav]->m_sName == name)
			return (int)uav;
	}

	return -1;
}

// IN: UINT - ID of Render Target View
// OUT: ID3D11RenderTargetView* - pointer to Render Target View, nullptr on error
//
// Returns the requested Render Target View
DX11RenderTargetView* RenderingResources::GetRenderTargetViewByID(UINT ID)
{
	if (ID >= m_vRenderTargets.size())
		return nullptr;

	return m_vRenderTargets[ID];
}
// IN: string - name of the Render Target View
// OUT: int - id of the Render Target View, -1 on error
//
// Returns the ID for the requested Render Target View
int RenderingResources::GetRenderTargetViewByName(std::string name)
{
	return GetRenderTargetViewByName(Inferno::ToWideString(name));
}
// IN: wstring - name of the Render Target View
// OUT: int - id of the Render Target View, -1 on error
//
// Returns the ID for the requested Render Target View
int RenderingResources::GetRenderTargetViewByName(std::wstring name)
{
	for (size_t rtv = 0; rtv < m_vRenderTargets.size(); rtv++)
	{
		if (m_vRenderTargets[rtv]->m_sName == name)
			return (int)rtv;
	}

	return -1;
}

// IN: UINT - ID of Depth Stencil View
// OUT: ID3D11DepthStencilView* - pointer to Depth Stencil View, nullptr on error
//
// Returns the requested Depth Stencil View
DX11DepthStencilView* RenderingResources::GetDepthStencilViewByID(UINT ID)
{
	if (ID >= m_vDepthStecilViews.size())
		return nullptr;

	return m_vDepthStecilViews[ID];
}
// IN: string - name of the Depth Stencil View
// OUT: int - id of the Depth Stencil View, -1 on error
//
// Returns the ID for the requested Depth Stencil View
int RenderingResources::GetDepthStencilViewByName(std::string name)
{
	return GetDepthStencilViewByName(Inferno::ToWideString(name));
}
// IN: wstring - name of the Depth Stencil View
// OUT: int - id of the Depth Stencil View, -1 on error
//
// Returns the ID for the requested Depth Stencil View
int RenderingResources::GetDepthStencilViewByName(std::wstring name)
{
	for (size_t dsv = 0; dsv < m_vDepthStecilViews.size(); dsv++)
	{
		if (m_vDepthStecilViews[dsv]->m_sName == name)
			return (int)dsv;
	}

	return -1;
}

// IN: UINT - ID of Sampler State
// OUT: ID3D11SamplerState* - pointer to Sampler State, nullptr on error
//
// Returns the requested Sampler State
DX11SampleState* RenderingResources::GetSamplerStateByID(UINT ID)
{
	if (ID >= m_vSampleStates.size())
		return nullptr;

	return m_vSampleStates[ID];
}
// IN: string - name of the SamplerState
// OUT: int - id of the SamplerState, -1 on error
//
// Returns the ID for the requested SamplerState
int RenderingResources::GetSamplerStateByName(std::string name)
{
	return GetSamplerStateByName(Inferno::ToWideString(name));
}
// IN: wstring - name of the SamplerState
// OUT: int - id of the SamplerState, -1 on error
//
// Returns the ID for the requested SamplerState
int RenderingResources::GetSamplerStateByName(std::wstring name)
{
	for (size_t state = 0; state < m_vSampleStates.size(); state++)
	{
		if (m_vSampleStates[state]->m_sName == name)
			return (int)state;
	}

	return -1;
}

// IN: UINT - ID of Rasterizer State
// OUT: DX11RasterizerState* - pointer to Rasterizer State, nullptr on error
//
// Returns the requested Rasterizer State
DX11RasterizerState* RenderingResources::GetRasterizerStateByID(UINT ID)
{
	if (ID >= m_vRasterizerStates.size())
		return nullptr;

	return m_vRasterizerStates[ID];
}
// IN: string - name of the Rasterizer State
// OUT: int - id of the Rasterizer State, -1 on error
//
// Returns the ID for the requested Rasterizer State
int RenderingResources::GetRasterizerStateByName(std::string name)
{
	return GetRasterizerStateByName(Inferno::ToWideString(name));
}
// IN: string - name of the Rasterizer State
// OUT: int - id of the Rasterizer State, -1 on error
//
// Returns the ID for the requested Rasterizer State
int RenderingResources::GetRasterizerStateByName(std::wstring name)
{
	for (size_t state = 0; state < m_vRasterizerStates.size(); state++)
	{
		if (m_vRasterizerStates[state]->m_sName == name)
			return (int)state;
	}

	return -1;
}

// IN: UINT - ID of Blend State
// OUT: DX11BlendState* - pointer to Blend State, nullptr on error
//
// Returns the requested Blend State
DX11BlendState* RenderingResources::GetBlendStateByID(UINT ID)
{
	if (ID >= m_vBlendStates.size())
		return nullptr;

	return m_vBlendStates[ID];
}
// IN: string - name of the Blend State
// OUT: int - id of the Blend State, -1 on error
//
// Returns the ID for the requested Blend State
int RenderingResources::GetBlendStateByName(std::string name)
{
	return GetBlendStateByName(Inferno::ToWideString(name));
}
// IN: wstring - name of the Blend State
// OUT: int - id of the Blend State, -1 on error
//
// Returns the ID for the requested Blend State
int RenderingResources::GetBlendStateByName(std::wstring name)
{
	for (size_t state = 0; state < m_vBlendStates.size(); state++)
	{
		if (m_vBlendStates[state]->m_sName == name)
			return (int)state;
	}

	return -1;
}

// IN: UINT - ID of Depth Stencil State
// OUT: DX11DepthStencilState* - pointer to Depth Stencil State, nullptr on error
//
// Returns the requested Depth Stencil State
DX11DepthStencilState* RenderingResources::GetDepthStencilStateByID(UINT ID)
{
	if (ID >= m_vDpethStencilStates.size())
		return nullptr;

	return m_vDpethStencilStates[ID];
}
// IN: string - name of the Depth Stencil State
// OUT: int - id of the Depth Stencil State, -1 on error
//
// Returns the ID for the requested Depth Stencil State
int RenderingResources::GetDepthStencilStateByName(std::string name)
{
	return GetDepthStencilStateByName(Inferno::ToWideString(name));
}
// IN: wstring - name of the Depth Stencil State
// OUT: int - id of the Depth Stencil State, -1 on error
//
// Returns the ID for the requested Depth Stencil State
int RenderingResources::GetDepthStencilStateByName(std::wstring name)
{
	for (size_t state = 0; state < m_vDpethStencilStates.size(); state++)
	{
		if (m_vDpethStencilStates[state]->m_sName == name)
			return (int)state;
	}

	return -1;
}

// IN: UINT  - ID of buffer to update
//     void* - pointer to the data to update
//     UINT  - size of data in bytes
// OUT: void
//
// Updates the sub resource requested if the resource is a valid buffer
void RenderingResources::UpdateSubResource(UINT ID, void* data, UINT size)
{
	DX11Buffer* buffer = GetBufferByID(ID);

	if (!buffer)
		return;

	D3D11_MAPPED_SUBRESOURCE resource;
	m_pContext->Map(buffer->m_pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, data, size);
	m_pContext->Unmap(buffer->m_pBuffer, 0);
}

// IN: int - ID of the buffer to copy to
//     int - offset to use
//     int - ID of the UVA that has a counter
// OUT: void
//
// Copies the invisible counter on the UAV to the buffer
void RenderingResources::CopyStructureCount(int bufferID, int offset, int UAVID)
{
	DX11Buffer* buffer = GetConstantBufferByID(bufferID);
	DX11UnorderedAccessView* uav = GetUnorderedAccessViewByID(UAVID);

	if (!buffer || !uav)
		return;

	m_pContext->CopyStructureCount(buffer->m_pBuffer, offset, uav->m_pUAV);
}

// IN: UINT - ID of Viewport
// OUT: D3D11_VIEWPORT* - pointer to Viewport, nullptr on error
//
// Returns the requested Viewport
D3D11_VIEWPORT* RenderingResources::GetViewportByID(UINT ID)
{
	if (ID >= m_vViewports.size())
		return nullptr;

	return &m_vViewports[ID];
}