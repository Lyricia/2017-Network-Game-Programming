#include "stdafx.h"
#include "Player.h"
#include "Object\Brick\Brick.h"
#include "Object\Projectile\Grenade\Grenade.h"

CPlayer::CPlayer(D2D_POINT_2F pt, D2D_RECT_F rc)
	: CUnit(pt, rc)
	, m_pTarget(nullptr)
	, m_fBlockStunTimer(0)
	, m_fShootTimer(0)
	, m_fGrenadeTimer(0)
	, m_bCollision(false)
	, m_bShoot(true)
	, m_bReload(false)
	, m_bGrenade(false)
	, m_ptMoveDirection(Point2F())
	, m_ptVelocity(Point2F())
	, m_bMove(false)
	, m_iAmmo(MAX_AMMO)
	, m_iGrenade(MAX_GRENADE)
	, m_iTurretKit(MAX_TURRET)
{
	m_Tag = CObject::Type::Player;
	m_fHP = PLAYER_MAX_HP;
}
CPlayer::~CPlayer()
{
}

void CPlayer::Update(float fTimeElapsed)
{
	Move(PLAYER_VELOCITY * fTimeElapsed);
	m_ptPos += m_ptVelocity * fTimeElapsed;

	m_ptVelocity -= m_ptVelocity * fTimeElapsed * FRICTIONAL_DRAG;
	if (Length(m_ptVelocity) < FRICTIONAL_DRAG)
	{
		m_ptVelocity = Point2F();
		m_bMove = false;
	}

	if (m_bReload)
	{
		m_fShootTimer += fTimeElapsed;
		if (m_fShootTimer > RELOAD_TIME)
		{
			m_bReload = false;
			m_bShoot = false;
			m_fShootTimer = 0.f;
			m_iAmmo = MAX_AMMO;
		}
	}
	else if (m_bShoot)
	{
		m_fShootTimer += fTimeElapsed;
		if (m_fShootTimer > SHOOT_TIME)
		{
			m_bShoot = false;
			m_fShootTimer = 0.f;
		}
	}

	if (m_bGrenade)
	{
		m_fGrenadeTimer += fTimeElapsed;
		if (m_fGrenadeTimer > GRENADE_DELAY)
		{
			m_bGrenade = false;
			m_fGrenadeTimer = 0.f;
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
void CPlayer::Collide(float atk)
{
	if (m_bCollision) return;
	m_bCollision = true;
	m_fHP -= atk;
	if (m_fHP < 0)
		m_fHP = 0;
}

void CPlayer::SetObjectInfo(LPVOID info)
{
	ObjInfo* objinfo = static_cast<ObjInfo*>(info);
	float damage = m_fHP - objinfo->HP;

	m_iGrenade = objinfo->Ammo.Granade;
	m_iAmmo = objinfo->Ammo.GunAmmo;
	m_iTurretKit = objinfo->Ammo.TurretKit;
	m_ptDirection = objinfo->Direction;
	m_ptPos = objinfo->Position;

	if(damage > 0) Collide(damage);
}
LPVOID CPlayer::GetObjectInfo()
{
	ObjInfo* objinfo = new ObjInfo();

	objinfo->Ammo = { m_iGrenade, m_iAmmo, m_iTurretKit };
	objinfo->Direction = m_ptDirection;
	objinfo->Position = m_ptPos;
	objinfo->HP = m_fHP;
	objinfo->ObjectID = m_Id;
	objinfo->ObjectType = OBJECTTYPE::Player;
	objinfo->Collision = m_bCollision;

	return objinfo;
}

void CPlayer::SetMoveDirection(const D2D_POINT_2F & ptMoveDirection)
{
	m_ptMoveDirection = ptMoveDirection;
	m_bMove = true;
}

void CPlayer::Move(const D2D_POINT_2F& ptVelocity)
{
	m_ptVelocity += ptVelocity;
	float len = Length(m_ptVelocity);
	if (len > PLAYER_MAX_VELOCITY)
		m_ptVelocity -= Normalize(m_ptVelocity) * (len - PLAYER_MAX_VELOCITY);
}

void CPlayer::Move(float fSpeed)
{
	if (m_bMove) Move(m_ptMoveDirection * fSpeed);
}

void CPlayer::Reflection(const D2D_POINT_2F& ptDirReflect)
{
	if (ptDirReflect == Point2F())
		m_ptVelocity = m_ptVelocity * -REFLACTION_FACTOR;
	else
		m_ptVelocity = ptDirReflect * Length(m_ptVelocity * REFLACTION_FACTOR);
	SetMoveDirection(Normalize(m_ptVelocity));
}

void CPlayer::Stop()
{
	m_ptVelocity = Point2F();
	SetMoveDirection(Point2F());
}

CEffect* CPlayer::Shoot()
{
	if (m_bShoot) return nullptr;
	if (m_bReload) return nullptr;

	if (m_iAmmo == 0) m_bReload = true;
	else --m_iAmmo;
	
	m_bShoot = true;
	m_ptMuzzleDirection = m_ptDirection;
	m_ptMuzzleStartPos = m_ptPos + m_ptMuzzleDirection * MUZZLE_OFFSET;

	if (m_pTarget)
	{
		switch (m_pTarget->GetTag())
		{
		case CObject::Type::Player:
		{
			CPlayer* player = static_cast<CPlayer*>(m_pTarget);
			player->Collide(SHOOT_DAMAGE);
			player->Move(m_ptMuzzleDirection * PLAYER_VELOCITY);
			break;
		}
		case CObject::Type::Brick:
		{
			CBrick* brick = static_cast<CBrick*>(m_pTarget);
			brick->Collide(SHOOT_DAMAGE);
			break;
		}
		}
	}
	return nullptr;
}

void CPlayer::RayCastingToShoot(std::vector<CObject*>& pvecObjects)
{
	if (m_bShoot) return;
	if (m_bReload) return;
	for (float ratio = 0; ratio < 1; ratio += 0.01f)
	{
		D2D1_POINT_2F ptDevidedRay = m_ptPos + (m_ptDirection * SHOOT_RANGE * ratio);
		for (auto& p : pvecObjects)
		{
			switch (p->GetTag())
			{
			case CObject::Type::Player:
			{
				if (Length(p->GetPos() - ptDevidedRay) < p->GetSize().right)
				{
					m_ptMuzzleEndPos = ptDevidedRay;
					m_pTarget = p;
					return;
				}
				break;
			}
			case CObject::Type::Brick:
			{
				if (PtInRect(&(p->GetPos() + p->GetSize()), ptDevidedRay))
				{
					m_ptMuzzleEndPos = ptDevidedRay;
					m_pTarget = p;
					return;
				}
				break;
			}
			}
		}
	}
	m_pTarget = nullptr;
	m_ptMuzzleEndPos = m_ptPos + (m_ptDirection * SHOOT_RANGE);
}

CObject * CPlayer::GrenadeOut()
{
	if (m_bGrenade) return nullptr;
	if (m_iGrenade == 0) return nullptr;
	--m_iGrenade;
	m_bGrenade = true;

	CGrenade* grenade = new CGrenade(m_ptPos);
	grenade->SetParent(this);
	grenade->SetVelocity(m_ptDirection
		* Length(m_ptTargetPos - m_ptPos)
		* PROJECTILE_FRICTIONAL_DRAG);
	return grenade;
}



