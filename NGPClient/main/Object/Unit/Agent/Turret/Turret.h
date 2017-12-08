#pragma once

#include "stdafx.h"
#include "Object\Unit\Agent\Agent.h"
#include "Object\Unit\Unit.h"

#define TURRET_MAX_HP			100.0
#define TURRET_SHOOT_TIME		0.4
#define TURRET_SHOOT_DAMAGE		4.0

class CTurret : public CAgent
{
protected:
	CObject *	m_pParent;
public:
	CTurret(D2D_POINT_2F pt, D2D_RECT_F rc = RectF());
	virtual ~CTurret();

	virtual void Shoot();
	virtual void Shoot(const D2D_POINT_2F & ptHitPos);

	virtual void Update(float fTimeElapsed) override;
	virtual void Draw(ID2D1HwndRenderTarget* pd2dRenderTarget) override;
	void SetParent(CObject* pParent) { m_pParent = pParent; }
	virtual void RegisterResourceManager(std::shared_ptr<CResourceManager> resMng) override;
};