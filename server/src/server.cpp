#include"server.h"
#include <iostream>
#include"CELLTimesTamp.hpp"//��ʱ��


Server::Server() {
	//����
	_serverSocket = INVALID_SOCKET;
}
Server::~Server()
{
	//���� �ر�
	Close();
}
//�����绷������ʼ��Socket
void Server::Init()
{
	//����WinSOCK 2.x����
#ifdef _WIN32
	//����·��
	WORD word = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(word, &data);
#endif	
	//���������Ч�ģ��ȹرյ��������´���
	if (INVALID_SOCKET != _serverSocket)
	{
		Close();
		std::cout << "�رվ�����:" << _serverSocket << std::endl;
	}
	_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _serverSocket) {
		Close();
		std::cout << "�����Socket<" << _serverSocket << ">����ʧ��" << std::endl;
	}
	else
	{
		std::cout << "�����Socket<" << _serverSocket << ">�����ɹ�" << std::endl;
	}
}
//��IP�˿�
int Server::Bind(const char* IP, short PORT)
{
	if (INVALID_SOCKET == _serverSocket)
	{
		Init();
	}
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = PORT;//host to net undesigned short htons(PORT)
#ifdef _WIN32
	//���û��ָ��������ip�����������ip
	if (IP)
	{
		addr.sin_addr.S_un.S_addr = inet_addr(IP);
	}
	else
	{
		addr.sin_addr.S_un.S_addr = INADDR_ANY;
	}
#else
	if (IP)
	{
		addr.sin_addr.s_addr = inet_addr(IP);
	}
	else
	{
		addr.sin_addr.s_addr = INADDR_ANY;
	}

#endif
	//��ip�Ͷ˿�
	//����ֵ���ɹ��򷵻�0 ��ʧ�ܷ���-1������ԭ����� errno �С�
	//����󶨵ĵ�ַ���󣬻��߶˿��ѱ�ռ�ã�bind ����һ���ᱨ��
	//����һ�㲻�᷵�ش���
	int ret = bind(_serverSocket, (sockaddr*)&addr, sizeof(addr));
	//#define INVALID_SOCKET  (SOCKET)(~0)
	if (INVALID_SOCKET == ret)
	{
		std::cout << "�������󶨶˿�<" << PORT << ">ʧ��" << std::endl;
		Close();
	}
	else {
		std::cout << "�������󶨶˿�<" << PORT << ">�ɹ�" << std::endl;
	}
	return ret;
}
//�����˿�
int Server::Listen(int n)
{
	//��������˿�
	int ret = listen(_serverSocket, n);
	if (SOCKET_ERROR == ret) {
		std::cout << "�����Socket<" << _serverSocket << ">���󣬼�������˿�ʧ��" << std::endl;
	}
	else {
		std::cout << "�����Socket<" << _serverSocket << ">����<"<<n<<">������ɹ�, �ȴ��ͻ�������......" << std::endl;
	}
	return ret;
}


//���߳̽��ܵĿͻ�������ͳ��
std::atomic_int recvcClientCount = 0;
//���ܿͻ�������
void Server::Accept()						
{
	// �����¿ͻ�������
	sockaddr_in socket_in = {};
	int Len = sizeof(sockaddr_in);
	SOCKET cliSocket = INVALID_SOCKET;
#ifdef _WIN32
	cliSocket = accept(_serverSocket, (sockaddr*)&socket_in, &Len);
#else
	cliSocket = accept(_serverSocket, (sockaddr*)&socket_in, (socklen_t*)&Len);
#endif
	if (SOCKET_ERROR == cliSocket)
	{
		std::cout << "���ܵ���Ч�ͻ���" << cliSocket << std::endl;
		return;
	}
	else
	{
		recvcClientCount++;
		////���û����룬Ⱥ�������пͻ���
		//NewUser_Join join;
		//strcpy(join.ip, inet_ntoa(socket_in.sin_addr));

		//join.port = socket_in.sin_port;

		/*for (int i = 0; i < g_cliSocket.size() ; i++) {
			send(g_cliSocket[i]->socketfd(), (const char*)&join, sizeof(join), 0);
			count++;
		}*/
		//send2All(&join);

		//���̵߳����̼߳����¿ͻ���
		addClientToServer(new ClientSocket(cliSocket));
		//std::cout<< "�ͻ���IP=" << inet_ntoa(socket_in.sin_addr)<<",SOCKET="<< cliSocket << "<"<<count<<"> PORT = " << socket_in.sin_port << "  ����" << std::endl;
		//std::cout << "recvcClientCount<" << recvcClientCount << ">" << std::endl;
	}
}
bool Server::isRun()
{
	return _serverSocket != INVALID_SOCKET;
}
void Server::onRun()
{
	time4msg();
	fd_set readfds{};
	fd_set writefds{};
	fd_set exceptfds{};
	//���һ��prama:select������ʱ�䣬��ΪNull��selectһֱ����ֱ����ʱ�䷢��
	//���fdset�������ļ�������
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_ZERO(&exceptfds);
	//����ļ�������fd��fdset������
	FD_SET(_serverSocket, &readfds);
	FD_SET(_serverSocket, &writefds);
	FD_SET(_serverSocket, &exceptfds);

	//���һ��������
	timeval time_out = { 1,0 };
	int ret = select(_serverSocket + 1, &readfds, &writefds, &exceptfds, &time_out);
	//std::cout << "������select�ͻ�����ret=" << ret << std::endl;
	if (ret < 0)
	{
		std::cout << "�ͻ���ȫ���˳�....select�������";
		return;
	}
	//�ж��������Ƿ��ڼ�����
	if (FD_ISSET(_serverSocket, &readfds))
	{
		FD_CLR(_serverSocket, &readfds);
		Accept();
	}
}

