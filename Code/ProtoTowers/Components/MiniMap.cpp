//*********************************************************************//
//	Author:			Brian McLaine
//	Description:	
//*********************************************************************//
#include "stdafx.h"
#include "MiniMap.h"

#include "../InfernoUI.h"
#include "EventSystem.h"
#include "Entity.h"
#include "InfernoTimer.h"
#include "UIComponents\UIRenderer.h"

MiniMap::MiniMap(UIElement* _owner) : UIComponent(_owner)
{
	// EventSystem::GetInstance()->RegisterForEvent("EntityInstantiate", this, BindEventFunction(&MiniMap::OnRegisterEntity, this));
	// EventSystem::GetInstance()->RegisterForEvent("EntityDestroy", this, BindEventFunction(&MiniMap::OnUnRegisterEntity, this));
}

MiniMap::~MiniMap()
{

}

void MiniMap::Init()
{
	m_pWarningBorder = GetTransform()->GetChildByName(std::string("MiniMapBorderWarning"));
	m_fWarningTime = 6.0f;
	m_fFlashTime = 0.5f;
	m_bWarningIsRed = false;
}

void MiniMap::Update()
{
	size_t mapElementCount = m_vMapElements.size();
	for (size_t element = 0; element < mapElementCount; element++)
	{
		if (m_vMapElements[element]->transform != nullptr)
		{
			Vector3 position = m_vMapElements[element]->transform->GetPosition();
			float mapPosX = abs(mapMinX) + position.x;
			float mapPosZ = abs(mapMinZ) + position.z;

			float xRatio = mapPosX / (mapMaxX - mapMinX);
			float zRatio = 1 - (mapPosZ / (mapMaxZ - mapMinZ));

			Vector2 mapBounds = m_pRectTransform->GetTotalBounds();
			float miniMapPosX = mapBounds.x * xRatio;
			float miniMapPosZ = mapBounds.y * zRatio;

			m_vMapElements[element]->element->GetTransform()->SetPosition(Vector2(miniMapPosX, miniMapPosZ));

			float yRot = m_vMapElements[element]->transform->WorldMatrix().GetRotation().y;
			m_vMapElements[element]->element->GetTransform()->SetRotation(yRot);
		}
	}

	//	Vector2 mapBounds = m_pRectTransform->GetTotalBounds();
	//
	//	size_t mapElementCount = m_vMapElements.size();
	//	for (size_t element = 0; element < mapElementCount; element++)
	//	{
	//		Vector3 position = m_vMapElements[element]->transform->GetPosition();
	//		position.x += (worldWidth * 0.5f);
	//		position.z += (worldHeight * 0.5f);
	//
	//		float xPosRatio = position.x / worldWidth;
	//		float zPosRatio = position.z / worldHeight;
	//
	//		Vector2 newPos = Vector2(xPosRatio * mapBounds.x, zPosRatio * mapBounds.y);
	//
	//		m_vMapElements[element]->element->GetTransform()->SetPosition(newPos);
	//	}
}

void MiniMap::OnEnable()
{
	m_pWarningBorder->SetActive(false);

	EventSystem::GetInstance()->RegisterForEvent("EntityInstantiate", this, BindEventFunction(&MiniMap::OnRegisterEntity, this));
	EventSystem::GetInstance()->RegisterForEvent("EntityDestroy", this, BindEventFunction(&MiniMap::OnUnRegisterEntity, this));
	EventSystem::GetInstance()->RegisterForEvent("WarningEvent", this, BindEventFunction(&MiniMap::StartWarning, this));
}

void MiniMap::OnDisable()
{
	// size_t numElements = m_vMapElements.size();
	// for (size_t element = 0; element < numElements; element++)
	// {
	// 	m_vMapElements[element]->element->GetTransform()->SetParent(nullptr);
	// }
	// 
	// EventSystem::GetInstance()->UnregisterFromEvent("EntityInstantiate", this, BindEventFunction(&MiniMap::OnRegisterEntity, this));
	// EventSystem::GetInstance()->UnregisterFromEvent("EntityDestroy", this, BindEventFunction(&MiniMap::OnUnRegisterEntity, this));
	
	// EventSystem::GetInstance()->UnregisterFromEvent("EntityInstantiate", this, BindEventFunction(&MiniMap::OnRegisterEntity, this));
	// EventSystem::GetInstance()->UnregisterFromEvent("EntityDestroy", this, BindEventFunction(&MiniMap::OnUnRegisterEntity, this));
}

