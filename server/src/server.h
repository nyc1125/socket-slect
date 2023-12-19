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


#include"serMessageHeader.hpp"				//��Ϣ����ͷ�ļ�
#include"ClientSocket.hpp"					//�ͻ���ͷ�ļ�
#include"CellServer.hpp"					//��ϸ���߳�ͷ�ļ�
#include"CELLTimesTamp.hpp"					//��ʱ��ͷ�ļ�

#include<mutex>								//��
		

#define _CellServer_THREAD_COUNT 4			//��ϸ���߳�����


//������
class Server {
private:
	SOCKET _serverSocket;										//������socket
	std::vector<ClientSocket*> _clinets;						//�ͻ��˶���
	std::vector<CellServer*> _cellServerVector;					//���̶߳���
	CELLTimestamp _time;										//��ʱ��
public:
	Server();
	virtual ~Server();
	//��ʼ��
	void Init();
	//��IP�Ͷ˿ں�
	int Bind(const char* IP, short PORT);
	//�����ͻ���
	int Listen(int n);
	//�Ƿ�����
	bool isRun();
	void onRun();
	//���ܿͻ���
	void Accept();
	//���ͻ�����ӵ�������
	void addClientToServer(ClientSocket* client);
	//�����߳�
	void Start();
	//����ͻ�������
	void time4msg();
	//��������
	int sendMessage(SOCKET cliSocket, DataHead* head);
	//�ر�socket
	void Close();
};

