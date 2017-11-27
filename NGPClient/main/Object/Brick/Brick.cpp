#include "stdafx.h"
#include "Framework\IndRes\IndRes.h"
#include "Brick.h"

CBrick::CBrick(D2D_POINT_2F pt, D2D_RECT_F rc)
	: CObject(pt, rc)
	, m_bmpImage(nullptr)
	, m_ptCurrImg(Point2F())
	, m_nSpriteImgWidth(0)
	, m_nSpriteImgHeight(0)
	, m_fHP(BRICK_MAX_HP)
	, m_fBlockStunTimer(0)
{
	m_Tag = CObject::Type::Brick;
}

CBrick::~CBrick()
{
}

void CBrick::Update(float fTimeElapsed)
{
	if (m_bCollision)
	{
		m_fBlockStunTimer += fTimeElapsed;
		if (m_fBlockStunTimer > BLOCK_STUN_TIME)
		{
			UpdateImg();
			m_bCollision = false;
			m_fBlockStunTimer = 0.f;
		}
	}
	
}

void CBrick::Draw(ID2D1HwndRenderTarget * pd2dRenderTarget)
{
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
}

void CBrick::RegisterSpriteImage(
	CIndRes * pIndRes
	, ID2D1HwndRenderTarget * pd2dRenderTarget
	, path filename
	, D2D_POINT_2F ptLength)
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

void CBrick::Collide(float atk)
{
	if (m_bCollision) return;
	m_fHP -= atk;
	m_bCollision = true;
	UpdateImg();
	if(m_ptCurrImg.x < m_nSpriteImgWidth - 1)
		m_ptCurrImg.x += 1;
	if (m_fHP < 0)
		m_fHP = 0;
}

void CBrick::UpdateImg()
{
	m_ptCurrImg.x = (m_nSpriteImgWidth - 1) * 
		(1.f - (m_fHP / BRICK_MAX_HP));
}
