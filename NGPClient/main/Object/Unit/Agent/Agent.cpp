#include "stdafx.h"
#include "Framework\ResourceManager\ResourceManager.h"
#include "Object\Brick\Brick.h"
#include "Object\Unit\Player\Player.h"

#include "Object\Effect\Effect.h"
#include "Agent.h"



CAgent::CAgent(D2D_POINT_2F pt, D2D_RECT_F rc)
	: CUnit(pt, rc)
	, m_pTarget(nullptr)
	, m_ptTargetPos({ 0,0 })
	, m_fBlockStunTimer(0)
	, m_bCollision(false)
	, m_bShoot(true)
	, m_ptVelocity(Point2F())

{
	m_Tag = CObject::Type::Agent;
	m_AgentType = CAgent::AgentType::Bot;
	m_next_change_dir_timer = rand() % 5 + 1;
	m_fHP = PLAYER_MAX_HP;

}


CAgent::~CAgent()
{
}


void CAgent::Update(float fTimeElapsed)
{

	m_ptPos += m_ptVelocity * fTimeElapsed;

	m_ptVelocity -= m_ptVelocity * fTimeElapsed * FRICTIONAL_DRAG;
	if (Length(m_ptVelocity) < FRICTIONAL_DRAG)
		m_ptVelocity = Point2F();

	
	if (m_bShoot)
	{
		m_shoot_effect_timer += fTimeElapsed;
		if (m_shoot_effect_timer > SHOOT_TIME)
		{
			m_bShoot = false;
			m_shoot_effect_timer = 0.f;
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


	m_changedir_timer += fTimeElapsed;

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

	// 체력바
	m_pResMng->brRed->SetOpacity(0.8f);
	pd2dRenderTarget->FillRectangle(
		RectF(m_rcSize.left
			, m_rcSize.top - (m_rcSize.bottom - m_rcSize.top) * 0.1f
			, m_rcSize.left + (m_rcSize.right - m_rcSize.left) * (m_fHP / PLAYER_MAX_HP)
			, m_rcSize.top)
		+ m_ptPos
		, m_pResMng->brRed.Get());
	m_pResMng->brRed->SetOpacity(1.0f);

	// 체력바 테두리
	pd2dRenderTarget->DrawRectangle(
		RectF(m_rcSize.left
			, m_rcSize.top - (m_rcSize.bottom - m_rcSize.top) * 0.1f
			, m_rcSize.left + (m_rcSize.right - m_rcSize.left)
			, m_rcSize.top)
		+ m_ptPos
		, m_pResMng->brWhite.Get());


	if (m_bShoot)
	{
		m_pResMng->brLightYellow->SetOpacity(1 - (m_shoot_effect_timer / SHOOT_TIME));
		pd2dRenderTarget->DrawLine(
			m_ptMuzzleStartPos
			, m_ptMuzzleEndPos
			, m_pResMng->brLightYellow.Get()
			, SHOOT_STROKE);
		m_pResMng->brLightYellow->SetOpacity(1.f);
	}
	
	std::wstring strTag(L"A");
	pd2dRenderTarget->DrawTextW(
		  strTag.c_str()
		, static_cast<UINT>(strTag.length())
		, m_pResMng->dwUITextFormat.Get()
		, m_rcSize + m_ptPos
		, m_pResMng->brDarkGray.Get());
}


void CAgent::RegisterResourceManager(shared_ptr<CResourceManager> resMng)
{
	m_pResMng = resMng;
	m_bmpImage = m_pResMng->GetImageRef(ResImgName::character_sheet);
	m_szImg = m_pResMng->GetImgLength(ResImgName::character_sheet);

	m_ptCurrImg.x = m_Id % m_szImg.width;

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

void CAgent::Move(const D2D_POINT_2F& ptVelocity)
{
	m_ptVelocity += ptVelocity;
	float len = Length(m_ptVelocity);
	if (len > PLAYER_MAX_VELOCITY)
		m_ptVelocity -= Normalize(m_ptVelocity) * (len - PLAYER_MAX_VELOCITY);
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



void CAgent::Shoot()
{

	m_shoot_effect_timer = 0;
	m_bisShootable = false;
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
		case CObject::Type::Agent:
		{
			CAgent* agent = static_cast<CAgent*>(m_pTarget);
			agent->Collide(SHOOT_DAMAGE);
			agent->Move(m_ptMuzzleDirection * PLAYER_VELOCITY);
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
	return;



}


void CAgent::Shoot(const D2D_POINT_2F & ptHitPos)
{
	m_bShoot = true;
	m_ptMuzzleDirection = m_ptDirection;
	m_ptMuzzleStartPos = m_ptPos + m_ptMuzzleDirection * MUZZLE_OFFSET;
	m_ptMuzzleEndPos = ptHitPos;
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


		if (p->GetTag() == CObject::Type::Player)
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
	m_fHP = objinfo->HP;

	float angle = -acosf(m_ptDirection*Point2F(1, 0));
	if (m_ptDirection.y > 0)
		angle = -angle;
	m_mtxRotate = Matrix3x2F::Rotation(
		RADIAN_TO_DEGREE(angle), m_ptPos);
}

