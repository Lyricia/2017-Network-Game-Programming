#include "stdafx.h"
#include "Turret.h"



CTurret::CTurret(D2D_POINT_2F pt, D2D_RECT_F rc)
	: CAgent(pt, rc)
{

	m_pTarget = nullptr;
	m_ptTargetPos = { 0,0 };
	m_fBlockStunTimer = 0;
	m_bCollision = false;
	m_bShoot = true;
	m_ptVelocity = Point2F();

	m_Tag = CObject::Type::Agent;
	m_next_change_dir_timer = rand() % 5 + 1;
	m_fHP = TURRET_MAX_HP;

	m_pParent = nullptr;
}

CTurret::~CTurret()
{
}

void CTurret::Update(float fTimeElapsed)
{
	Move(AGENT_VELOCITY * fTimeElapsed);

	m_ptVelocity -= m_ptVelocity * fTimeElapsed * FRICTIONAL_DRAG;
	if (Length(m_ptVelocity) < FRICTIONAL_DRAG)
	{
		m_ptVelocity = Point2F();
		m_bMove = false;
	}


	if (m_bShoot)
	{
		m_fShootTimer += fTimeElapsed;
		if (m_fShootTimer > SHOOT_TIME)
		{
			m_bShoot = false;
			m_fShootTimer = 0.f;
		}
	}

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
