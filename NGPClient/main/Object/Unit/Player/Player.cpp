#include "stdafx.h"
#include "Framework\IndRes\IndRes.h"
#include "Player.h"

CPlayer::CPlayer(D2D_POINT_2F pt, D2D_RECT_F rc)
	: CUnit(pt, rc)
	, m_rcWeaponSize(RectF())
{
}

CPlayer::~CPlayer()
{
}

void CPlayer::Update(float fTimeElapsed)
{
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
