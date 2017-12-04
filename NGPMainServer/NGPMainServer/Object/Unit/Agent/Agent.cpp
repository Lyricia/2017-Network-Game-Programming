#include "stdafx.h"

#include "Object\Brick\Brick.h"
#include "Object\Unit\Player\Player.h"

#include "Agent.h"



CAgent::CAgent(D2D_POINT_2F pt, D2D_RECT_F rc)
	: CUnit(pt, rc)
	, m_ptTargetPos({ 0,0 })
	, m_fBlockStunTimer(0)
	, m_bCollision(false)

{
	m_Tag = CObject::Type::Agent;
	m_next_change_dir_timer = rand() % 5 + 1;

	m_pStateMachine = new StateMachine<CAgent>(this);
	m_pStateMachine->SetGlobalState(AgentGlobalState::Instance());
	m_pStateMachine->SetCurrentState(Wandering::Instance());

}


CAgent::~CAgent()
{
}


void CAgent::Update(float fTimeElapsed)
{

	m_changedir_timer += fTimeElapsed;
	m_shoot_timer += fTimeElapsed;

	m_pStateMachine->Update(fTimeElapsed);

	m_fClosestTargetDistance = 99999999;

}
#ifdef WITH_RENDER_AGENT
void CAgent::Draw(ID2D1HwndRenderTarget * pd2dRenderTarget)
{

	D2D_MATRIX_3X2_F transform;
	pd2dRenderTarget->GetTransform(&transform);
	pd2dRenderTarget->SetTransform(m_mtxRotate*transform);

	auto bmpSize = m_bmpImage->GetSize();
	float aniWidthFactor = bmpSize.width / (float)m_szImg.width;
	float aniHeightFactor = bmpSize.height / (float)m_szImg.height;
	pd2dRenderTarget->DrawBitmap(
		m_bmpImage.Get()
		, m_rcSize + m_ptPos
		, 1.f
		, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR
		, RectF(
			aniWidthFactor * (int)m_ptCurrImg.x
			, aniHeightFactor * (int)m_ptCurrImg.y
			, aniWidthFactor * ((int)m_ptCurrImg.x + 1)
			, aniHeightFactor * ((int)m_ptCurrImg.y + 1)));

	pd2dRenderTarget->DrawBitmap(
		m_bmpWeaponImage.Get()
		, m_rcWeaponSize + m_ptPos);

	pd2dRenderTarget->SetTransform(transform);

}


void CAgent::RegisterResourceManager(shared_ptr<CResourceManager> resMng)
{
	m_pResMng = resMng;
	m_bmpImage = m_pResMng->GetImageRef(ResImgName::character_sheet);
	m_szImg = m_pResMng->GetImgLength(ResImgName::character_sheet);

	if (IsRectInvalid(m_rcSize))
	{
		auto sz = m_bmpImage->GetSize();
		sz.width /= m_szImg.width;
		sz.height /= m_szImg.height;
		m_rcSize = SizeToRect(sz);
	}

	m_bmpWeaponImage = m_pResMng->GetImage(ResImgName::gun);
	if (IsRectInvalid(m_rcWeaponSize))
		m_rcWeaponSize = SizeToRect(m_bmpWeaponImage->GetSize());

}
#endif


void CAgent::Collide(float atk)
{
	if (m_bCollision) return;
	m_bCollision = true;
	m_fHP -= atk;
	if (m_fHP < 0)
		m_fHP = 0;
}

void CAgent::Move(const D2D_POINT_2F & ptVelocity, float fTimeElapsed)
{
	m_ptVelocity += ptVelocity;
	float len = Length(m_ptVelocity);
	if (len > AGENT_MAX_VELOCITY)
		m_ptVelocity -= Normalize(m_ptVelocity) * (len - AGENT_MAX_VELOCITY);

	m_ptPos += m_ptVelocity * fTimeElapsed;
}

