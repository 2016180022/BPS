#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "ws2_32")
#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>
#include "Server.h"

int main() {

	//윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	printf("type command code plz\n");
	printf("1: serverbutton\n");
	printf("2: clientbutton\n");

	int commandCode;
	int retval = scanf("%d", &commandCode);

	switch (commandCode)
	{
	case 1:
		serverButton();
		break;
	case 2:
		clientButton();
		break;
	}

	return 0;
}

//Client의 데이터 수신
DWORD WINAPI getClient(LPVOID arg)
{
	int id, retval, header;
	SOCKADDR_IN clientAddr;
	int addrLen;
	char buf[BUFSIZE];
	ClientId* argInfo;

	//클라이언트 번호 처리(각 클라이언트 정보 구분)
	argInfo = (ClientId*)arg;
	id = argInfo->client_id;

	//클라이언트 처리

	//getpeername
	addrLen = sizeof(clientAddr);
	getpeername(argInfo->client_sock, (SOCKADDR*)&clientAddr, &addrLen);

	//printf("data 수신 시작\n");
	//data-recving
	while (1)
	{
		//declare additional-needed data

		//update 이벤트 대기

		//헤더 데이터 수신
		//recvCommand(header);
		retval = recvn(argInfo->client_sock, buf, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
		}

		header = atoi(buf);

		//if (id == 0)
		//	printf("[TCP 클라이언트1] 헤더 수신 완료: %d\n", header);
		//else
		//	printf("[TCP 클라이언트2] 헤더 수신 완료: %d\n", header);

		//헤더별 분기
		//header switch
		switch (header)
		{
		case DEFAULTCASE:
			//포지션 데이터 수신
			//position data recv
			retval = recvn(argInfo->client_sock, buf, sizeof(Point2D), 0);
			if (retval == SOCKET_ERROR) {
				err_display("recv()");
			}

			Point2D* temp;
			temp = (Point2D*)buf;

			if (id == 0)
			{
				//클라이언트1 수신 정보 적용
				//printf("[TCP 클라이언트1 수신 정보] pPosition.x : %d, pPosition.y : %d\n", pPosition[0].position_x, pPosition[0].position_y);
			}
			else
			{
				//클라이언트2 수신 정보 적용
				//printf("[TCP 클라이언트2 수신 정보] pPosition.x : %d, pPosition.y : %d\n", pPosition[1].position_x, pPosition[1].position_y);
			}

			break;

		case FIRSTCASE:
			//충돌 데이터 수신(Accel)
			//포지션 데이터 수신
			retval = recv(argInfo->client_sock, buf, sizeof(Accel2D), 0);
			if (retval == SOCKET_ERROR) {
				err_display("recv()");
			}

			Accel2D* tempAccel;
			tempAccel = (Accel2D*)buf;

			break;
		}

		//이벤트 활성화
	}
}

//서버 업데이트 + 양 클라이언트에게 데이터 전송
DWORD WINAPI updateClient(LPVOID arg)
{
	//printf("update 실행\n");

	int id, retval, header;
	SOCKADDR_IN clientAddr;
	int addrLen;
	char buf[BUFSIZE];
	ClientId* argInfo;

	//클라이언트 번호 처리(각 클라이언트 정보 구분)
	argInfo = (ClientId*)arg;
	id = argInfo->client_id;

	//getpeername
	addrLen = sizeof(clientAddr);
	getpeername(argInfo->client_sock, (SOCKADDR*)&clientAddr, &addrLen);

	//sendCommand()
	//printf("data 송신 시작\n");

	while (1)
	{
		//getclient 종료 대기

		//업데이트

		//헤더 변경
		//if (P1Goal || P2Goal)
		//	snprintf(buf, sizeof(buf), "%d", GOAL);
		//else
		//	snprintf(buf, sizeof(buf), "%d", B_POSITION);

		header = atoi(buf);

		//헤더별 분기
		switch (header)
		{
		case DEFAULTCASE:
			//헤더 전송
			retval = send(argInfo->client_sock, buf, sizeof(int), 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()");
			}

			//데이터 전송
			//retval = send(argInfo->client_sock, (char*)&score, sizeof(int), 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()");
			}

			break;
			
			//구조체 데이터 송신 형식
			//retval = send(argInfo->client_sock, (char*)&temp, sizeof(Point2D), 0);
			//if (retval == SOCKET_ERROR) {
			//	err_display("send()");
			//}
		}

		//event활성화
	}

}

void serverButton()
{
	//초기화
	int retval;

	//socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	//bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	//listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	//데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	bool token = true;
	HANDLE getClientThread[2];
	HANDLE updateClientThread[2];
	ClientId cNum, cNum2;	//id와 클라이언트 정보를 같이 관리할 구조체

	while (1)
	{
		//accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		printf("[TCP 서버] 클라이언트 접속: IP 주소 = %s, 포트 번호 = %d\r\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		//if (token)	//클라이언트 1의 연결 설정을 받으면
		//{
		//	cNum.client_id = 0;
		//	cNum.client_sock = client_sock;

		//	ClientId* cTemp;
		//	cTemp = &cNum;

		//	//클라이언트1의 데이터를 받는 스레드
		//	getClientThread[0] = CreateThread(nullptr, 0, getClient, (LPVOID)cTemp, 0, nullptr);
		//	updateClientThread[0] = CreateThread(nullptr, 0, updateClient, (LPVOID)cTemp, 0, nullptr);
		//	token = !token;
		//}
		//else
		//{
		//	cNum2.client_id = 1;
		//	cNum2.client_sock = client_sock;

		//	ClientId* cTemp;
		//	cTemp = &cNum2;

		//	//클라이언트2의 데이터를 받는 스레드
		//	getClientThread[1] = CreateThread(nullptr, 0, getClient, (LPVOID)cTemp, 0, nullptr);
		//	updateClientThread[1] = CreateThread(nullptr, 0, updateClient, (LPVOID)cTemp, 0, nullptr);
		//}

	}

	//closesocket()
	closesocket(listen_sock);

	//윈속 종료
	WSACleanup();

	//이벤트 제거
}

void clientButton()
{
	//초기화
	int retval;
	char buf[BUFSIZE];

	//socket()
	SOCKET client_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (client_sock == INVALID_SOCKET) err_quit("socket()");

	//connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(client_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_quit((char*)"connect()");

}

// 소켓 함수 오류 출력 후 종료
void err_quit(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(nullptr, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

//소켓 함수 오류 출력
void err_display(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(nullptr, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

//사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char* buf, int len, int flags)
{
	int received;
	char* ptr = buf;
	int left = len;

	while (left > 0)
	{
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