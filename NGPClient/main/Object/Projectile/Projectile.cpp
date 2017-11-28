#include "stdafx.h"
#include "Framework\ResourceManager\ResourceManager.h"
#include "Projectile.h"

CProjectile::CProjectile(D2D_POINT_2F pt, D2D_RECT_F rc)
	: CObject(pt, rc)
	, m_ptCurrImg(Point2U())
	, m_szImg(SizeU())
	, m_pResMng(nullptr)
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

void CProjectile::RegisterResourceManager(shared_ptr<CResourceManager> resMng)
{
	m_pResMng = resMng;
}
