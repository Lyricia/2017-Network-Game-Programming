// NGPAgentServer.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include "Server\Server.h"
#include "Server\AgentServer.h"


int main()
{
	AgentServer* Agentserver = new AgentServer();

	Agentserver->Initialize();
	Agentserver->Run();
	Agentserver->Release();

	return 0;
}

