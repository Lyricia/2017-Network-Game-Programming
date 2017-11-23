#include "stdafx.h"
#include "Framework\IndRes\IndRes.h"
#include "Player.h"
#include "Object\Brick\Brick.h"

CPlayer::CPlayer(D2D_POINT_2F pt, D2D_RECT_F rc)
	: CUnit(pt, rc)
	, m_bmpImage(nullptr)
	, m_bmpWeaponImage(nullptr)
	, m_pd2dLineBrush(nullptr)
	, m_rcWeaponSize(RectF())
	, m_fBlockStunTimer(0)
	, m_bCollision(false)
	, m_fShootTimer(0)
	, m_bShoot(true)
{
}
CPlayer::~CPlayer()
{
}

void CPlayer::Update(float fTimeElapsed)
{
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

void CPlayer::Draw(ID2D1HwndRenderTarget * pd2dRenderTarget)
{
	D2D_MATRIX_3X2_F transform;
	pd2dRenderTarget->GetTransform(&transform);
	pd2dRenderTarget->SetTransform(m_mtxRotate*transform);

	auto bmpSize = m_bmpImage->GetSize();
	float aniWidthFactor = bmpSize.width / (float)m_nSpriteImgWidth;
	float aniHeightFactor = bmpSize.height / (float)m_nSpriteImgHeight;
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

	if (m_bShoot)
	{
		if (!m_pd2dLineBrush)
			pd2dRenderTarget->CreateSolidColorBrush(
				ColorF{ ColorF::LightYellow }
				, &m_pd2dLineBrush);

		m_pd2dLineBrush->SetOpacity(1 - (m_fShootTimer /SHOOT_TIME));
		pd2dRenderTarget->DrawLine(
			m_ptMuzzleStartPos
			, m_ptMuzzleEndPos
			, m_pd2dLineBrush.Get()
			, SHOOT_STROKE);
	}
}

void CPlayer::RegisterImage(CIndRes * pIndRes, ID2D1HwndRenderTarget * pd2dRenderTarget, path filename)
{
	LoadImageFromFile(
		  pIndRes->wicFactory()
		, pd2dRenderTarget
		, filename.c_str()
		, &m_bmpWeaponImage
	);

	if (IsRectInvalid(m_rcWeaponSize))
		m_rcWeaponSize = SizeToRect(m_bmpWeaponImage->GetSize());
}

void CPlayer::RegisterSpriteImage(CIndRes * pIndRes, ID2D1HwndRenderTarget * pd2dRenderTarget, path filename, D2D_POINT_2F ptLength)
{
	m_nSpriteImgWidth = ptLength.x;
	m_nSpriteImgHeight = ptLength.y;

	LoadImageFromFile(
		pIndRes->wicFactory()
		, pd2dRenderTarget
		, filename.c_str()
		, &m_bmpImage
	);
	if (IsRectInvalid(m_rcSize))
	{
		auto sz = m_bmpImage->GetSize();
		sz.width /= ptLength.x;
		sz.height /= ptLength.y;
		m_rcSize = SizeToRect(sz);
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

void CPlayer::Move(const D2D_POINT_2F & pt)
{
	m_ptPos = m_ptPos + pt;
}
void CPlayer::Move(float x, float y)
{
	m_ptPos = m_ptPos + Point2F(x, y);
}

void CPlayer::Shoot()
{
	if (m_bShoot) return;
	m_bShoot = true;
	m_ptMuzzleDirection = m_ptDirection;
	m_ptMuzzleStartPos = m_ptPos + m_ptMuzzleDirection * MUZZLE_OFFSET;
	if (m_pTarget)
	{
		CUnit* unit = dynamic_cast<CUnit*>(m_pTarget);
		if (unit)
			unit->Collide(SHOOT_DAMAGE);
		else
		{
			CBrick* brick = dynamic_cast<CBrick*>(m_pTarget);
			if (brick)
				brick->Collide(SHOOT_DAMAGE);
		}
	}
}

void CPlayer::RayCastingToShoot(std::vector<CObject*>& pvecObjects)
{
	if (m_bShoot) return;
	for (float ratio = 0; ratio < 1; ratio += 0.01f)
	{
		D2D1_POINT_2F ptDevidedRay = m_ptPos + (m_ptDirection * SHOOT_RANGE * ratio);
		for (auto& p : pvecObjects)
		{
			CUnit* unit = dynamic_cast<CUnit*>(p);
			if (unit)
			{
				if (Length(unit->GetPos() - ptDevidedRay) < unit->GetSize().right)
				{
					m_ptMuzzleEndPos = ptDevidedRay;
					m_pTarget = p;
					return;
				}
				continue;
			}
			else
			{
				CBrick* brick = dynamic_cast<CBrick*>(p);
				if (brick)
				{
					if (PtInRect(&(brick->GetPos() + brick->GetSize()), ptDevidedRay))
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
