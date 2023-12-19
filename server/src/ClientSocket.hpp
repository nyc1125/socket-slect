#pragma once
#include<WinSock2.h>
#include<mutex>

//������Ϣ���� 10k
#define RECV_BUF_SIZE 10240

//�ͻ���Socket
class ClientSocket
{
private:
	//�ͻ���socketfd
	SOCKET _clientSocketfd;
	//�ͻ��˽������ݳ���
	char _recvMsgBUF[RECV_BUF_SIZE * 10];
	//��¼λ��
	int _lastPos;
public:
	//���캯��
	ClientSocket(SOCKET cliSocket = INVALID_SOCKET)
	{
		this->_clientSocketfd = cliSocket;
		memset(this->_recvMsgBUF, 0, sizeof(this->_recvMsgBUF));
		this->_lastPos = 0;
	}
	//������
	virtual ~ClientSocket() {};
	//��ȡsocket������
	SOCKET socketfd() {
		return this->_clientSocketfd;
	}
	//��ȡ��Ϣ������
	char* recvMsgBuf()
	{
		return this->_recvMsgBUF;
	}
	//��ȡ���һ�����ݵ�λ��
	int getlastPos()
	{
		return this->_lastPos;
	}
	//�������һ�����ݵ�λ��
	void setlastPos(int pos)
	{
		this->_lastPos = pos;
	}

};