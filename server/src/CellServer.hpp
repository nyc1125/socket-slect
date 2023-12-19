#pragma once
#include"ClientSocket.hpp"					//ͷ�ļ�

#include<iostream>
#include<vector>
#include<mutex>
#include<thread>


//������ϸ���߳�
class CellServer
{
public:
	std::atomic_int _recvCount;						//���߳̽��ܵ����ݳ��ȼ�¼
private:
	SOCKET _serverSocket;								//������socket
	std::vector<ClientSocket*> _clients;			//�ͻ��˶���
	std::vector<ClientSocket*> _clientsBuf;			//�ͻ��˻������
	//��
	std::mutex _mutex;								//��
	std::thread* _thread;							//�߳�
public:
	//���캯��
	CellServer(SOCKET socket = INVALID_SOCKET)
	{
		_thread = NULL;
		this->_serverSocket = socket;
		this->_recvCount = 0;
	}
	//��������
	~CellServer()
	{
		Close();
		this->_serverSocket = INVALID_SOCKET;
	}
	//��ȡsocket
	int getSocket()
	{
		return this->_serverSocket;
	}


#define RECV_BUF_SIZE 10240							//���建������С

	//���ܻ�����
	char recvBUF[RECV_BUF_SIZE] = {};
	//���߳̽�������
	int recvData(ClientSocket* clientSocket)
	{
		_recvCount++;
		//�ӻ�����ȡ������ͷ���ȣ�4�ֽ�
		/*********
			�ɹ�ִ��ʱ�����ؽ��յ����ֽ�����
			��һ���ѹر��򷵻�0��
			ʧ�ܷ��� - 1��errno����Ϊ���µ�ĳ��ֵ ��
		*********/
		int len1 = recv(clientSocket->socketfd(), recvBUF, RECV_BUF_SIZE, 0);
		if (len1 <= 0)
		{
			//�˴��ͻ��˹رշ���-1�������ԭ��
			std::cout << "�ͻ���" << clientSocket->socketfd() << "�˳�" << std::endl;
			return -1;
		}
		//std::cout << "��������һ���������ݳ���len1=" << len1 << std::endl;

		//��������Ϣ������
		memcpy(clientSocket->recvMsgBuf() + clientSocket->getlastPos(), recvBUF, len1);
		clientSocket->setlastPos(clientSocket->getlastPos() + len1);

		//������������Ϣ�����ƶ�
		while (clientSocket->getlastPos() >= sizeof(DataHead))
		{
			//������������ת��������ͷ
			DataHead* header = (DataHead*)clientSocket->recvMsgBuf();

			if (clientSocket->getlastPos() >= header->dataLength)
			{
				int nSize = clientSocket->getlastPos() - header->dataLength;
				//������Ϣ
				coupleMessage(header, clientSocket->socketfd());
				//ʣ��δ������Ϣǰ��
				memcpy(clientSocket->recvMsgBuf(), clientSocket->recvMsgBuf() + header->dataLength, nSize);
				clientSocket->setlastPos(nSize);
			}
			else
			{
				std::cout << "��Ϣ���Ȳ���" << std::endl;
				break;
			}
		}
		return 0;
	}
	//��ϸ���̴߳�����Ϣ
	void coupleMessage(DataHead* header, SOCKET clientSocket)
	{
		switch (header->cmd)
		{
		case CMD_LOGIN:
		{
			Login* login = (Login*)header;
			//���ܿͻ��˷��͵�������
			//cout << "�������CMD_LOGIN" << "     ���ݳ��ȣ�" << login->dataLength << "    �����û�����" << login->userName << "    ���룺" << login->password << endl;

			//�ж��˻��������Ƿ���ȷserSocket
			//�˴��ж��߼�

			//��Ӧ�ͻ�������
			LoginResult loginResult = {};
			loginResult.result = 1;
			//send(clientSocket, (const char*)&loginResult, sizeof(loginResult), 0);//��Ӧ��Ϣ
			return;
		}
		case CMD_LOGOUT:
		{
			//���ܿͻ��˷��͵�����
			Logout* logout = (Logout*)header;
			//std::cout << "�������CMD_LOGOUT" << "    ���ݳ��ȣ�" << logout->dataLength << "    �����û�����" << logout->userName << std::endl;

			//��Ӧ�ͻ�������
			LogoutResult logoutResult = {};
			logoutResult.result = 1;
			send(clientSocket, (const char*)&logoutResult, sizeof(logoutResult), 0);//��Ӧ��Ϣ
			return;
		}
		case CMD_NEWUSER_JOIN:
		{
			//���ܿͻ��˷��͵�����
			NewUser_Join* newJoin = (NewUser_Join*)header;
			std::cout << "�������CMD_NEW_USER_JOIN,<socket>" << clientSocket << ",IP��" << newJoin->ip << ",�˿ڣ�" << newJoin->port << std::endl;

			//��Ӧ�ͻ�������
			NewUser_Join_Result newJoinRet = {};
			strcpy(newJoinRet.ip, newJoin->ip);
			newJoinRet.port = newJoin->port;
			send(clientSocket, (const char*)&newJoinRet, sizeof(NewUser_Join_Result), 0);//��Ӧ��Ϣ
			return;
		}
		case CMD_EXIT: {
			Exit* exit = (Exit*)header;
			std::cout << "�ͻ����˳�" << std::endl;
			return;
		}
		default:
		{
			std::cout << "�������յ�δ������Ϣ" << std::endl;
			break;
		}
		}
	}

