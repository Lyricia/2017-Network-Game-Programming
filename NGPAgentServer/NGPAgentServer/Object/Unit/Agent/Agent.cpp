#include "stdafx.h"
#ifdef WITH_RENDER_AGENT
#include "Framework\IndRes\IndRes.h"
#include "Framework\ResourceManager\ResourceManager.h"
#endif
#include "Agent.h"



CAgent::CAgent(D2D_POINT_2F pt, D2D_RECT_F rc)
	: CUnit(pt, rc)
#ifdef WITH_RENDER_AGENT
	, m_bmpImage(nullptr)
	, m_bmpWeaponImage(nullptr)
#endif
	, m_pTarget(nullptr)
#ifdef WITH_RENDER_AGENT
	, m_rcWeaponSize(RectF())
#endif
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
}

void CAgent::Draw(ID2D1HwndRenderTarget * pd2dRenderTarget)
{
#ifdef WITH_RENDER_AGENT
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

#endif

}


void CAgent::RegisterResourceManager(std::shared_ptr<CResourceManager> resMng)
{
#ifdef WITH_RENDER_AGENT
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
#endif

}

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



void CAgent::Shoot()
{
	if(m_shoot_timer > AGENT_SHOOT_TIME)
			m_shoot_timer = 0;

}

void CAgent::RayCastingToShoot(std::vector<CObject*>& pvecObjects)
{
}

