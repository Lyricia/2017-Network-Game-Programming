#pragma once
#define SERVERPORT 9000
#define BUFFER_SIZE 512

struct ConnectedServerInfo
{
	SOCKET 	   		sock;
	SOCKADDR_IN  	addr;
	HANDLE 	    	hReceiver;
	//MsgQueue      	*pMsgQueue;
};

class CClient
{
private:
	UINT 	         		m_Local_id;
	SOCKET 	     			m_Local_sock;
	SOCKADDR_IN       		m_Local_addr;
	//MsgQueue				m_MsgQueue;
	ConnectedServerInfo 	m_MainServer;

	// Test용 버퍼. Msg Queue로 바꿀것.
	char					m_pBuffer[BUFFER_SIZE];
	
public:
	CClient();
	~CClient();

	void Initialize();
	void Release();
	void ConnectServer();
	void SendMsgs();

	void SetClientID(UINT id) { m_Local_id = id; }
};
