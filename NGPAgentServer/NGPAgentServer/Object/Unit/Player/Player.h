#pragma once
#include "Object/Unit/Unit.h"

#define PLAYER_MAX_HP			100.0
#define BLOCK_STUN_TIME			0.2

#define SHOOT_TIME				0.4
#define SHOOT_RANGE				1000.f
#define SHOOT_DAMAGE			10.f
#define SHOOT_STROKE			6.0f // 레이저 두께
#define RELOAD_TIME				3.5f
#define GRENADE_DELAY			1.5f
#define MUZZLE_OFFSET			64.f

#define MAX_AMMO				30
#define MAX_GRENADE				3
#define MAX_TURRET				1

#define PLAYER_MAX_VELOCITY		400.f
#define PLAYER_VELOCITY			2000.f

#define FRICTIONAL_DRAG			2.5f
#define REFLACTION_FACTOR		0.8f

class CClient;
class CPlayer : public CUnit
{
public:
	CPlayer(D2D_POINT_2F pt, D2D_RECT_F rc = RectF());
	virtual ~CPlayer();

	virtual void Update(float fTimeElapsed) override;

	virtual void Collide(float atk) override;
	virtual void SetObjectInfo(LPVOID info) override;

	void Move(const D2D_POINT_2F& ptVelocity);

	void Reflection(const D2D_POINT_2F& ptDirReflect = Point2F());

	void Stop();

	void Shoot(const D2D_POINT_2F& ptHitPos);
	void Shoot();

	void RayCastingToShoot(std::vector<CObject*>& pvecObjects);

	CObject* GrenadeOut();

private:
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
