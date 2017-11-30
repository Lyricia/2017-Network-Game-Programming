#pragma once
#include "Object\Object.h"

#define BRICK_MAX_HP	100.0
#define BLOCK_STUN_TIME 0.2

class CBrick : public CObject
{
public:
	CBrick(D2D_POINT_2F pt, D2D_RECT_F rc = RectF());
	virtual ~CBrick();

	virtual void Update(float fTimeElapsed) override;

	void Collide(float atk);

	void SetHP(float hp) { m_fHP = hp; }
	bool IsBroken() const { return m_fHP <= 0; }

private:
	float							m_fHP;
	float							m_fBlockStunTimer;
	bool							m_bCollision;
};