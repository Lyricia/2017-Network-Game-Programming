#include "stdafx.h"
#include "Framework\IndRes\IndRes.h"
#include "Brick.h"

CBrick::CBrick(D2D_POINT_2F pt, D2D_RECT_F rc)
{
}

CBrick::~CBrick()
{
}

void CBrick::Update(float fTimeElapsed)
{
}

void CBrick::Draw(ID2D1HwndRenderTarget * pd2dRenderTarget)
{
	pd2dRenderTarget->DrawBitmap(
		m_bmpImage.Get()
		, m_rcSize + m_ptPos);
}

void CBrick::RegisterImage(CIndRes * pIndRes, ID2D1HwndRenderTarget * pd2dRenderTarget, path filename)
{
	LoadImageFromFile(
		pIndRes->wicFactory()
		, pd2dRenderTarget
		, filename.c_str()
		, &m_bmpImage
	);

	if (IsRectValid(m_rcSize))
		m_rcSize = SizeToRect(m_bmpImage->GetSize());
}
void CBrick::RegisterImage(const ComPtr<ID2D1Bitmap1>& bmp)
{
	m_bmpImage = bmp;
	if (IsRectValid(m_rcSize))
		m_rcSize = SizeToRect(m_bmpImage->GetSize());
}
void CBrick::RegisterImage(ComPtr<ID2D1Bitmap1>&& bmp) noexcept
{
	m_bmpImage = move(bmp);
	if (IsRectValid(m_rcSize))
		m_rcSize = SizeToRect(m_bmpImage->GetSize());
}
