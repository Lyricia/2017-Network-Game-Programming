#pragma once

class CObject
{
public:
	enum class Type { Player, Agent, Brick, Grenade, Effect };

public:
	CObject(D2D_POINT_2F pt = Point2F(), D2D_RECT_F rc = RectF());
	virtual ~CObject();

	virtual void Update(float fTimeElapsed) {}
	virtual void Draw(ID2D1HwndRenderTarget * pd2dRenderTarget) = 0;

	virtual void SetObjectInfo(LPVOID info) {}

	void SetPos(D2D_POINT_2F pt) { m_ptPos = pt; }
	void SetSize(D2D_RECT_F rc) { m_rcSize = rc; }
	void SetID(UINT id) { m_Id = id; }

	void RejectID();

	Type					GetTag()	const { return m_Tag; }
	UINT					GetID()		const { return m_Id; }
	const D2D_POINT_2F&		GetPos()	const { return m_ptPos; }
	const D2D_RECT_F&		GetSize()	const { return m_rcSize; }


protected:
	Type				m_Tag;
	UINT				m_Id;

	D2D_POINT_2F		m_ptPos;
	D2D_RECT_F			m_rcSize;
};
