#include "Rendering\DXEvent.h"
#include "GlobalIncludes.h"

DXEvent::DXEvent(ID3DUserDefinedAnnotation* annotation, const char* name)
{
	m_pAnnotation = annotation;

	if (nullptr != m_pAnnotation)
		m_pAnnotation->BeginEvent(Inferno::ToWideString(name).c_str());
	else
		PRINT("No Pointer cannot Group Visual debugging calls");
}
DXEvent::DXEvent(ID3DUserDefinedAnnotation* annotation, const wchar_t* name)
{
	m_pAnnotation = annotation;

	if (nullptr != m_pAnnotation)
		m_pAnnotation->BeginEvent(name);
	else
		PRINT("No Pointer cannot Group Visual debugging calls");
}
DXEvent::DXEvent(ID3DUserDefinedAnnotation* annotation, std::string name)
{
	m_pAnnotation = annotation;

	if (nullptr != m_pAnnotation)
		m_pAnnotation->BeginEvent(Inferno::ToWideString(name).c_str());
	else
		PRINT("No Pointer cannot Group Visual debugging calls");
}
DXEvent::DXEvent(ID3DUserDefinedAnnotation* annotation, std::wstring name)
{
	m_pAnnotation = annotation;

	if (nullptr != m_pAnnotation)
		m_pAnnotation->BeginEvent(name.c_str());
	else
		PRINT("No Pointer cannot Group Visual debugging calls");
}
DXEvent::~DXEvent()
{
	if (nullptr != m_pAnnotation)
		m_pAnnotation->EndEvent();
	else
		PRINT("No Pointer cannot end log");
}