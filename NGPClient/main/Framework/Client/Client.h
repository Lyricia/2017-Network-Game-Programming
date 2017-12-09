#pragma once
#define SERVERPORT 9000
#define BUFFER_SIZE 512
//#define SERVERIP	"192.168.180.180"
#define SERVERIP	"127.0.0.1"

#define MESSAGE_PROCESSING_TIME 0.1

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

	void EnterCriticalSection() { ::EnterCriticalSection(pCS); }
	void LeaveCriticalSection() { ::LeaveCriticalSection(pCS); }
};

class CClient
{
private:
	UINT 	         		m_Local_id;
	UCHAR 	         		m_Room_id;
	list<NGPMSG*>			m_MsgQueue;
	ConnectedServerInfo 	m_MainServer;
	CRITICAL_SECTION		m_CS;

	std::string				m_ServerIP;
	
public:
	CClient();
	~CClient();

	void Initialize();
	void Release();
	bool ConnectServer();
	void DisconnectServer();
	void SendMsgs(char* buf, UINT buf_size);

	void EnterCriticalSection() { ::EnterCriticalSection(&m_CS); }
	void LeaveCriticalSection() { ::LeaveCriticalSection(&m_CS); }

	list<NGPMSG*>& MsgQueue() { return m_MsgQueue; }

	UINT GetClientID() const { return m_Local_id; }
	UCHAR GetRoomID() const { return m_Room_id; }
};

static DWORD WINAPI RecvMessage(LPVOID arg);