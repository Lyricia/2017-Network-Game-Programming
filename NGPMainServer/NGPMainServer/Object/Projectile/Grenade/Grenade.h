#pragma once
#include "Object\Projectile\Projectile.h"

#define GRENADE_EXPLOSION_RANGE 300.f
#define GRENADE_EXPLOSION_FORCE 5000.f
#define GRENADE_EXPLOSION_ATK	50.f
#define GRENADE_EXPLOSION_DELAY	3.f

class CGrenade : public CProjectile
{
public:
	CGrenade(D2D_POINT_2F pt, D2D_RECT_F rc = RectF());
	virtual ~CGrenade();

	virtual void Update(float fTimeElapsed) override;

	virtual void SetObjectInfo(LPVOID info) override;
	virtual LPVOID GetObjectInfo() override;

	void SetVelocity(const D2D_POINT_2F& ptVelocity) { m_ptVelocity = ptVelocity; }
	void Reflection(const D2D_POINT_2F& ptDirReflect = Point2F());
	void Explosion(std::vector<CObject*>& pvecObjects);

	bool IsExplosion() const { return m_bExplosion; }

private:
	bool							m_bExplosion;
};