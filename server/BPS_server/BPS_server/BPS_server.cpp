#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "ws2_32")
#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>
#include "Server.h"

//�̺�Ʈ �ڵ�
HANDLE recvData[2], updateData[2];

//������Ʈ Ȯ�ο� ������
Point2D demoPos[2];

int main() {

	//�ʱ�ȭ
	demoPos[0].position_x = 0;
	demoPos[0].position_y = 0;
	demoPos[1].position_x = 0;
	demoPos[1].position_y = 0;

	//�̺�Ʈ ����
	recvData[0] = CreateEvent(nullptr, false, false, nullptr);
	recvData[1] = CreateEvent(nullptr, false, false, nullptr);
	updateData[0] = CreateEvent(nullptr, false, true, nullptr);
	updateData[1] = CreateEvent(nullptr, false, true, nullptr);

	//���� �ʱ�ȭ
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

	//���� ����
	WSACleanup();

	//�̺�Ʈ ����
	CloseHandle(recvData[0]);
	CloseHandle(updateData[0]);
	CloseHandle(recvData[1]);
	CloseHandle(updateData[1]);

	return 0;
}

//Client�� ������ ����
DWORD WINAPI getClient(LPVOID arg)
{
	int id, retval, header;
	SOCKADDR_IN clientAddr;
	int addrLen;
	char buf[BUFSIZE];
	ClientId* argInfo;

	//Ŭ���̾�Ʈ ��ȣ ó��(�� Ŭ���̾�Ʈ ���� ����)
	argInfo = (ClientId*)arg;
	id = argInfo->client_id;

	//Ŭ���̾�Ʈ ó��

	//getpeername
	addrLen = sizeof(clientAddr);
	getpeername(argInfo->client_sock, (SOCKADDR*)&clientAddr, &addrLen);

	printf("data ���� ����\n");
	//data-recving
	while (1)
	{
		//declare additional-needed data

		//update �̺�Ʈ ���
		if (id == 0)
			WaitForSingleObject(updateData[0], INFINITE);
		else
			WaitForSingleObject(updateData[1], INFINITE);

		//��� ������ ����
		//recvCommand(header);
		retval = recvn(argInfo->client_sock, buf, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
		}

		header = atoi(buf);

		if (id == 0)
			printf("[TCP Ŭ���̾�Ʈ1] ��� ���� �Ϸ�: %d\n", header);
		else
			printf("[TCP Ŭ���̾�Ʈ2] ��� ���� �Ϸ�: %d\n", header);

		//����� �б�
		//header switch
		switch (header)
		{
		case DEFAULTCASE:
			//������ ������ ����
			//position data recv
			retval = recvn(argInfo->client_sock, buf, sizeof(Point2D), 0);
			if (retval == SOCKET_ERROR) {
				err_display("recv()");
			}

			Point2D* temp;
			temp = (Point2D*)buf;

			if (id == 0)
			{
				//Ŭ���̾�Ʈ1 ���� ���� ����
				printf("[TCP Ŭ���̾�Ʈ1 ���� ����] pPosition.x : %d, pPosition.y : %d\n", temp->position_x, temp->position_y);
			}
			else
			{
				//Ŭ���̾�Ʈ2 ���� ���� ����
				printf("[TCP Ŭ���̾�Ʈ2 ���� ����] pPosition.x : %d, pPosition.y : %d\n", temp->position_x, temp->position_y);
			}

			break;

		case FIRSTCASE:
			//�浹 ������ ����(Accel)
			//������ ������ ����
			retval = recv(argInfo->client_sock, buf, sizeof(Accel2D), 0);
			if (retval == SOCKET_ERROR) {
				err_display("recv()");
			}

			Accel2D* tempAccel;
			tempAccel = (Accel2D*)buf;

			break;
		}

		//�̺�Ʈ Ȱ��ȭ
		if (id == 0)
			SetEvent(recvData[0]);
		else
			SetEvent(recvData[1]);
	}
}