	//�Ƿ�����
	bool isRun()
	{
		return this->_serverSocket != INVALID_SOCKET;
	}
	//��ϸ���߳���������
	void onRun()
	{
		while (isRun())
		{
			//����ͻ��˻����������пͻ��˼��� 
			if (this->_clientsBuf.size() > 0)
			{
				//������ �Զ�����
				std::lock_guard<std::mutex> lock(this->_mutex);				//������������Ӧ���Ǵ�����{}������ʱ��
				for (auto client : this->_clientsBuf)
				{
					//���ͻ��˼�����ʽ����
					this->_clients.push_back(client);
					//std::cout << "�ͻ��˼������߳���ʽ����" << std::endl;
				}
				//�������������
				this->_clientsBuf.clear();
			}
			//���û����Ҫ����Ŀͻ��� ���� ������ִ��
			if (this->_clients.empty())
			{
				//��ʾ��ǰ�߳�����һ��ʱ�䣬�����ڼ䲻�������߳̾���cpu
				std::chrono::microseconds timeSleep(100);			//100����
				std::this_thread::sleep_for(timeSleep);
				//std::cout << "�޿ͻ��˼������ϸ���̣߳�����100����" << std::endl;
				continue;
			}
			fd_set readfds = {};
			fd_set writefds = {};
			fd_set exceptfds = {};
			//���һ��prama:select������ʱ�䣬��ΪNull��selectһֱ����ֱ����ʱ�䷢��
			//���fdset�������ļ�������
			FD_ZERO(&readfds);
			FD_ZERO(&writefds);
			FD_ZERO(&exceptfds);
			//����ļ�������fd��fdset������
			FD_SET(_serverSocket, &readfds);
			FD_SET(_serverSocket, &writefds);
			FD_SET(_serverSocket, &exceptfds);

			//�ȶ����׸��ͻ���socket���������
			SOCKET maxSocket = _clients[0]->socketfd();
			//���¿ͻ���socket�����д����
			for (int i = _clients.size() - 1; i >= 0; i--)
			{
				FD_SET(_clients[i]->socketfd(), &readfds);
				if (maxSocket < _clients[i]->socketfd())
				{
					maxSocket = _clients[i]->socketfd();
				}
			}
			//���һ��������
			timeval time_out = { 1,0 };
			int ret = select(maxSocket + 1, &readfds, &writefds, &exceptfds, &time_out);
			//std::cout << "���������߳�select�ͻ�����ret=" << ret << std::endl;
			if (ret <= 0)
			{
				//std::cout << "�ͻ���ȫ���˳�....select�������";
				return;
			}
			//���߳� �ͻ�����ʽvector���� ��������
			for (int i = _clients.size() - 1; i >= 0; i--)
			{
				//����ɶ����������пɶ���socket���ݣ�fd_set��һ������Ϊ64�����飩
				if (FD_ISSET(_clients[i]->socketfd(), &readfds))
				{
					if (-1 == recvData(_clients[i]))//�пͻ����˳�
					{
						auto iter = _clients.begin() + i;
						if (iter != _clients.end())
						{
							delete _clients[i];
							_clients.erase(iter);
						}
					}
				}
			}
		}
	}

	//��ѯ�ĸ����̶߳��пͻ�����������
	size_t getClientCount()
	{
		return this->_clients.size() + this->_clientsBuf.size();
	}
	//�߳������ر�
	void Close()
	{
		if (INVALID_SOCKET != _serverSocket)
		{
#ifdef _WIN32
			//�ر����пͻ���socket
			for (int i = _clients.size() - 1; i >= 0; i--) {
				closesocket(_clients[i]->socketfd());
				if (NULL != _clientsBuf[i])	delete _clientsBuf[i];
				if (NULL != _clients[i])	delete _clients[i];
			}
			//�رշ�����socket
			closesocket(_serverSocket);
			//���WinSock���� 
			WSACleanup();
#else
			//�ر����пͻ���socket
			for (int i = _clients.size() - 1; i >= 0; i--) {
				close(_clients[i]->socketfd());
				if (NULL != _clientsBuf[i])	delete _clientsBuf[i];
				if (NULL != _clients[i])	delete _clients[i];
			}
			//�ر�socket
			close(_serverSocket);
#endif
			//��������
			_clients.clear();
			//�������Ϊ��Ч
			_serverSocket = INVALID_SOCKET;
		}
	}
	//�����߳�
	void Start()
	{
		this->_thread = new std::thread(&CellServer::onRun, this);
	}
	//��ӿͻ���
	void addClient(ClientSocket* client)
	{
		std::lock_guard<std::mutex> lock(this->_mutex);
		//this->_mutex.lock();
		//cout << "�ͻ��˼������̻߳���������" << endl;
		this->_clientsBuf.push_back(client);

		//this->_mutex.unlock();
	}
};