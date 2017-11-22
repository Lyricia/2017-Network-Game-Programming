#pragma once
#include "Object\Unit\Unit.h"

class CIndRes;
class CAgent : public CUnit
{
	CAgent(D2D_POINT_2F pt, D2D_RECT_F rc = RectF());
	virtual ~CAgent();

	virtual void Update(float fTimeElapsed) override;
	virtual void Draw(ID2D1HwndRenderTarget* pd2dRenderTarget) override;

	virtual void RegisterImage(
		CIndRes*					pIndRes
		, ID2D1HwndRenderTarget*	pd2dRenderTarget
		, path						filename);
	virtual void RegisterSpriteImage(
		CIndRes*					pIndRes
		, ID2D1HwndRenderTarget*	pd2dRenderTarget
		, path						filename
		, D2D_POINT_2F				ptLength);

	virtual void Collide(float atk) override;

	void Move(const D2D_POINT_2F& pt);
	void Move(float x, float y);
	void Shoot();
	void RayCastingToShoot(std::vector<CObject*>& pvecObjects);

	void SetHP(float hp) { m_fHP = hp; }
};