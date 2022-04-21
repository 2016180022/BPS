#pragma once

#define SERVERIP "127.0.0.1"
//#define SERVERIP "192.168.35.203"
#define SERVERPORT 9000
#define BUFSIZE 1024

#define PLAYERMOVE 0001
#define PLAYERSHOOT 0002
#define PLAYERHIT 0003

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
	void printHp() { std::cout << "�÷��̾�" << number << "�� ���� ü��: " << hp << std::endl; };
	void printPos() { std::cout << "�÷��̾�" << number << "�� ���� ��ġ: (" << position.position_x << ", " << position.position_y << ")" << std::endl; };
	void printAcc() { std::cout << "�÷��̾�" << number << "�� �Ѿ� ����: (" << bAccel.accel_x << ", " << bAccel.accel_y << ")" << std::endl; };
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