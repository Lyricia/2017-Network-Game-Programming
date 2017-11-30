#pragma once

#include "Object\Object.h"
#define DEFAULT_EFFECT_TIME 1.f

class CEffect : public CObject
{
public:
	CEffect(D2D_POINT_2F pt, D2D_RECT_F rc);
	virtual ~CEffect();

	virtual void Update(float fTimeElapsed) override;

	//virtual void Draw(ID2D1HwndRenderTarget* pd2dRenderTarget) override;
	virtual void RegisterEffectSprite(
		ComPtr<ID2D1Bitmap1> bmpImg, const D2D1_SIZE_U& sz);

	void SetEffectTime(float time) { m_fEffectTime = time; }
	bool IsFinished() const { return m_bFinished; }

private:
	ComPtr<ID2D1Bitmap1>			m_bmpImage;
	D2D1_SIZE_U						m_szImg;
	D2D1_POINT_2F					m_ptCurrImg;

	float							m_fEffectTime;

	bool							m_bFinished;
};