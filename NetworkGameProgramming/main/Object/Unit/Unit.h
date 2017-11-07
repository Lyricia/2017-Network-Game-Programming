#pragma once
#include "Object\Object.h"

class CUnit : public CObject
{
public:
	CUnit(D2D_POINT_2F pt, D2D_RECT_F rc = RectF());
	virtual ~CUnit();

	virtual void Update(float fTimeElapsed) override;
	virtual void Draw(ID2D1HwndRenderTarget* pd2dRenderTarget) override;

protected:

};