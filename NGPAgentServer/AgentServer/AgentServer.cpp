#include "stdafx.h"
#include "AgentServer.h"

// TCP ȯ�濡�� �����͸� ������ �޾Ƴ��� ���� �Լ�
int recvn(SOCKET s, char *buf, int len, int flags)
{
	int received;
	char *ptr = buf;
	int left = len;

	while (left > 0) {
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}

void AgentServer::Run()
{

	// ���� ������ ������ ������ �ּ� ������ �޾Ƴ���.
	SOCKET MainServer_sock;
	SOCKADDR_IN MainServer_addr;

	// ������ ��ſ� ����
	char buf[BUFSIZE]; 
	// �ּ��� ���̸� �޾Ƴ���.
	int addrlen;
	// �޾Ƴ� ������ ���̸� �����Ѵ�.
	int datalen;
	// ���� �� ��� ���� �����Ѵ�.
	int retval;
	while (1)
	{
		// accept �ܰ�
		addrlen = sizeof(MainServer_addr);
		MainServer_sock = accept(m_ListenSock, (SOCKADDR *)&MainServer_addr, &addrlen);
		if (MainServer_sock == INVALID_SOCKET)
		{
			cout << "Main Server Socket Error" << endl;
			break;
		}

		printf("\n[TCP ����] ���μ��� ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
			inet_ntoa(MainServer_addr.sin_addr), ntohs(MainServer_addr.sin_port));

		while (1) {
			// ������ �ޱ�(���� ����)
			retval = recvn(MainServer_sock, (char *)&datalen, sizeof(int), 0);
			if (retval == SOCKET_ERROR) {
				break;
			}
			else if (retval == 0)
				break;

			// ������ �ޱ�(���� ����)
			retval = recvn(MainServer_sock, buf, datalen, 0);
			if (retval == SOCKET_ERROR) {
				break;
			}
			else if (retval == 0)
				break;

			// ���� ������ ���
			buf[retval] = '\0';
			printf("[TCP/%s:%d] %s\n", inet_ntoa(MainServer_addr.sin_addr),
				ntohs(MainServer_addr.sin_port), buf);
		}

		closesocket(MainServer_sock);
		printf("[TCP ����] ���μ��� ���� ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
			inet_ntoa(MainServer_addr.sin_addr), ntohs(MainServer_addr.sin_port));




	}
	



}

void AgentServer::AcceptMainServer()
{
}

void AgentServer::CreateAgentsToRoom()
{
}

void AgentServer::DeleteAgentsFromRoom()
{
}


