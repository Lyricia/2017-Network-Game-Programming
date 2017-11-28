#include "stdafx.h"
#include "Framework\IndRes\IndRes.h"
#include "Projectile.h"

CProjectile::CProjectile(D2D_POINT_2F pt, D2D_RECT_F rc)
	: CObject(pt, rc)
	, m_bmpImage(nullptr)
	, m_ptCurrImg(Point2F())
	, m_nSpriteImgWidth(0)
	, m_nSpriteImgHeight(0)
{
	m_Tag = CObject::Type::Projectile;
}

CProjectile::~CProjectile()
{
}

void CProjectile::Update(float fTimeElapsed)
{
}

void CProjectile::Draw(ID2D1HwndRenderTarget * pd2dRenderTarget)
{
}

void CProjectile::RegisterSpriteImage(CIndRes * pIndRes, ID2D1HwndRenderTarget * pd2dRenderTarget, path filename, D2D_POINT_2F ptLength)
{
}
