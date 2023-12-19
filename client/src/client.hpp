#ifndef _Client_hpp
#define _Client_hpp
#endif // !_Client_hpp

#ifdef _WIN32
	#include<WinSock2.h>				//windows平台的网络库头文件
	#pragma comment(lib,"ws2_32.lib")		//库文件
#else
	#include<unistd.h>
	#include<arpa/inet.h>
	#include<string.h>
	#include<stdio.h>
	#define INVALID_SOCKET  (SOCKET)(-0)
	#define SOCKET_ERROR	(-1) 
	#define SOCKET int
#endif // _WIN32

#include<iostream>
#include<atomic>
#include"cliMessageHeader.hpp"
using namespace std;

class Client {
	SOCKET _clientSocket;
public:
	Client();
	virtual ~Client();
	//初始化socket
	SOCKET initSocket();
	//连接服务器
	int connectServer(const char* IP, short port);
	//关闭Socket
	void Close();
	//是否工作中
	bool isRun()
	{
		return _clientSocket != INVALID_SOCKET;
	}
	bool onRun();

	//接受网络消息 处理粘包问题
	int recvData(SOCKET cliSocket);
	//处理网络消息
	void coupleMessage(DataHead* header);
	//发送网络消息
	int sendMessage(DataHead* header, int nLen);
private:
};
//构造函数
Client::Client() 
{
	_clientSocket = INVALID_SOCKET;
}
//虚构函数
Client::~Client() 
{
	Close();
}
//初始化WinSock环境 并创建socket
SOCKET Client::initSocket()
{
	//启动WinSOCK 2.x环境
	#ifdef _WIN32
		//打开网路库
		WORD word = MAKEWORD(2, 2);
		WSADATA data;
		WSAStartup(MAKEWORD(2, 2), &data);
	#endif	
	//如果不是无效的，先关闭掉，再重新创建
	if (  INVALID_SOCKET != _clientSocket)
	{
		Close();
		//cout << "关闭旧连接:" << _clientSocket << endl;
	}
	//创建客户端socket
	_clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	if (INVALID_SOCKET == _clientSocket) {
		
		cout << "客户端Socket<" <<_clientSocket<< "创建失败" << endl;
	}
	else
	{
		//cout << "客户端Socket<" << _clientSocket << ">建立成功" << endl;
	}
	return _clientSocket;
}

atomic_int faildCount = 0;
//连接服务器
int Client::connectServer(const char* IP, short PORT)
{
	//如果是无效的,就初始化以下
	if (INVALID_SOCKET == _clientSocket)
	{
		initSocket();
	}
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = PORT;
#ifdef _WIN32
	addr.sin_addr.S_un.S_addr = inet_addr(IP);
#else
	addr.sin_addr.s_addr = inet_addr(IP);
#endif
	//成功返回0，失败返回-1
	int ret = connect(_clientSocket, (sockaddr*)&addr, sizeof(sockaddr_in));
	if (SOCKET_ERROR == ret)
	{
		faildCount++;
		cout << "Socket<" << _clientSocket <<"><"<<faildCount << ">连接服务器失败,原因："<<WSAGetLastError()<<endl;
		return SOCKET_ERROR;
	}
	else
	{
		cout << "Socket<"<<_clientSocket<<">连接服务器成功" << endl;
		return ret;
	}
	return ret;
} 
//关闭连接
void Client::Close()
{
	//如果不是无效的，则关闭
	if (this->_clientSocket != INVALID_SOCKET) 
	{
#ifdef _WIN32
		//再调用关闭函数
		closesocket(_clientSocket);
		//清除WinSock环境 
		WSACleanup();	
#else
		close(_clientSocket);
#endif
		//最后再置为无效
		_clientSocket = INVALID_SOCKET;
	}

}
bool Client::onRun()
{
	if (isRun()) 
	{
		fd_set readfds;
		//清空fdset中所有文件描述符
		FD_ZERO(&readfds);
		//添加文件描述符fd到fdset集合中
		FD_SET(_clientSocket, &readfds);
		//最后一个prama:select的阻塞时间，若为Null则select一直阻塞直到有时间发生
		timeval time_out = { 0,10 };
		int ret = select(_clientSocket + 1, &readfds, 0, 0, &time_out);

		if (ret < 0)
		{
			cout << "客户端select任务完成结束" << endl;
			return false;
		}
		if (FD_ISSET(_clientSocket, &readfds)) {

			FD_CLR(_clientSocket, &readfds);

			if (-1 == recvData(_clientSocket)) {
				cout << "客户端socket<"<<_clientSocket<<">任务完成结束" << endl;
				return false;
			}
		}
		return true;
	}	
	return false;
}

#define RECV_BUF_SIZE 10240
char recvBUF[RECV_BUF_SIZE] = {};
char recvMsgBUF[RECV_BUF_SIZE * 10] = {};
int lastPos = 0;

//接收消息
int Client::recvData(SOCKET cliSocket)
{
	//从缓冲区取出数据头长度：4字节
	int len1 = recv(cliSocket, recvBUF, RECV_BUF_SIZE, 0);
	if (len1 <= 0)
	{
		cout << "与服务器断开连接，任务结束" << endl;
		return -1;
	}
	cout << "第一缓冲区收到数据长度=" << len1 << endl;

	//将数据拷贝到消息缓冲区
	memcpy(recvMsgBUF+lastPos, recvBUF, len1);
	lastPos += len1;
	
	while(lastPos >= sizeof(DataHead))			
	{
		//取出消息头
		DataHead* header = (DataHead*)recvMsgBUF;
		if (lastPos >= header->dataLength)			//处理少包粘包问题
		{
			//未处理消息长度
			int nSize = lastPos - header->dataLength;
			//处理消息
			coupleMessage(header);
			//剩余为处理消息前移
			memcpy(recvMsgBUF, recvMsgBUF + header->dataLength, nSize);
			lastPos = nSize;
		}
		else
		{
			cout << "消息缓冲区不够一条完整消息" << endl;
			break;
		}
	}
	return 0;
}
//处理消息
void Client::coupleMessage(DataHead* header)
{
	switch (header->cmd)
	{
		case CMD_LOGIN_RESULT:
		{
			//接受服务器响应数据
			LoginResult *loginRet = (LoginResult*)header;//登录结果
			cout << "服务器响应:CMD_LOGIN_RESULT" <<"    数据长度:" << loginRet->dataLength <<"    结果："<<loginRet->result<< endl;
			return;
		}
		case CMD_LOGOUT_RESULT:
		{
			//接受服务器响应数据
			LogoutResult *logoutRet = (LogoutResult*)header;//登录结果
			//cout << "服务器响应:CMD_LOGOUT_RESULT" << "    数据长度:" << logoutRet->dataLength <<"   结果："<<logoutRet->result<< endl;
			return;
		}
		case CMD_NEWUSER_JOIN: {
			//接受服务器响应数据
			NewUser_Join* newJoin = (NewUser_Join*)header;//登录结果
			//cout << "服务器响应:CMD_LOGOUT_RESULT" << "    数据长度:" << newJoin->dataLength <<"    新客户IP："<<newJoin->ip<<"    新客户Port："<<newJoin->port<< endl;
			return;
		}
		default:
		{
			cout << "收到客户端未知消息" << endl;
			break;
		}
	}
}
//发送消息
int Client::sendMessage(DataHead* header,int nLen)
{
	if (isRun() && header) 
	{
		return send(_clientSocket, (const char*)header, nLen, 0);
	}
	return SOCKET_ERROR;
}
	