#include "stdafx.h"
#include "Grenade.h"
#include "Object\Unit\Player\Player.h"
#include "Object\Unit\Agent\Agent.h"
#include "Object\Brick\Brick.h"

CGrenade::CGrenade(D2D_POINT_2F pt, D2D_RECT_F rc)
	: CProjectile(pt, rc)
	, m_bExplosion(false)
{
	m_Tag = CObject::Type::Grenade;
	m_szImg = SizeU(26, 1);
}
CGrenade::~CGrenade()
{
}

void CGrenade::Update(float fTimeElapsed)
{
	if (m_bExplosion) return;

	m_ptPos += m_ptVelocity * fTimeElapsed;

	m_ptVelocity -= m_ptVelocity * fTimeElapsed * PROJECTILE_FRICTIONAL_DRAG;

	if (Length(m_ptVelocity) < PROJECTILE_FRICTIONAL_DRAG)
		m_ptVelocity = Point2F();

	m_ptCurrImg.x += fTimeElapsed *
		((float)m_szImg.width / GRENADE_EXPLOSION_DELAY);
	if (m_ptCurrImg.x >= (float)m_szImg.width)
		m_bExplosion = true;
}

void CGrenade::SetObjectInfo(LPVOID info)
{
	ObjInfo* objinfo = static_cast<ObjInfo*>(info);

	m_ptVelocity = objinfo->Velocity;
	m_ptPos = objinfo->Position;
	m_ptCurrImg = objinfo->Grenade_ptCurrImg;
}

LPVOID CGrenade::GetObjectInfo()
{
	ObjInfo* objinfo = new ObjInfo();

	objinfo->ObjectID = m_Id;
	objinfo->ParentID = m_pParent->GetID();
	objinfo->Velocity = m_ptVelocity;
	objinfo->Position = m_ptPos;
	objinfo->ObjectType = OBJECTTYPE::Grenade;
	objinfo->Grenade_ptCurrImg = m_ptCurrImg;

	return objinfo;
}

void CGrenade::Reflection(const D2D_POINT_2F & ptDirReflect)
{
	if (ptDirReflect == Point2F())
		m_ptVelocity = m_ptVelocity * -PROJECTILE_REFLACTION_FACTOR;
	else
		m_ptVelocity = ptDirReflect * Length(m_ptVelocity * PROJECTILE_REFLACTION_FACTOR);
}

void CGrenade::Explosion(std::vector<CObject*>& pvecObjects)
{
	if (!m_bExplosion) return;
	for (auto& p : pvecObjects)
	{
		switch (p->GetTag())
		{
		case CObject::Type::Player:
		{
			if (Length(p->GetPos() - m_ptPos) < GRENADE_EXPLOSION_RANGE)
			{
				CPlayer* player = static_cast<CPlayer*>(p);
				player->Collide(GRENADE_EXPLOSION_ATK);
				player->Move(Normalize(p->GetPos() - m_ptPos) * GRENADE_EXPLOSION_FORCE);
			}
			break;
		}
		case CObject::Type::Agent:
		{
			if (Length(p->GetPos() - m_ptPos) < GRENADE_EXPLOSION_RANGE)
			{
				CAgent* agent = static_cast<CAgent*>(p);
				agent->Collide(GRENADE_EXPLOSION_ATK);
				agent->Move(Normalize(p->GetPos() - m_ptPos) * GRENADE_EXPLOSION_FORCE);
			}
			break;
		}
		case CObject::Type::Brick:
		{
			if (Length(p->GetPos() - m_ptPos) < GRENADE_EXPLOSION_RANGE)
			{
				CBrick* brick = static_cast<CBrick*>(p);
				brick->Collide(GRENADE_EXPLOSION_ATK);
				break;
			}
			break;
		}
		}
	}
}
