#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<iostream>
#include"Cmd_Server.h"
#include"Data_Server.h"
using namespace std;


#include<WinSock2.h>					//windowsƽ̨�������ͷ�ļ�
#pragma comment(lib,"ws2_32.lib")		//���ļ�

//����·��
bool init_Socket();
//�ر������
bool close_Socket();
//�����ͻ���socket
SOCKET creatServerSocket();
//�������
int processor(SOCKET cliSocket);



