#pragma once
#include "Object\Object.h"

#define PROJECTILE_MAX_VELOCITY			500.f
#define PROJECTILE_VELOCITY				2000.f

#define PROJECTILE_FRICTIONAL_DRAG		1.5f
#define PROJECTILE_REFLACTION_FACTOR	0.9f

class CResourceManager;
class CProjectile : public CObject
{
public:
	CProjectile(D2D_POINT_2F pt, D2D_RECT_F rc = RectF());
	virtual ~CProjectile();

	virtual void Update(float fTimeElapsed) override;

	void SetParent(CObject* pParent) { m_pParent = pParent; }

protected:
	shared_ptr<CResourceManager>	m_pResMng;
	D2D1_SIZE_U						m_szImg;
	D2D1_POINT_2F					m_ptCurrImg;

	D2D_POINT_2F					m_ptVelocity;

	CObject*						m_pParent;
};