#pragma once

#define SERVERPORT 9000
#define BUFSIZE 1024

#define DEFAULTCASE 0001
#define FIRSTCASE 0002

//클라이언트와 해당 클라이언트의 식별자(ID)를 묶은 구조체
struct ClientId {
	SOCKET client_sock;
	int client_id;
};

//객체의 2차원 임시좌표
struct Point2D {
	float position_x;
	float position_y;
};

//객체의 2차원 가속도(임시)
struct Accel2D {
	float accel_x;
	float accel_y;
};

DWORD WINAPI updateClient(LPVOID arg);		//Server내부 업데이트 + 데이터 송신
DWORD WINAPI getClient(LPVOID arg);			//Client의 데이터 수신

//사용자 정의 함수
void err_quit(const char* msg);
void err_display(const char* msg);
int recvn(SOCKET s, char* buf, int len, int flags);
