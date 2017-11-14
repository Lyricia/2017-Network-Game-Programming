#pragma once
#include "stdafx.h"
#include "Server.h"

class AgentServer : public Server
{

	//ConnectedServerInfo		m_AgentServer;
	//list<RoomInfo*>			m_RoomList;

public:
	AgentServer() {};
	~AgentServer() {};

	// 서버가 서비스하기 시작하는 함수. High level design에 정의 되어있는 순서도를 따라 로직이 수행된다.
	virtual void Run();

	// Run()함수의 초반부에서 호출되며 메인 서버의 연결 요청을 기다린다. 메인 서버가 연결되면 recv 스레드가 생성된다.
	void AcceptMainServer();

	// 룸의 생성을 알리고 에이전트의 생성을 요청하는 메시지를 받으면 생성된 룸의 정보를 
	// 바탕으로 룸 리스트에 룸을 추가하고 에이전트들을 생성하여 정보를 메인 서버에 전달한다.
	void CreateAgentsToRoom();

	// 게임이 종료된 룸이 있으면 호출되는 함수. 
	// 게임 종료를 알리는 메시지를 받아 게임이 종료된 룸에 사용되던 시스템 자원을 회수하고 룸을 삭제한다.
	void DeleteAgentsFromRoom();
};