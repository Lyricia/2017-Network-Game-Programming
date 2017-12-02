#pragma once
#include "Object\Object.h"

#define BRICK_MAX_HP	100.0
#define BLOCK_STUN_TIME 0.2

class CResourceManager;
class CIndRes;
class CBrick : public CObject
{
public:
	CBrick(D2D_POINT_2F pt, D2D_RECT_F rc = RectF());
	virtual ~CBrick();

	virtual void Update(float fTimeElapsed) override;
	virtual void Draw(ID2D1HwndRenderTarget* pd2dRenderTarget) override;

	virtual void RegisterResourceManager(shared_ptr<CResourceManager> resMng);
	virtual void SetObjectInfo(LPVOID info) override;

	void Collide(float atk);
	void UpdateImg();

	void SetHP(float hp) { m_fHP = hp; }
	bool IsBroken() const { return m_fHP <= 0; }

private:
	shared_ptr<CResourceManager>	m_pResMng;
	ComPtr<ID2D1Bitmap1>			m_bmpImage;
	D2D1_SIZE_U						m_szImg;
	D2D1_POINT_2U					m_ptCurrImg;

	float							m_fHP;
	float							m_fBlockStunTimer;
	bool							m_bCollision;
};