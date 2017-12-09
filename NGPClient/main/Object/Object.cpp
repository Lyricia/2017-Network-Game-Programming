#include "stdafx.h"
#include "Object.h"

UINT		g_ObjectID = 0;

void CObject::ResetIDCount()
{
	g_ObjectID = 0;
}

CObject::CObject(D2D_POINT_2F pt, D2D_RECT_F rc)
	: m_ptPos(pt)
	, m_rcSize(rc)
	, m_Id(g_ObjectID++)
{
}
CObject::~CObject()
{
}

void CObject::RejectID()
{
	--g_ObjectID;
}
