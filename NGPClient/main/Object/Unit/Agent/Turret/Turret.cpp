#include "stdafx.h"
#include "Turret.h"
#include "Framework\ResourceManager\ResourceManager.h"
#include "Object\Brick\Brick.h"
#include "Object\Unit\Player\Player.h"
#include "Object\Effect\Effect.h"

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
	m_AgentType = CAgent::AgentType::Turret;
	m_next_change_dir_timer = rand() % 5 + 1;
	m_fHP = TURRET_MAX_HP;
	
	m_pParent = nullptr;
}

CTurret::~CTurret()
{
}

void CTurret::Shoot()
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
			player->Collide(TURRET_SHOOT_DAMAGE);
			player->Move(m_ptMuzzleDirection * PLAYER_VELOCITY);
			break;
		}
		case CObject::Type::Agent:
		{
			CAgent* agent = static_cast<CAgent*>(m_pTarget);
			agent->Collide(TURRET_SHOOT_DAMAGE);
			agent->Move(m_ptMuzzleDirection * PLAYER_VELOCITY);
			break;
		}
		case CObject::Type::Brick:
		{
			CBrick* brick = static_cast<CBrick*>(m_pTarget);
			brick->Collide(TURRET_SHOOT_DAMAGE);
			break;
		}
		}

	}
	return;

}

void CTurret::Shoot(const D2D_POINT_2F & ptHitPos)
{
	m_bShoot = true;
	m_ptMuzzleDirection = m_ptDirection;
	m_ptMuzzleStartPos = m_ptPos + m_ptMuzzleDirection * MUZZLE_OFFSET;
	m_ptMuzzleEndPos = ptHitPos;
}

void CTurret::Update(float fTimeElapsed)
{

	if (m_bShoot)
	{
		m_shoot_effect_timer += fTimeElapsed;
		if (m_shoot_effect_timer > TURRET_SHOOT_TIME)
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


}

void CTurret::Draw(ID2D1HwndRenderTarget * pd2dRenderTarget)
{
	D2D_MATRIX_3X2_F transform;
	pd2dRenderTarget->GetTransform(&transform);
	

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

	pd2dRenderTarget->SetTransform(m_mtxRotate*transform);

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
		m_pResMng->brLightYellow->SetOpacity(1 - (m_shoot_effect_timer / TURRET_SHOOT_TIME));
		pd2dRenderTarget->DrawLine(
			m_ptMuzzleStartPos
			, m_ptMuzzleEndPos
			, m_pResMng->brLightYellow.Get()
			, SHOOT_STROKE);
		m_pResMng->brLightYellow->SetOpacity(1.f);
	}
	std::wstring strTag(L"T");
	pd2dRenderTarget->DrawTextW(
		  strTag.c_str()
		, static_cast<UINT>(strTag.length())
		, m_pResMng->dwUITextFormat.Get()
		, m_rcSize + m_ptPos
		, m_pResMng->brDarkGray.Get());
}


void CTurret::RegisterResourceManager(std::shared_ptr<CResourceManager> resMng)
{
	m_pResMng = resMng;
	m_bmpImage = m_pResMng->GetImageRef(ResImgName::turret_base_sheet);
	m_szImg = m_pResMng->GetImgLength(ResImgName::turret_base_sheet);

	m_ptCurrImg.x = m_Id % m_szImg.width;

	if (IsRectInvalid(m_rcSize))
	{
		auto sz = m_bmpImage->GetSize();
		sz.width /= m_szImg.width;
		sz.height /= m_szImg.height;
		m_rcSize = SizeToRect(sz);
	}

	m_bmpWeaponImage = m_pResMng->GetImage(ResImgName::turret_barrel);
	if (IsRectInvalid(m_rcWeaponSize))
		m_rcWeaponSize = SizeToRect(m_bmpWeaponImage->GetSize());
}
