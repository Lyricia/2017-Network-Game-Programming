#pragma once

class CGameWorld;

class CScene
{
public:
	CScene();
	virtual ~CScene();

	virtual bool OnCreate(wstring&& tag, CGameWorld* pGameWorld);

	virtual void Update(float fTimeElapsed) = 0;

	bool FindByTag(const wstring& Tag) const { return Tag == m_strTag; }

	const wstring& Tag() const { return m_strTag; }

protected:

	CGameWorld*				m_pGameWorld	{ nullptr }	;

	wstring					m_strTag					;
};
