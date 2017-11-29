#include "stdafx.h"
#include "GameWorld\ResourceManager\ResourceManager.h"
#include "Brick.h"

CBrick::CBrick(D2D_POINT_2F pt, D2D_RECT_F rc)
	: CObject(pt, rc)
	, m_bmpImage(nullptr)
	, m_ptCurrImg(Point2U())
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
}

void CBrick::RegisterResourceManager(shared_ptr<CResourceManager> resMng)
{
	m_pResMng = resMng;
	m_bmpImage = m_pResMng->GetImageRef(ResImgName::brick_Sheet);
	m_szImg = m_pResMng->GetImgLength(ResImgName::brick_Sheet);

	if (IsRectInvalid(m_rcSize))
	{
		auto sz = m_bmpImage->GetSize();
		sz.width /= m_szImg.width;
		sz.height /= m_szImg.height;
		m_rcSize = SizeToRect(sz);
	}
}

void CBrick::Collide(float atk)
{
	if (m_bCollision) return;
	m_fHP -= atk;
	m_bCollision = true;
	UpdateImg();
	if(m_ptCurrImg.x < m_szImg.width - 1)
		m_ptCurrImg.x += 1;
	if (m_fHP < 0)
		m_fHP = 0;
}

void CBrick::UpdateImg()
{
	m_ptCurrImg.x = (m_szImg.width - 1) *
		(1.f - (m_fHP / BRICK_MAX_HP));
}
