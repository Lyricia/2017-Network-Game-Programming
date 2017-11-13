// NGPMainServer.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include "./GameWorld/IndRes/IndRes.h"
#include "./GameWorld/GameWorld.h"

int main()
{
	shared_ptr<CIndRes> ind = make_shared<CIndRes>();
	if (!ind->Initialize()) return FALSE;

	CGameWorld gameworld;
	gameworld.Initailize(ind);
	gameworld.Run();

	return 0;
}