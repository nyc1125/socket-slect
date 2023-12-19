#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<iostream>
#include"Cmd_Server.h"
#include"Data_Server.h"
using namespace std;


#include<WinSock2.h>					//windows平台的网络库头文件
#pragma comment(lib,"ws2_32.lib")		//库文件

//打开网路库
bool init_Socket();
//关闭网络库
bool close_Socket();
//创建客户端socket
SOCKET creatServerSocket();
//处理进程
int processor(SOCKET cliSocket);



