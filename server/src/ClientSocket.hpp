#pragma once
#include<WinSock2.h>
#include<mutex>

//缓冲消息长度 10k
#define RECV_BUF_SIZE 10240

//客户端Socket
class ClientSocket
{
private:
	//客户端socketfd
	SOCKET _clientSocketfd;
	//客户端接收数据长度
	char _recvMsgBUF[RECV_BUF_SIZE * 10];
	//记录位置
	int _lastPos;
public:
	//构造函数
	ClientSocket(SOCKET cliSocket = INVALID_SOCKET)
	{
		this->_clientSocketfd = cliSocket;
		memset(this->_recvMsgBUF, 0, sizeof(this->_recvMsgBUF));
		this->_lastPos = 0;
	}
	//虚析构
	virtual ~ClientSocket() {};
	//获取socket描述符
	SOCKET socketfd() {
		return this->_clientSocketfd;
	}
	//获取消息缓冲区
	char* recvMsgBuf()
	{
		return this->_recvMsgBUF;
	}
	//获取最后一个数据的位置
	int getlastPos()
	{
		return this->_lastPos;
	}
	//设置最后一个数据的位置
	void setlastPos(int pos)
	{
		this->_lastPos = pos;
	}

};