void MiniMap::OnDestroy()
{
	size_t numElements = m_vMapElements.size();
	for (size_t element = 0; element < numElements; element++)
	{
		m_vMapElements[element]->element->GetTransform()->SetParent(nullptr);
	}

	EventSystem::GetInstance()->UnregisterFromEvent("EntityInstantiate", this, BindEventFunction(&MiniMap::OnRegisterEntity, this));
	EventSystem::GetInstance()->UnregisterFromEvent("EntityDestroy", this, BindEventFunction(&MiniMap::OnUnRegisterEntity, this));
	EventSystem::GetInstance()->UnregisterFromEvent("WarningEvent", this, BindEventFunction(&MiniMap::StartWarning, this));
}

void MiniMap::OnRegisterEntity(void* _pData)
{
	MapElement* temp = static_cast<MapElement*>(_pData);

	Inferno_UI::AddToView(temp->element, "HUD");
	RectTransform* rectTransform = temp->element->GetTransform();
	rectTransform->SetParent(m_pUIElement->GetTransform());

	if (temp->element->GetName() == "BarrierIcon")
	{
		float barrierWidth = temp->transform->GetScale().x;
		float mapWidth = mapMaxX - mapMinX;
		float widthRatio = barrierWidth / mapWidth;

		Vector2 mapBounds = m_pRectTransform->GetTotalBounds();
		float miniMapWidth = mapBounds.x * widthRatio;

		rectTransform->SetBounds(Vector2(miniMapWidth, 10.0f));
	}

	temp->element->SetActive(true);
	m_vMapElements.push_back(temp);
}

void MiniMap::OnUnRegisterEntity(void* _pData)
{
	MapElement* temp = static_cast<MapElement*>(_pData);

	auto iter = m_vMapElements.begin();

	while (iter != m_vMapElements.end())
	{
		if (*iter == temp)
		{
			(*iter)->element->GetTransform()->SetParent(nullptr);
			(*iter)->element->SetActive(false);

			// swap the element being removed with the last element and remove from the back to prevent vector resizing.
			std::iter_swap(iter, m_vMapElements.end() - 1);
			m_vMapElements.pop_back();

			break;
		}

		++iter;
	}
}

void MiniMap::StartWarning(void* pData)
{
	m_itWarningTimer.SetTime(m_fWarningTime);
	m_itWarningTimer.SetOnTimerEndFunction(std::bind(&MiniMap::EndWarning, this));

	m_itFlashTimer.SetTime(0.5f);
	m_itFlashTimer.SetOnTimerEndFunction(std::bind(&MiniMap::UpdateWarning, this));

	m_pWarningBorder->SetActive(true);
}

void MiniMap::UpdateWarning()
{
	if (m_bWarningIsRed)
		m_pWarningBorder->GetComponent<UIRenderer>()->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
	else
		m_pWarningBorder->GetComponent<UIRenderer>()->SetColor(Color(1.0f, 0.0f, 0.0f, 1.0f));

	m_bWarningIsRed = !m_bWarningIsRed;

	m_itFlashTimer.SetTime(0.5f);
	m_itFlashTimer.SetOnTimerEndFunction(std::bind(&MiniMap::UpdateWarning, this));
}

void MiniMap::EndWarning()
{
	m_pWarningBorder->SetActive(false);
}


void MiniMap::SetWorldBounds(float _minX, float _maxX, float _minZ, float _maxZ)
{
	mapMinX = _minX;
	mapMinZ = _minZ;
	mapMaxX = _maxX;
	mapMaxZ = _maxZ;
}