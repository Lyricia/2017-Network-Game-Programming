#include "stdafx.h"
#include "Framework\ResourceManager\ResourceManager.h"
#include "Grenade.h"
#include "Object\Unit\Player\Player.h"
#include "Object\Brick\Brick.h"

CGrenade::CGrenade(D2D_POINT_2F pt, D2D_RECT_F rc)
	: CProjectile(pt, rc)
	, m_bExplosion(false)
{
	m_Tag = CObject::Type::Grenade;
}
CGrenade::~CGrenade()
{
}

void CGrenade::Update(float fTimeElapsed)
{
	if (m_bExplosion) return;

	m_ptPos += m_ptVelocity * fTimeElapsed;

	m_ptVelocity -= m_ptVelocity * fTimeElapsed * PROJECTILE_FRICTIONAL_DRAG;

	if (Length(m_ptVelocity) < PROJECTILE_FRICTIONAL_DRAG)
		m_ptVelocity = Point2F();

	m_ptCurrImg.x += fTimeElapsed *
		((float)m_szImg.width / GRENADE_EXPLOSION_DELAY);
	if (m_ptCurrImg.x >= (float)m_szImg.width)
		m_bExplosion = true;
}

void CGrenade::Draw(ID2D1HwndRenderTarget * pd2dRenderTarget)
{
	if (m_bExplosion) return;
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

void CGrenade::RegisterResourceManager(shared_ptr<CResourceManager> resMng)
{
	m_pResMng = resMng;
	m_bmpImage = m_pResMng->GetImageRef(ResImgName::grenade);
	m_szImg = m_pResMng->GetImgLength(ResImgName::grenade);

	if (IsRectInvalid(m_rcSize))
	{
		auto sz = m_bmpImage->GetSize();
		sz.width /= m_szImg.width;
		sz.height /= m_szImg.height;
		m_rcSize = SizeToRect(sz);
	}
}

void CGrenade::Reflection(const D2D_POINT_2F & ptDirReflect)
{
	if (ptDirReflect == Point2F())
		m_ptVelocity = m_ptVelocity * -PROJECTILE_REFLACTION_FACTOR;
	else
		m_ptVelocity = ptDirReflect * Length(m_ptVelocity * PROJECTILE_REFLACTION_FACTOR);
}

void CGrenade::Explosion(std::vector<CObject*>& pvecObjects)
{
	if (!m_bExplosion) return;
	for (auto& p : pvecObjects)
	{
		switch (p->GetTag())
		{
		case CObject::Type::Player:
		{
			if (Length(p->GetPos() - m_ptPos) < GRENADE_EXPLOSION_RANGE)
			{
				CPlayer* player = static_cast<CPlayer*>(p);
				player->Collide(GRENADE_EXPLOSION_ATK);
				player->Move(Normalize(p->GetPos() - m_ptPos) * GRENADE_EXPLOSION_FORCE);
			}
			break;
		}
		case CObject::Type::Brick:
		{
			if (Length(p->GetPos() - m_ptPos) < GRENADE_EXPLOSION_RANGE)
			{
				CBrick* brick = static_cast<CBrick*>(p);
				brick->Collide(GRENADE_EXPLOSION_ATK);
				break;
			}
			break;
		}
		}
	}
}
