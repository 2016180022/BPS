#pragma once

#define SERVERIP "127.0.0.1"
//define SERVERIP ""
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
	int position_x;
	int position_y;
};

//��ü�� 2���� ���ӵ�(�ӽ�)
struct Accel2D {
	float accel_x;
	float accel_y;
};

//������ �Լ�
DWORD WINAPI updateClient(LPVOID arg);		//Server���� ������Ʈ + ������ �۽�
DWORD WINAPI getClient(LPVOID arg);			//Client�� ������ ����

//����� ���� �Լ�
void err_quit(const char* msg);
void err_display(const char* msg);
int recvn(SOCKET s, char* buf, int len, int flags);

void serverButton();
void clientButton();