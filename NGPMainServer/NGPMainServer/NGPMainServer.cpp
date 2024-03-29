// NGPMainServer.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include "GameWorld\IndRes\IndRes.h"
#include "GameWorld\GameWorld.h"
#include "Server\Main\MainServer.h"

int main()
{
	//
	//CGameWorld gameworld;
	//gameworld.Initailize(m_IndRes);
	//gameworld.Run();

	MainServer* mainserver = new MainServer();

	mainserver->Initialize();
	mainserver->Run();
	mainserver->Release();

	return 0;
}