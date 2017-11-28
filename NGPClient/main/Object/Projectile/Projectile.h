#pragma once
#include "Object\Object.h"

class CIndRes;
class CProjectile : public CObject
{
public:
	CProjectile(D2D_POINT_2F pt, D2D_RECT_F rc = RectF());
	virtual ~CProjectile();

	virtual void Update(float fTimeElapsed) override;
	virtual void Draw(ID2D1HwndRenderTarget* pd2dRenderTarget) override;

	virtual void RegisterSpriteImage(
		CIndRes*					pIndRes
		, ID2D1HwndRenderTarget*	pd2dRenderTarget
		, path						filename
		, D2D_POINT_2F				ptLength);

private:
	ComPtr<ID2D1Bitmap1>		m_bmpImage;
	D2D_POINT_2F				m_ptCurrImg;
	UINT						m_nSpriteImgWidth;
	UINT						m_nSpriteImgHeight;

};