#pragma once

#include <string>
#include <d3d11_1.h>

class DXEvent
{
private:
	ID3DUserDefinedAnnotation* m_pAnnotation;

public:
	DXEvent(ID3DUserDefinedAnnotation* annotation, const char* name);
	DXEvent(ID3DUserDefinedAnnotation* annotation, const wchar_t* name);
	DXEvent(ID3DUserDefinedAnnotation* annotation, std::string name);
	DXEvent(ID3DUserDefinedAnnotation* annotation, std::wstring name);
	~DXEvent();
};

