#pragma once
#include "Object/Unit/Unit.h"

#define Player_MAX_HP	100.0
#define BLOCK_STUN_TIME 0.2
#define SHOOT_TIME		0.3
#define SHOOT_RANGE		1000.f
#define SHOOT_DAMAGE	10.f
#define SHOOT_STROKE	6.0f // 레이저 두께
#define MUZZLE_OFFSET	64.f

class CIndRes;
class CPlayer : public CUnit
{
public:
	CPlayer(D2D_POINT_2F pt, D2D_RECT_F rc = RectF());
	virtual ~CPlayer();

	virtual void Update(float fTimeElapsed) override;
	virtual void Draw(ID2D1HwndRenderTarget* pd2dRenderTarget) override;

	virtual void RegisterImage(
		CIndRes*					pIndRes
		, ID2D1HwndRenderTarget*	pd2dRenderTarget
		, path						filename);
	virtual void RegisterSpriteImage(
		CIndRes*					pIndRes
		, ID2D1HwndRenderTarget*	pd2dRenderTarget
		, path						filename
		, D2D_POINT_2F				ptLength);

	virtual void Collide(float atk) override;

	void Move(const D2D_POINT_2F& pt);
	void Move(float x, float y);
	void Shoot();
	void RayCastingToShoot(std::vector<CObject*>& pvecObjects);

	void SetHP(float hp) { m_fHP = hp; }

private:
	ComPtr<ID2D1Bitmap1>			m_bmpImage;
	D2D_POINT_2F					m_ptCurrImg;
	UINT							m_nSpriteImgWidth;
	UINT							m_nSpriteImgHeight;

	ComPtr<ID2D1Bitmap1>			m_bmpWeaponImage;
	D2D_RECT_F						m_rcWeaponSize;

	float							m_fHP;

	float							m_fBlockStunTimer;
	bool							m_bCollision;

	CObject*						m_pTarget;

	ComPtr<ID2D1SolidColorBrush>	m_pd2dLineBrush;
	D2D_POINT_2F					m_ptMuzzleDirection;
	D2D_POINT_2F					m_ptMuzzleStartPos;
	D2D_POINT_2F					m_ptMuzzleEndPos;

	float							m_fShootTimer;
	bool							m_bShoot;
};
