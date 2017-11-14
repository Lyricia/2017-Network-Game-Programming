#include "stdafx.h"
#include "AgentServer.h"

// TCP 환경에서 데이터를 완전히 받아내기 위한 함수
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

	// 메인 서버의 소켓의 정보와 주소 정보를 받아낸다.
	SOCKET MainServer_sock;
	SOCKADDR_IN MainServer_addr;

	// 데이터 통신용 버퍼
	char buf[BUFSIZE]; 
	// 주소의 길이를 받아낸다.
	int addrlen;
	// 받아낸 데이터 길이를 저장한다.
	int datalen;
	// 오류 시 결과 값을 저장한다.
	int retval;
	while (1)
	{
		// accept 단계
		addrlen = sizeof(MainServer_addr);
		MainServer_sock = accept(m_ListenSock, (SOCKADDR *)&MainServer_addr, &addrlen);
		if (MainServer_sock == INVALID_SOCKET)
		{
			cout << "Main Server Socket Error" << endl;
			break;
		}

		printf("\n[TCP 서버] 메인서버 접속: IP 주소=%s, 포트 번호=%d\n",
			inet_ntoa(MainServer_addr.sin_addr), ntohs(MainServer_addr.sin_port));

		while (1) {
			// 데이터 받기(고정 길이)
			retval = recvn(MainServer_sock, (char *)&datalen, sizeof(int), 0);
			if (retval == SOCKET_ERROR) {
				break;
			}
			else if (retval == 0)
				break;

			// 데이터 받기(가변 길이)
			retval = recvn(MainServer_sock, buf, datalen, 0);
			if (retval == SOCKET_ERROR) {
				break;
			}
			else if (retval == 0)
				break;

			// 받은 데이터 출력
			buf[retval] = '\0';
			printf("[TCP/%s:%d] %s\n", inet_ntoa(MainServer_addr.sin_addr),
				ntohs(MainServer_addr.sin_port), buf);
		}

		closesocket(MainServer_sock);
		printf("[TCP 서버] 메인서버 접속 종료: IP 주소=%s, 포트 번호=%d\n",
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


