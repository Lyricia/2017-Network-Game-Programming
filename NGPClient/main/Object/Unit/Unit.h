#pragma once
#include "Object\Object.h"

class CIndRes;
class CUnit : public CObject
{
public:
	CUnit(D2D_POINT_2F pt, D2D_RECT_F rc);
	virtual ~CUnit();

	virtual void Update(float fTimeElapsed) override;
	virtual void Draw(ID2D1HwndRenderTarget* pd2dRenderTarget) override;

	virtual void RegisterImage(
		  CIndRes*					pIndRes
		, ID2D1HwndRenderTarget*	pd2dRenderTarget
		, path						filename);
	virtual void RegisterImage(const ComPtr<ID2D1Bitmap1>& bmp);
	virtual void RegisterImage(ComPtr<ID2D1Bitmap1>&& bmp) noexcept;

protected:
	D2D1_POINT_2F				m_ptDirection;
	D2D1_MATRIX_3X2_F			m_mtxRotate;
	ComPtr<ID2D1Bitmap1>		m_bmpImage;

};