//���� ������Ʈ + �� Ŭ���̾�Ʈ���� ������ ����
DWORD WINAPI updateClient(LPVOID arg)
{
	//printf("update ����\n");

	int id, retval, header;
	SOCKADDR_IN clientAddr;
	int addrLen;
	char buf[BUFSIZE];
	ClientId* argInfo;
	Point2D temp;

	//Ŭ���̾�Ʈ ��ȣ ó��(�� Ŭ���̾�Ʈ ���� ����)
	argInfo = (ClientId*)arg;
	id = argInfo->client_id;

	//getpeername
	addrLen = sizeof(clientAddr);
	getpeername(argInfo->client_sock, (SOCKADDR*)&clientAddr, &addrLen);

	//sendCommand()
	//printf("data �۽� ����\n");

	while (1)
	{
		//getclient ���� ���
		if (id == 0)
			WaitForSingleObject(recvData[0], INFINITE);
		else
			WaitForSingleObject(recvData[1], INFINITE);

		//������Ʈ
		demoPos[0].position_x++;
		demoPos[0].position_y++;
		demoPos[1].position_x--;
		demoPos[1].position_y--;

		//��� ����
		snprintf(buf, sizeof(buf), "%d", DEFAULTCASE);

		header = atoi(buf);

		//����� �б�
		switch (header)
		{
		case DEFAULTCASE:
			//��� ����
			retval = send(argInfo->client_sock, buf, sizeof(int), 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()");
			}

			if (id == 0)
			{
				temp.position_x = demoPos[1].position_x;
				temp.position_y = demoPos[1].position_y;
			}
			else
			{
				temp.position_x = demoPos[0].position_x;
				temp.position_y = demoPos[0].position_y;
			}

			//����ü ������ �۽� ����
			retval = send(argInfo->client_sock, (char*)&temp, sizeof(Point2D), 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()");
			}

			break;
			
		}

		//eventȰ��ȭ
		if (id == 0)
			SetEvent(updateData[0]);
		else
			SetEvent(updateData[1]);
	}

}

void serverButton()
{
	//�ʱ�ȭ
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

	//������ ��ſ� ����� ����
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	bool token = true;
	HANDLE getClientThread[2];
	HANDLE updateClientThread[2];
	ClientId cNum, cNum2;	//id�� Ŭ���̾�Ʈ ������ ���� ������ ����ü

	while(1)
	{
		//accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ� = %s, ��Ʈ ��ȣ = %d\r\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		if (token)	//Ŭ���̾�Ʈ 1�� ���� ������ ������
		{
			cNum.client_id = 0;
			cNum.client_sock = client_sock;

			ClientId* cTemp;
			cTemp = &cNum;

			//Ŭ���̾�Ʈ1�� �����͸� �޴� ������
			getClientThread[0] = CreateThread(nullptr, 0, getClient, (LPVOID)cTemp, 0, nullptr);
			updateClientThread[0] = CreateThread(nullptr, 0, updateClient, (LPVOID)cTemp, 0, nullptr);
			token = !token;
		}
		else
		{
			cNum2.client_id = 1;
			cNum2.client_sock = client_sock;

			ClientId* cTemp;
			cTemp = &cNum2;

			//Ŭ���̾�Ʈ2�� �����͸� �޴� ������
			getClientThread[1] = CreateThread(nullptr, 0, getClient, (LPVOID)cTemp, 0, nullptr);
			updateClientThread[1] = CreateThread(nullptr, 0, updateClient, (LPVOID)cTemp, 0, nullptr);

			printf("ALl Player Connected\n");
		}

	}

	//closesocket()
	closesocket(listen_sock);
}

void clientButton()
{
	//�ʱ�ȭ
	int retval;
	char buf[BUFSIZE];
	int header;
	Point2D pos;
	pos.position_x = 1000;
	pos.position_y = 1000;

	//socket()
	SOCKET server_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (server_sock == INVALID_SOCKET) err_quit("socket()");

	//connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(server_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_quit((char*)"connect()");

	while (1)
	{
		//header set
		snprintf(buf, sizeof(buf), "%d", DEFAULTCASE);

		//header ����
		retval = send(server_sock, buf, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
		}

		//����ü ������ �۽� ����
		retval = send(server_sock, (char*)&pos, sizeof(Point2D), 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
		}

		//��� ������ ����
		//recvCommand(header);
		retval = recvn(server_sock, buf, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
		}

		header = atoi(buf);

		switch (header)
		{
		case DEFAULTCASE:
			retval = recvn(server_sock, buf, sizeof(Point2D), 0);
			if (retval == SOCKET_ERROR) {
				err_display("recv()");
			}
			
			pos = *(Point2D*)buf;

			printf("[����] ������ ���� ���� x: %d, y: %d\n", pos.position_x, pos.position_y);

			break;
		}
	}
}

// ���� �Լ� ���� ��� �� ����
void err_quit(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(nullptr, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

//���� �Լ� ���� ���
void err_display(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(nullptr, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

//����� ���� ������ ���� �Լ�
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