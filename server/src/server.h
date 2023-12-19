#ifndef _server_hpp
#define _server_hpp
#endif // !_server_hpp

#ifdef _WIN32
	#define FD_SETSIZE      1024
	#include<WinSock2.h>
	#pragma comment(lib,"ws2_32.lib")
#else
	#include<unistd.h>
	#include<arpa/inet.h>
	#include<string.h>
	#include<stdio.h>
	#define INVALID_SOCKET  (SOCKET)(-0)
	#define SOCKET_ERROR	(-1) 
	#define SOCKET int
#endif // _WIN32


#include"serMessageHeader.hpp"				//消息定义头文件
#include"ClientSocket.hpp"					//客户端头文件
#include"CellServer.hpp"					//子细胞线程头文件
#include"CELLTimesTamp.hpp"					//计时器头文件

#include<mutex>								//锁
		

#define _CellServer_THREAD_COUNT 4			//子细胞线程数量


//服务器
class Server {
private:
	SOCKET _serverSocket;										//服务器socket
	std::vector<ClientSocket*> _clinets;						//客户端队列
	std::vector<CellServer*> _cellServerVector;					//子线程队列
	CELLTimestamp _time;										//计时器
public:
	Server();
	virtual ~Server();
	//初始化
	void Init();
	//绑定IP和端口号
	int Bind(const char* IP, short PORT);
	//监听客户端
	int Listen(int n);
	//是否工作中
	bool isRun();
	void onRun();
	//接受客户端
	void Accept();
	//将客户端添加到服务器
	void addClientToServer(ClientSocket* client);
	//启动线程
	void Start();
	//处理客户端请求
	void time4msg();
	//发送数据
	int sendMessage(SOCKET cliSocket, DataHead* head);
	//关闭socket
	void Close();
};

