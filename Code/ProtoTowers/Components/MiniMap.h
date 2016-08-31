//*********************************************************************//
//	Author:			Brian McLaine
//	Description:	
//*********************************************************************//
#pragma once

#include "UIComponents\UIComponent.h"

class UIRenderer;
class UIElement;

class MiniMap : public UIComponent
{
private: struct MapElement
{
	Transform* transform;
	UIElement* element;
};

private:
	UIRenderer* m_pImage;
	UIElement* m_pWarningBorder;

	float mapMinX, mapMaxX;
	float mapMinZ, mapMaxZ;

	float m_fWarningTime;
	float m_fFlashTime;

	InfernoTimer m_itWarningTimer;
	InfernoTimer m_itFlashTimer;

	bool m_bWarningIsRed;

	std::vector<MapElement*> m_vMapElements;

	void OnRegisterEntity(void* _pData);
	void OnUnRegisterEntity(void* _pData);

	void StartWarning(void* pData);
	void UpdateWarning();
	void EndWarning();

protected:


public:
	MiniMap(UIElement* _owner);

	virtual ~MiniMap() override;

	virtual void Init() override;

	virtual void Update() override;

	virtual void OnEnable() override;

	virtual void OnDisable() override;

	virtual void OnDestroy() override;

	void SetWorldBounds(float _minX, float _maxX, float _minZ, float _maxZ);
};