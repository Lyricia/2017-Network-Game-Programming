#pragma once
#include "Object/Unit/Unit.h"

#define PLAYER_MAX_HP			100.0
#define BLOCK_STUN_TIME			0.2

#define SHOOT_TIME				0.3
#define SHOOT_RANGE				1000.f
#define SHOOT_DAMAGE			10.f
#define SHOOT_STROKE			6.0f // 레이저 두께
#define RELOAD_TIME				3.0f
#define GRENADE_DELAY			1.0f
#define MUZZLE_OFFSET			64.f

#define MAX_AMMO				30
#define MAX_GRENADE				3
#define MAX_TURRET				1

#define PLAYER_MAX_VELOCITY		400.f
#define PLAYER_VELOCITY			2000.f

#define FRICTIONAL_DRAG			2.5f
#define REFLACTION_FACTOR		0.8f

class CEffect;
class CResourceManager;
class CPlayer : public CUnit
{
public:
	CPlayer(D2D_POINT_2F pt, D2D_RECT_F rc = RectF());
	virtual ~CPlayer();

	virtual void Update(float fTimeElapsed) override;
	virtual void Draw(ID2D1HwndRenderTarget* pd2dRenderTarget) override;
	virtual void DrawUI(ID2D1HwndRenderTarget* pd2dRenderTarget, float fScaleFactor);

	virtual void RegisterResourceManager(shared_ptr<CResourceManager> resMng);

	virtual void Collide(float atk) override;

	void Move(const D2D_POINT_2F& ptVelocity);
	void Reflection(const D2D_POINT_2F& ptDirReflect = Point2F());
	void Stop();
	CEffect* Shoot();
	void RayCastingToShoot(std::vector<CObject*>& pvecObjects);

	CObject* GrenadeOut();

private:
	ComPtr<ID2D1Bitmap1>			m_bmpImage;

	ComPtr<ID2D1Bitmap1>			m_bmpWeaponImage;
	D2D_RECT_F						m_rcWeaponSize;

	float							m_fBlockStunTimer;
	bool							m_bCollision;

	D2D_POINT_2F					m_ptVelocity;

	CObject*						m_pTarget;

	D2D_POINT_2F					m_ptMuzzleDirection;
	D2D_POINT_2F					m_ptMuzzleStartPos;
	D2D_POINT_2F					m_ptMuzzleEndPos;

	float							m_fShootTimer;
	bool							m_bShoot;
	bool							m_bReload;

	float							m_fGrenadeTimer;
	bool							m_bGrenade;

	UINT							m_iAmmo;
	UINT							m_iGrenade;
	UINT							m_iTurretKit;
};
