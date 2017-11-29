#pragma once
#include "stdafx.h"
// ��Ʈ��ũ ���� ���α׷��� �޼��� ����ü
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

// ������ ���� �⺻���� ������ҵ�� �޼������ ������ �ִ� ���� �������̽� ��ü.
// �� ��ü�� ��ӹ޴� �ڽ� ���� ��ü���� �⺻���� Ʋ�� �����ش�.
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

	// WSAStartup(), socket(), bind(), listen() �Լ����� �̿��Ͽ�
	// ����������� ��� ������ ���·� ����� accept()�� ȣ���� �� �ֵ��� �غ��Ѵ�.
	void Initialize();

	// closesocket(), WSACleanup() �Լ����� ȣ���Ͽ� ������� listen ������ �ݰ� ������ �����Ѵ�.
	void Release();

	//������ ���񽺸� �����ϰ� �ϴ� ���� �߻�ȭ ���� �Լ��̴�. 
	// ��ӹ޴� �ڽ� ������ü���� ���ǵȴ�.
	virtual void Run() = 0;
};

