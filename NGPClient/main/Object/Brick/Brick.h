#pragma once
#include "Object\Object.h"

class CIndRes;
class CBrick : public CObject
{
public:
	CBrick(D2D_POINT_2F pt, D2D_RECT_F rc);
	virtual ~CBrick();

	virtual void Update(float fTimeElapsed) override;
	virtual void Draw(ID2D1HwndRenderTarget* pd2dRenderTarget) override;

	virtual void RegisterImage(
		  CIndRes*					pIndRes
		, ID2D1HwndRenderTarget*	pd2dRenderTarget
		, path						filename);
	virtual void RegisterImage(const ComPtr<ID2D1Bitmap1>& bmp);
	virtual void RegisterImage(ComPtr<ID2D1Bitmap1>&& bmp) noexcept;

private:
	ComPtr<ID2D1Bitmap1>		m_bmpImage;

	float						m_fHP;
};