#pragma once
#define SERVERPORT 9000
#define BUFFER_SIZE 512

struct ConnectedServerInfo
{
	SOCKET 	   			sock;
	SOCKADDR_IN  		addr;
	HANDLE 	    		hReceiver;
	list<NGPMSG*>*		pMsgQueue;

	ConnectedServerInfo() : hReceiver(NULL), pMsgQueue(nullptr) {}
	~ConnectedServerInfo() {
		if (hReceiver) TerminateThread(hReceiver, 0);
	}
};

class CClient
{
private:
	UINT 	         		m_Local_id;
	list<NGPMSG*>			m_MsgQueue;
	ConnectedServerInfo 	m_MainServer;
	
public:
	CClient();
	~CClient();

	void Initialize();
	void Release();
	void ConnectServer();
	void SendMsgs(char* buf, UINT buf_size);

	void SetClientID(UINT id) { m_Local_id = id; }
	UINT GetClientID() const { return m_Local_id; }
};

static DWORD WINAPI RecvMessage(LPVOID arg);