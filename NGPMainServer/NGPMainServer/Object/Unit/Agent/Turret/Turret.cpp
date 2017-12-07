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


}

void CTurret::Move(const D2D_POINT_2F& ptVelocity)
{
	
}

void CTurret::Move(const D2D_POINT_2F & ptVelocity, float fTimeElapsed)
{
	
}
