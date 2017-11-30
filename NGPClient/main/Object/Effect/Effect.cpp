#include "stdafx.h"
#include "Effect.h"

CEffect::CEffect(D2D_POINT_2F pt, D2D_RECT_F rc)
	: CObject(pt, rc)
	, m_bmpImage(nullptr)
	, m_szImg(SizeU())
	, m_ptCurrImg(Point2F())
	, m_fEffectTime(DEFAULT_EFFECT_TIME)
	, m_bFinished(false)
{
	m_Tag = CObject::Type::Effect;
	RejectID();
}
CEffect::~CEffect()
{
}

void CEffect::Update(float fTimeElapsed)
{
	if (m_bFinished) return;

	//m_ptCurrImg.x += fTimeElapsed *
	//	(((float)m_szImg.width * (float)m_szImg.height) / m_fEffectTime);
	//if (m_ptCurrImg.x >= (float)m_szImg.width)
	//{
	//	m_ptCurrImg.x = 0;
	//	++m_ptCurrImg.y;
	//}
	//if (m_ptCurrImg.y >= (float)m_szImg.height)
	//	m_bFinished = true;

	m_ptCurrImg.x += fTimeElapsed * ((float)m_szImg.width  / m_fEffectTime);
	if (m_ptCurrImg.x >= (float)m_szImg.width)
		m_bFinished = true;
}

void CEffect::Draw(ID2D1HwndRenderTarget * pd2dRenderTarget)
{
	if (m_bFinished) return;

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

void CEffect::RegisterEffectSprite(
	  ComPtr<ID2D1Bitmap1> bmpImg
	, const D2D1_SIZE_U & sz)
{
	m_bmpImage = bmpImg;
	m_szImg = sz;
	m_ptCurrImg.y = m_szImg.height - 1;
}
