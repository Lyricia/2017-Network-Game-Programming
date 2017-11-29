#pragma once
#include "stdafx.h"
// 네트워크 게임 프로그래밍 메세지 구조체
#include "NGPmsg.h"

struct ConnectedServerInfo
{
	SOCKET 	   		sock;
	SOCKADDR_IN  	addr;
	HANDLE 	    	hReceiver;
	//MsgQueue      *pMsgQueue;
};

void err_quit(char *msg);
void err_display(char *msg);

// 서버의 가장 기본적인 구성요소들과 메서드들을 가지고 있는 서버 인터페이스 객체.
// 이 객체를 상속받는 자식 서버 객체들의 기본적인 틀을 정해준다.
class Server
{
protected:
	WSADATA			m_WSA;
	SOCKET			m_ListenSock;
	SOCKADDR_IN		m_ServerAddr;
	u_short			m_ServerPort = 9000;
	//MsgQueue		m_MsgQueue;

public:
	Server() {};
	~Server() {};

	// WSAStartup(), socket(), bind(), listen() 함수들을 이용하여
	// 멤버변수들을 사용 가능한 상태로 만들어 accept()를 호출할 수 있도록 준비한다.
	void Initialize();

	// closesocket(), WSACleanup() 함수들을 호출하여 멤버변수 listen 소켓을 닫고 윈속을 종료한다.
	void Release();

	//서버가 서비스를 시작하게 하는 완전 추상화 가상 함수이다. 
	// 상속받는 자식 서버객체에서 정의된다.
	virtual void Run() = 0;
};

