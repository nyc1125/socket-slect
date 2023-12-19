#ifndef _Client_hpp
#define _Client_hpp
#endif // !_Client_hpp

#ifdef _WIN32
	#include<WinSock2.h>				//windowsƽ̨�������ͷ�ļ�
	#pragma comment(lib,"ws2_32.lib")		//���ļ�
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
	//��ʼ��socket
	SOCKET initSocket();
	//���ӷ�����
	int connectServer(const char* IP, short port);
	//�ر�Socket
	void Close();
	//�Ƿ�����
	bool isRun()
	{
		return _clientSocket != INVALID_SOCKET;
	}
	bool onRun();

	//����������Ϣ ����ճ������
	int recvData(SOCKET cliSocket);
	//����������Ϣ
	void coupleMessage(DataHead* header);
	//����������Ϣ
	int sendMessage(DataHead* header, int nLen);
private:
};
//���캯��
Client::Client() 
{
	_clientSocket = INVALID_SOCKET;
}
//�鹹����
Client::~Client() 
{
	Close();
}
//��ʼ��WinSock���� ������socket
SOCKET Client::initSocket()
{
	//����WinSOCK 2.x����
	#ifdef _WIN32
		//����·��
		WORD word = MAKEWORD(2, 2);
		WSADATA data;
		WSAStartup(MAKEWORD(2, 2), &data);
	#endif	
	//���������Ч�ģ��ȹرյ��������´���
	if (  INVALID_SOCKET != _clientSocket)
	{
		Close();
		//cout << "�رվ�����:" << _clientSocket << endl;
	}
	//�����ͻ���socket
	_clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	if (INVALID_SOCKET == _clientSocket) {
		
		cout << "�ͻ���Socket<" <<_clientSocket<< "����ʧ��" << endl;
	}
	else
	{
		//cout << "�ͻ���Socket<" << _clientSocket << ">�����ɹ�" << endl;
	}
	return _clientSocket;
}

atomic_int faildCount = 0;
//���ӷ�����
int Client::connectServer(const char* IP, short PORT)
{
	//�������Ч��,�ͳ�ʼ������
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
	//�ɹ�����0��ʧ�ܷ���-1
	int ret = connect(_clientSocket, (sockaddr*)&addr, sizeof(sockaddr_in));
	if (SOCKET_ERROR == ret)
	{
		faildCount++;
		cout << "Socket<" << _clientSocket <<"><"<<faildCount << ">���ӷ�����ʧ��,ԭ��"<<WSAGetLastError()<<endl;
		return SOCKET_ERROR;
	}
	else
	{
		cout << "Socket<"<<_clientSocket<<">���ӷ������ɹ�" << endl;
		return ret;
	}
	return ret;
} 
//�ر�����
void Client::Close()
{
	//���������Ч�ģ���ر�
	if (this->_clientSocket != INVALID_SOCKET) 
	{
#ifdef _WIN32
		//�ٵ��ùرպ���
		closesocket(_clientSocket);
		//���WinSock���� 
		WSACleanup();	
#else
		close(_clientSocket);
#endif
		//�������Ϊ��Ч
		_clientSocket = INVALID_SOCKET;
	}

}
bool Client::onRun()
{
	if (isRun()) 
	{
		fd_set readfds;
		//���fdset�������ļ�������
		FD_ZERO(&readfds);
		//����ļ�������fd��fdset������
		FD_SET(_clientSocket, &readfds);
		//���һ��prama:select������ʱ�䣬��ΪNull��selectһֱ����ֱ����ʱ�䷢��
		timeval time_out = { 0,10 };
		int ret = select(_clientSocket + 1, &readfds, 0, 0, &time_out);

		if (ret < 0)
		{
			cout << "�ͻ���select������ɽ���" << endl;
			return false;
		}
		if (FD_ISSET(_clientSocket, &readfds)) {

			FD_CLR(_clientSocket, &readfds);

			if (-1 == recvData(_clientSocket)) {
				cout << "�ͻ���socket<"<<_clientSocket<<">������ɽ���" << endl;
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

//������Ϣ
int Client::recvData(SOCKET cliSocket)
{
	//�ӻ�����ȡ������ͷ���ȣ�4�ֽ�
	int len1 = recv(cliSocket, recvBUF, RECV_BUF_SIZE, 0);
	if (len1 <= 0)
	{
		cout << "��������Ͽ����ӣ��������" << endl;
		return -1;
	}
	cout << "��һ�������յ����ݳ���=" << len1 << endl;

	//�����ݿ�������Ϣ������
	memcpy(recvMsgBUF+lastPos, recvBUF, len1);
	lastPos += len1;
	
	while(lastPos >= sizeof(DataHead))			
	{
		//ȡ����Ϣͷ
		DataHead* header = (DataHead*)recvMsgBUF;
		if (lastPos >= header->dataLength)			//�����ٰ�ճ������
		{
			//δ������Ϣ����
			int nSize = lastPos - header->dataLength;
			//������Ϣ
			coupleMessage(header);
			//ʣ��Ϊ������Ϣǰ��
			memcpy(recvMsgBUF, recvMsgBUF + header->dataLength, nSize);
			lastPos = nSize;
		}
		else
		{
			cout << "��Ϣ����������һ��������Ϣ" << endl;
			break;
		}
	}
	return 0;
}
//������Ϣ
void Client::coupleMessage(DataHead* header)
{
	switch (header->cmd)
	{
		case CMD_LOGIN_RESULT:
		{
			//���ܷ�������Ӧ����
			LoginResult *loginRet = (LoginResult*)header;//��¼���
			cout << "��������Ӧ:CMD_LOGIN_RESULT" <<"    ���ݳ���:" << loginRet->dataLength <<"    �����"<<loginRet->result<< endl;
			return;
		}
		case CMD_LOGOUT_RESULT:
		{
			//���ܷ�������Ӧ����
			LogoutResult *logoutRet = (LogoutResult*)header;//��¼���
			//cout << "��������Ӧ:CMD_LOGOUT_RESULT" << "    ���ݳ���:" << logoutRet->dataLength <<"   �����"<<logoutRet->result<< endl;
			return;
		}
		case CMD_NEWUSER_JOIN: {
			//���ܷ�������Ӧ����
			NewUser_Join* newJoin = (NewUser_Join*)header;//��¼���
			//cout << "��������Ӧ:CMD_LOGOUT_RESULT" << "    ���ݳ���:" << newJoin->dataLength <<"    �¿ͻ�IP��"<<newJoin->ip<<"    �¿ͻ�Port��"<<newJoin->port<< endl;
			return;
		}
		default:
		{
			cout << "�յ��ͻ���δ֪��Ϣ" << endl;
			break;
		}
	}
}
//������Ϣ
int Client::sendMessage(DataHead* header,int nLen)
{
	if (isRun() && header) 
	{
		return send(_clientSocket, (const char*)header, nLen, 0);
	}
	return SOCKET_ERROR;
}
	