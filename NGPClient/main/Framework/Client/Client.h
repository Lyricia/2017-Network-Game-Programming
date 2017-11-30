#pragma once
#define SERVERPORT 9000
#define BUFFER_SIZE 512

struct ConnectedServerInfo
{
	SOCKET 	   				sock;
	SOCKADDR_IN  			addr;
	HANDLE 	    			hReceiver;
	list<NGPMSG*>*			pMsgQueue;
	LPCRITICAL_SECTION		pCS;

	ConnectedServerInfo() 
		: hReceiver(NULL)
		, pMsgQueue(nullptr)
		, pCS(NULL){}
	~ConnectedServerInfo() {
		if (hReceiver) TerminateThread(hReceiver, 0);
	}
};

class CClient
{
private:
	UINT 	         		m_Local_id;
	UINT 	         		m_Room_id;
	list<NGPMSG*>			m_MsgQueue;
	ConnectedServerInfo 	m_MainServer;
	CRITICAL_SECTION		m_CS;
	
public:
	CClient();
	~CClient();

	void Initialize();
	void Release();
	void ConnectServer();
	void SendMsgs(char* buf, UINT buf_size);

	void EnterCriticalSection() { ::EnterCriticalSection(&m_CS); }
	void LeaveCriticalSection() { ::LeaveCriticalSection(&m_CS); }

	UINT GetClientID() const { return m_Local_id; }
	UINT GetRoomID() const { return m_Local_id; }
};

static DWORD WINAPI RecvMessage(LPVOID arg);