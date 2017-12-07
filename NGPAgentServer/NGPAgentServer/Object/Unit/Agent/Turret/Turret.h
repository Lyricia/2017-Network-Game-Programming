#pragma once

#include "stdafx.h"
#include "Object\Unit\Agent\Agent.h"
#include "Object\Unit\Unit.h"
#include "Server\AgentServer.h"

#define TURRET_MAX_HP			100.0
#define TURRET_SHOOT_TIME		0.4f

class CTurret : public CAgent
{
protected:
	CObject *	m_pParent;
public:
	CTurret(D2D_POINT_2F pt, D2D_RECT_F rc = RectF());
	virtual ~CTurret();

	virtual void Update(float fTimeElapsed) override;
	virtual void Move(const D2D_POINT_2F& ptVelocity) override;
	virtual void Move(const D2D_POINT_2F& ptVelocity, float fTimeElapsed) override;
	virtual void Move(RoomInfo* pRoom, const D2D_POINT_2F& ptMoveDirection) override;
	virtual void InterActionCheck(std::vector<CObject*>& pObjects) override;

	// 사격 스위치가 켜질 수 있는가.
	virtual bool IsCanbeShootable() const override { return (m_shoot_timer > TURRET_SHOOT_TIME); }

	void SetParent(CObject* pParent) { m_pParent = pParent; }
};