void Server::time4msg()
{
	auto time = _time.getElapsedSecond();
	if (time >= 1.0)
	{
		//����һ��ʱ��İ�����
		int recvCount = 0;
		int totalClientCount = 0;
		for (auto cellSer : _cellServerVector)
		{
			//����ϸ���߳̽��յ����������ߣ���0
			recvCount += cellSer->_recvCount;
			cellSer->_recvCount = 0;
			totalClientCount += cellSer->getClientCount();
		}
		//_clinets
		//�յ������ݰ�������ʱ��
		std::cout << "time<" << time << ">,socket<" << this->_serverSocket << ">clients<" << totalClientCount << ">,recvCount<" << (int)(recvCount / time) << ">" << std::endl;
		recvCount = 0;
		_time.update();
	}
}
//���������߳̿�����ϸ���߳�
void Server::Start()
{
	for (int i = 0; i < _CellServer_THREAD_COUNT; i++)
	{
		auto ser = new CellServer(this->_serverSocket);
		this->_cellServerVector.push_back(ser);
		std::cout << "������������ϸ���߳�<"<<i<< "><Socket="<<ser->getSocket()<<">" << std::endl;
		(*ser).Start();
	}
}
//��ӿͻ��˵����ٵ��̶߳���
void Server::addClientToServer(ClientSocket* client)
{
	_clinets.push_back(client);
	//���ҿͻ����������ٵ��̶߳���
	auto minCellServer = this->_cellServerVector[0];
	for (auto cellServer : _cellServerVector)
	{
		if (minCellServer->getClientCount() >= cellServer->getClientCount())
		{
			minCellServer = cellServer;
		}
	}
	//�������ٵ��̶߳�������ͻ���
	minCellServer->addClient(client);
}
//���̹߳ر����̶߳���
void Server::Close()
{
	if (INVALID_SOCKET != _serverSocket)
	{
#ifdef _WIN32
		for (int i = _clinets.size(); i >= 0; i--)
		{
			closesocket(_clinets[i]->socketfd());
			delete _clinets[i];
		}
		//����������߳�
		for (auto cellVector : _cellServerVector)
		{
			cellVector->Close();
			delete cellVector;
		}
		//�ر����̷߳�����socket
		closesocket(_serverSocket);
		//���WinSock���� 
		WSACleanup();
#else
		for (int i = _clinets.size(); i >= 0; i--)
		{
			close(_clinets[i]->socketfd());
			delete _clinets[i];
		}
		//�ر�socket
		close(_serverSocket);
#endif
		//��������
		this->_cellServerVector.clear();
		//�������Ϊ��Ч
		_serverSocket = INVALID_SOCKET;
	}
}
//������Ϣ
int Server::sendMessage(SOCKET cliSocket, DataHead* head)
{
	if (isRun() && _serverSocket)
	{
		return send(cliSocket, (const char*)&head, head->dataLength, 0);//��Ӧ��Ϣ
	}
	return SOCKET_ERROR;
}