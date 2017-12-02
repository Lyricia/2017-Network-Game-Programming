#include "stdafx.h"
#include "Projectile.h"

CProjectile::CProjectile(D2D_POINT_2F pt, D2D_RECT_F rc)
	: CObject(pt, rc)
	, m_ptCurrImg(Point2F())
	, m_szImg(SizeU())
	, m_pResMng(nullptr)
	, m_ptVelocity(Point2F())
	, m_pParent(nullptr)
{
}

CProjectile::~CProjectile()
{
}

void CProjectile::Update(float fTimeElapsed)
{
}
