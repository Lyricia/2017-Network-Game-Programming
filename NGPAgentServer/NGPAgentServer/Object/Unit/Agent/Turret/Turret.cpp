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

	D2D_POINT_2F dir{ -10 + rand() % 20,-10 + rand() % 20 };
	SetDirection(Normalize(dir));
	SetMoveDirection(Normalize(dir));

	m_pStateMachine = new StateMachine<CAgent>(this);
	m_pStateMachine->SetGlobalState(AgentGlobalState::Instance());
	m_pStateMachine->SetCurrentState(Wandering::Instance());
}

CTurret::~CTurret()
{
}

void CTurret::Update(float fTimeElapsed)
{

	m_shoot_timer += fTimeElapsed;
	m_changedir_timer += fTimeElapsed;
}

void CTurret::Move(const D2D_POINT_2F & ptVelocity)
{
}

void CTurret::Move(const D2D_POINT_2F & ptVelocity, float fTimeElapsed)
{
}

void CTurret::Move(RoomInfo * pRoom, const D2D_POINT_2F & ptMoveDirection)
{
}

void CTurret::InterActionCheck(std::vector<CObject*>& pObjects)
{

	GetFSM()->ChangeState(Wandering::Instance());
	m_fClosestTargetDistance = 9999999;

	for (auto& p : pObjects)
	{
		if (this == p) continue;
		if (m_pParent->GetID() == p->GetID()) continue;

		if (p->GetTag() == CObject::Type::Player || p->GetTag() == CObject::Type::Agent)
		{
			
			float distance = Length(GetPos() - p->GetPos());

			if (m_fClosestTargetDistance > distance)
			{
				m_fClosestTargetDistance = distance;
				if (m_fClosestTargetDistance < AGENT_SHOOT_SIGHT)
				{
					m_ptTargetPos = p->GetPos();
					m_ptTargetPos.x += (-5 + rand() % 10);
					m_ptTargetPos.y += (-5 + rand() % 10);
					D2D_POINT_2F dir = m_ptTargetPos - m_ptPos;
					m_ptDirection = Normalize(dir);
					GetFSM()->ChangeState(Shooting::Instance());
				}

			}

		}

	}


}
