#pragma once

#define SERVERIP "127.0.0.1"
//#define SERVERIP "192.168.35.203"
#define SERVERPORT 9000
#define BUFSIZE 1024

#define PLAYERMOVE 0001
#define PLAYERSHOOT 0002
#define PLAYERHIT 0003

//클라이언트와 해당 클라이언트의 식별자(ID)를 묶은 구조체
struct ClientId {
	SOCKET client_sock;
	int client_id;
};

//객체의 2차원 임시좌표
struct Point2D {
	int position_x;
	int position_y;
};

//객체의 2차원 가속도(임시)
struct Accel2D {
	float accel_x;
	float accel_y;
};

class Player {

public:
	Point2D position;
	Accel2D bAccel;
	int hp;
	int number;

	Player() {};
	~Player() {};

	Player(Point2D pos, Accel2D bAcc, int hp, int number) :  hp(100) {}

public:
	void printHp() { std::cout << "플레이어" << number << "의 현재 체력: " << hp << std::endl; };
	void printPos() { std::cout << "플레이어" << number << "의 현재 위치: (" << position.position_x << ", " << position.position_y << ")" << std::endl; };
	void printAcc() { std::cout << "플레이어" << number << "의 총알 정보: (" << bAccel.accel_x << ", " << bAccel.accel_y << ")" << std::endl; };
};

//스레드 함수
DWORD WINAPI updateClient(LPVOID arg);		//Server내부 업데이트 + 데이터 송신
DWORD WINAPI getClient(LPVOID arg);			//Client의 데이터 수신

//사용자 정의 함수
void err_quit(const char* msg);
void err_display(const char* msg);
int recvn(SOCKET s, char* buf, int len, int flags);

void serverButton();
void clientButton();