void CAgent::Reflection(const D2D_POINT_2F & ptReflect)
{
	if (ptReflect == Point2F())
		m_ptVelocity = m_ptVelocity * -AGENT_REFLACTION_FACTOR;
	else
		m_ptVelocity = ptReflect * Length(m_ptVelocity * AGENT_REFLACTION_FACTOR);
}

void CAgent::Stop()
{
	m_ptVelocity = Point2F();
}



CEffect* CAgent::Shoot()
{
	if (!m_bisShootable) return nullptr;

	m_shoot_timer = 0;
	m_bisShootable = false;

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
#ifdef WITH_RENDER_AGENT
		auto& rc = SizeToRect(SizeF(m_rcSize.right, m_rcSize.bottom));
		CEffect* effect = new CEffect(m_ptMuzzleEndPos, rc);
		auto& img = m_pResMng->GetImageRef(ResImgName::MagicBlast);
		auto& sz = m_pResMng->GetImgLength(ResImgName::MagicBlast);
		effect->RegisterEffectSprite(img, sz);
		return effect;
#endif
	}
	return nullptr;



}

void CAgent::RayCastingToShoot(std::vector<CObject*>& pvecObjects)
{


	for (float ratio = 0; ratio < 1; ratio += 0.01f)
	{
		D2D1_POINT_2F ptDevidedRay = m_ptPos + (m_ptDirection * AGENT_SHOOT_RANGE * ratio);
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
				continue;
			}
			case CObject::Type::Brick:
			{
				if (PtInRect(&(p->GetPos() + p->GetSize()), ptDevidedRay))
				{
					m_ptMuzzleEndPos = ptDevidedRay;
					m_pTarget = p;
					return;
				}
				continue;
			}
			}
		}
	}
	m_pTarget = nullptr;
	m_ptMuzzleEndPos = m_ptPos + (m_ptDirection * SHOOT_RANGE);









}

void CAgent::InterActionCheck(std::vector<CObject*>& pObjects)
{

	for (auto& p : pObjects)
	{

		if (this == p) continue;

		D2D_POINT_2F dir = Point2F();


		switch (p->GetTag())
		{
		case CObject::Type::Player:
		{
			//if (m_pTarget == nullptr)
			//{
			float distance = Length(GetPos() - p->GetPos());

			if (m_fClosestTargetDistance > distance)
			{
				m_fClosestTargetDistance = distance;
			}
			if (m_fClosestTargetDistance < AGENT_SHOOT_SIGHT)
			{
				m_ptTargetPos = p->GetPos();

				GetFSM()->ChangeState(Shooting::Instance());
			}
			//}
			continue;
		}
		case CObject::Type::Brick:
		{
			dir = Normalize(p->GetPos() - GetPos());
			if (PtInRect(
				&(p->GetSize() + p->GetPos())
				, GetPos() + (dir * p->GetSize().right))
				)
			{
				Reflection(-1.f * dir);
				SetDirection(Normalize(-1.f *dir));
			}

			continue;
		}
		}


	}

	RayCastingToShoot(pObjects);




}


void CAgent::SetObjectInfo(LPVOID info)
{
	ObjInfo* objinfo = static_cast<ObjInfo*>(info);
	float damage = m_fHP - objinfo->HP;

	//m_iGrenade = objinfo->Ammo.Granade;
	//m_iAmmo = objinfo->Ammo.GunAmmo;
	//m_iTurretKit = objinfo->Ammo.TurretKit;
	m_ptDirection = objinfo->Direction;
	m_ptPos = objinfo->Position;

	if (damage > 0) Collide(damage);
}

LPVOID CAgent::GetObjectInfo()
{
	ObjInfo* objinfo = new ObjInfo();

	objinfo->Ammo = { 0, 30, 0 };
	objinfo->Direction = m_ptDirection;
	objinfo->Position = m_ptPos;
	objinfo->HP = m_fHP;
	objinfo->ObjectID = m_Id;
	objinfo->ObjectType = OBJECTTYPE::Agent;
	objinfo->Collision = m_bCollision;

	return objinfo;
}

