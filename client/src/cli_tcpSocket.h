#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<iostream>
using namespace std;


#include<WinSock2.h>					//windowsƽ̨�������ͷ�ļ�
#pragma comment(lib,"ws2_32.lib")		//���ļ�

#define PORT 8888;
//����·��
bool init_Socket();
//�ر������
bool close_Socket();
//�����ͻ���socket
SOCKET creatClientSocket(const char* ip);
//�ͻ��˴������
int processor(SOCKET cliSocket);



