#include "stdafx.h"
#include "Framework\IndRes\IndRes.h"
#include "Unit.h"


CUnit::CUnit(D2D_POINT_2F pt, D2D_RECT_F rc)
	: CObject(pt, rc)
	, m_ptDirection(Point2F())
	, m_mtxRotate(Matrix3x2F::Identity())
	, m_bmpImage(nullptr)
	, m_fHP(0)
	, m_fBlockStunTimer(0)
{
}
CUnit::~CUnit()
{
}

void CUnit::Update(float fTimeElapsed)
{
}
void CUnit::Draw(ID2D1HwndRenderTarget * pd2dRenderTarget)
{
	D2D_MATRIX_3X2_F transform;
	pd2dRenderTarget->GetTransform(&transform);
	pd2dRenderTarget->SetTransform(m_mtxRotate*transform);
	pd2dRenderTarget->DrawBitmap(
		  m_bmpImage.Get()
		, m_rcSize + m_ptPos);
	pd2dRenderTarget->SetTransform(transform);
}

void CUnit::RegisterImage(
	  CIndRes*					pIndRes
	, ID2D1HwndRenderTarget*	pd2dRenderTarget
	, path						filename)
{
	LoadImageFromFile(
		  pIndRes->wicFactory()
		, pd2dRenderTarget
		, filename.c_str()
		, &m_bmpImage
	);

	if (IsRectInvalid(m_rcSize))
		m_rcSize = SizeToRect(m_bmpImage->GetSize());
}

void CUnit::RegisterImage(const ComPtr<ID2D1Bitmap1>& bmp)
{
	m_bmpImage = bmp;
	if (IsRectInvalid(m_rcSize))
		m_rcSize = SizeToRect(m_bmpImage->GetSize());
}

void CUnit::RegisterImage(ComPtr<ID2D1Bitmap1>&& bmp) noexcept
{
	m_bmpImage = move(bmp);
	if (IsRectInvalid(m_rcSize))
		m_rcSize = SizeToRect(m_bmpImage->GetSize());
}

void CUnit::LookAt(const D2D1_POINT_2F& target_pos)
{
	if(m_ptPos == target_pos) return;
	m_ptDirection = Normalize(target_pos - m_ptPos);
	float angle = -acosf(m_ptDirection*Point2F(1, 0)); 
	if (m_ptDirection.y > 0)
		angle = -angle;
	m_mtxRotate = Matrix3x2F::Rotation(
		RADIAN_TO_DEGREE(angle), m_ptPos);
}
