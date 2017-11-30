#include "stdafx.h"
#include "Brick.h"

CBrick::CBrick(D2D_POINT_2F pt, D2D_RECT_F rc)
	: CObject(pt, rc)
	, m_fHP(BRICK_MAX_HP)
	, m_fBlockStunTimer(0)
{
	m_Tag = CObject::Type::Brick;
}

CBrick::~CBrick()
{
}

void CBrick::Update(float fTimeElapsed)
{
	if (m_bCollision)
	{
		m_fBlockStunTimer += fTimeElapsed;
		if (m_fBlockStunTimer > BLOCK_STUN_TIME)
		{
			m_bCollision = false;
			m_fBlockStunTimer = 0.f;
		}
	}
}

void CBrick::Collide(float atk)
{
	if (m_bCollision) return;
	m_fHP -= atk;
	m_bCollision = true;
	if (m_fHP < 0)
		m_fHP = 0;
}
