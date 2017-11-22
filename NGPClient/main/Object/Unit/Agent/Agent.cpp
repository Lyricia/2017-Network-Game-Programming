#include "stdafx.h"
#include "Agent.h"

CAgent::CAgent(D2D_POINT_2F pt, D2D_RECT_F rc)
	: CUnit(pt, rc)
{

}

CAgent::~CAgent()
{
}

void CAgent::Update(float fTimeElapsed)
{
}

void CAgent::Draw(ID2D1HwndRenderTarget * pd2dRenderTarget)
{
}

void CAgent::RegisterImage(CIndRes * pIndRes, ID2D1HwndRenderTarget * pd2dRenderTarget, path filename)
{
}

void CAgent::RegisterSpriteImage(CIndRes * pIndRes, ID2D1HwndRenderTarget * pd2dRenderTarget, path filename, D2D_POINT_2F ptLength)
{
}

void CAgent::Collide(float atk)
{
}
