#pragma once

#define SERVERPORT 9000
#define BUFSIZE 1024

#define DEFAULTCASE 0001
#define FIRSTCASE 0002

//Ŭ���̾�Ʈ�� �ش� Ŭ���̾�Ʈ�� �ĺ���(ID)�� ���� ����ü
struct ClientId {
	SOCKET client_sock;
	int client_id;
};

//��ü�� 2���� �ӽ���ǥ
struct Point2D {
	float position_x;
	float position_y;
};

//��ü�� 2���� ���ӵ�(�ӽ�)
struct Accel2D {
	float accel_x;
	float accel_y;
};

DWORD WINAPI updateClient(LPVOID arg);		//Server���� ������Ʈ + ������ �۽�
DWORD WINAPI getClient(LPVOID arg);			//Client�� ������ ����

//����� ���� �Լ�
void err_quit(const char* msg);
void err_display(const char* msg);
int recvn(SOCKET s, char* buf, int len, int flags);
