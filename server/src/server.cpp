#include"server.h"
#include <iostream>
#include"CELLTimesTamp.hpp"//计时器


Server::Server() {
	//构造
	_serverSocket = INVALID_SOCKET;
}
Server::~Server()
{
	//析构 关闭
	Close();
}
//打开网络环境，初始化Socket
void Server::Init()
{
	//启动WinSOCK 2.x环境
#ifdef _WIN32
	//打开网路库
	WORD word = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(word, &data);
#endif	
	//如果不是无效的，先关闭掉，再重新创建
	if (INVALID_SOCKET != _serverSocket)
	{
		Close();
		std::cout << "关闭旧连接:" << _serverSocket << std::endl;
	}
	_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _serverSocket) {
		Close();
		std::cout << "服务端Socket<" << _serverSocket << ">创建失败" << std::endl;
	}
	else
	{
		std::cout << "服务端Socket<" << _serverSocket << ">建立成功" << std::endl;
	}
}
//绑定IP端口
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
	//如果没有指定服务器ip，则接受所有ip
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
	//绑定ip和端口
	//返回值：成功则返回0 ，失败返回-1，错误原因存于 errno 中。
	//如果绑定的地址错误，或者端口已被占用，bind 函数一定会报错，
	//否则一般不会返回错误。
	int ret = bind(_serverSocket, (sockaddr*)&addr, sizeof(addr));
	//#define INVALID_SOCKET  (SOCKET)(~0)
	if (INVALID_SOCKET == ret)
	{
		std::cout << "服务器绑定端口<" << PORT << ">失败" << std::endl;
		Close();
	}
	else {
		std::cout << "服务器绑定端口<" << PORT << ">成功" << std::endl;
	}
	return ret;
}
//监听端口
int Server::Listen(int n)
{
	//监听网络端口
	int ret = listen(_serverSocket, n);
	if (SOCKET_ERROR == ret) {
		std::cout << "服务端Socket<" << _serverSocket << ">错误，监听网络端口失败" << std::endl;
	}
	else {
		std::cout << "服务端Socket<" << _serverSocket << ">监听<"<<n<<">个网络成功, 等待客户端连接......" << std::endl;
	}
	return ret;
}


//主线程接受的客户端数量统计
std::atomic_int recvcClientCount = 0;
//接受客户端连接
void Server::Accept()						
{
	// 接受新客户端连接
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
		std::cout << "接受到无效客户端" << cliSocket << std::endl;
		return;
	}
	else
	{
		recvcClientCount++;
		////新用户加入，群发给所有客户端
		//NewUser_Join join;
		//strcpy(join.ip, inet_ntoa(socket_in.sin_addr));

		//join.port = socket_in.sin_port;

		/*for (int i = 0; i < g_cliSocket.size() ; i++) {
			send(g_cliSocket[i]->socketfd(), (const char*)&join, sizeof(join), 0);
			count++;
		}*/
		//send2All(&join);

		//主线程的子线程加入新客户端
		addClientToServer(new ClientSocket(cliSocket));
		//std::cout<< "客户端IP=" << inet_ntoa(socket_in.sin_addr)<<",SOCKET="<< cliSocket << "<"<<count<<"> PORT = " << socket_in.sin_port << "  加入" << std::endl;
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
	//最后一个prama:select的阻塞时间，若为Null则select一直阻塞直到有时间发生
	//清空fdset中所有文件描述符
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_ZERO(&exceptfds);
	//添加文件描述符fd到fdset集合中
	FD_SET(_serverSocket, &readfds);
	FD_SET(_serverSocket, &writefds);
	FD_SET(_serverSocket, &exceptfds);

	//最后一个参数：
	timeval time_out = { 1,0 };
	int ret = select(_serverSocket + 1, &readfds, &writefds, &exceptfds, &time_out);
	//std::cout << "服务器select客户端数ret=" << ret << std::endl;
	if (ret < 0)
	{
		std::cout << "客户端全部退出....select任务结束";
		return;
	}
	//判断描述符是否在集合中
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
		//计算一秒时间的包数量
		int recvCount = 0;
		int totalClientCount = 0;
		for (auto cellSer : _cellServerVector)
		{
			//把子细胞线程接收到的数据拿走，清0
			recvCount += cellSer->_recvCount;
			cellSer->_recvCount = 0;
			totalClientCount += cellSer->getClientCount();
		}
		//_clinets
		//收到的数据包数量，时间
		std::cout << "time<" << time << ">,socket<" << this->_serverSocket << ">clients<" << totalClientCount << ">,recvCount<" << (int)(recvCount / time) << ">" << std::endl;
		recvCount = 0;
		_time.update();
	}
}
//服务器主线程开启子细胞线程
void Server::Start()
{
	for (int i = 0; i < _CellServer_THREAD_COUNT; i++)
	{
		auto ser = new CellServer(this->_serverSocket);
		this->_cellServerVector.push_back(ser);
		std::cout << "服务器创建子细胞线程<"<<i<< "><Socket="<<ser->getSocket()<<">" << std::endl;
		(*ser).Start();
	}
}
//添加客户端到最少的线程队列
void Server::addClientToServer(ClientSocket* client)
{
	_clinets.push_back(client);
	//查找客户端数量最少的线程队列
	auto minCellServer = this->_cellServerVector[0];
	for (auto cellServer : _cellServerVector)
	{
		if (minCellServer->getClientCount() >= cellServer->getClientCount())
		{
			minCellServer = cellServer;
		}
	}
	//数量最少的线程对流加入客户端
	minCellServer->addClient(client);
}
//主线程关闭子线程队列
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
		//清除所有子线程
		for (auto cellVector : _cellServerVector)
		{
			cellVector->Close();
			delete cellVector;
		}
		//关闭主线程服务器socket
		closesocket(_serverSocket);
		//清除WinSock环境 
		WSACleanup();
#else
		for (int i = _clinets.size(); i >= 0; i--)
		{
			close(_clinets[i]->socketfd());
			delete _clinets[i];
		}
		//关闭socket
		close(_serverSocket);
#endif
		//清理数组
		this->_cellServerVector.clear();
		//最后再置为无效
		_serverSocket = INVALID_SOCKET;
	}
}
//发送消息
int Server::sendMessage(SOCKET cliSocket, DataHead* head)
{
	if (isRun() && _serverSocket)
	{
		return send(cliSocket, (const char*)&head, head->dataLength, 0);//响应消息
	}
	return SOCKET_ERROR;
}