#pragma once

class CGameWorld;

class CScene
{
public:
	CScene();
	virtual ~CScene();

	virtual bool OnCreate(std::wstring&& tag, CGameWorld* pGameWorld);

	virtual void Update(float fTimeElapsed) = 0;

	bool FindByTag(const std::wstring& Tag) const { return Tag == m_strTag; }

	const std::wstring& Tag() const { return m_strTag; }

protected:

	CGameWorld*				m_pGameWorld	{ nullptr }	;

	std::wstring					m_strTag					;
};
