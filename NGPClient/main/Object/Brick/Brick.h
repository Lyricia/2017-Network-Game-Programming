#pragma once
#include "Object\Object.h"

#define BRICK_MAX_HP	100.0
#define BLOCK_STUN_TIME 0.2

class CIndRes;
class CBrick : public CObject
{
public:
	CBrick(D2D_POINT_2F pt, D2D_RECT_F rc = RectF());
	virtual ~CBrick();

	virtual void Update(float fTimeElapsed) override;
	virtual void Draw(ID2D1HwndRenderTarget* pd2dRenderTarget) override;

	virtual void RegisterSpriteImage(
		CIndRes*					pIndRes
		, ID2D1HwndRenderTarget*	pd2dRenderTarget
		, path						filename
		, D2D_POINT_2F				ptLength);

	void Collide(float atk);
	void UpdateImg();

	void SetHP(float hp) { m_fHP = hp; }
	bool IsBroken() const { return m_fHP <= 0; }

private:
	ComPtr<ID2D1Bitmap1>		m_bmpImage;
	D2D_POINT_2F				m_ptCurrImg;
	UINT						m_nSpriteImgWidth;
	UINT						m_nSpriteImgHeight;

	float						m_fHP;
	float						m_fBlockStunTimer;
	bool						m_bCollision;
};