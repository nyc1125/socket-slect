#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<iostream>
using namespace std;


#include<WinSock2.h>					//windows平台的网络库头文件
#pragma comment(lib,"ws2_32.lib")		//库文件

#define PORT 8888;
//打开网路库
bool init_Socket();
//关闭网络库
bool close_Socket();
//创建客户端socket
SOCKET creatClientSocket(const char* ip);
//客户端处理进程
int processor(SOCKET cliSocket);



