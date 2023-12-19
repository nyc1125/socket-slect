#pragma once
#include"ClientSocket.hpp"					//头文件

#include<iostream>
#include<vector>
#include<mutex>
#include<thread>


//服务器细胞线程
class CellServer
{
public:
	std::atomic_int _recvCount;						//子线程接受的数据长度记录
private:
	SOCKET _serverSocket;								//服务器socket
	std::vector<ClientSocket*> _clients;			//客户端队列
	std::vector<ClientSocket*> _clientsBuf;			//客户端缓冲队列
	//锁
	std::mutex _mutex;								//锁
	std::thread* _thread;							//线程
public:
	//构造函数
	CellServer(SOCKET socket = INVALID_SOCKET)
	{
		_thread = NULL;
		this->_serverSocket = socket;
		this->_recvCount = 0;
	}
	//析构函数
	~CellServer()
	{
		Close();
		this->_serverSocket = INVALID_SOCKET;
	}
	//获取socket
	int getSocket()
	{
		return this->_serverSocket;
	}


#define RECV_BUF_SIZE 10240							//定义缓冲区大小

	//接受缓冲区
	char recvBUF[RECV_BUF_SIZE] = {};
	//子线程接受数据
	int recvData(ClientSocket* clientSocket)
	{
		_recvCount++;
		//从缓冲区取出数据头长度：4字节
		/*********
			成功执行时，返回接收到的字节数。
			另一端已关闭则返回0。
			失败返回 - 1，errno被设为以下的某个值 ：
		*********/
		int len1 = recv(clientSocket->socketfd(), recvBUF, RECV_BUF_SIZE, 0);
		if (len1 <= 0)
		{
			//此处客户端关闭返回-1（不清楚原因）
			std::cout << "客户端" << clientSocket->socketfd() << "退出" << std::endl;
			return -1;
		}
		//std::cout << "服务器第一缓冲区数据长度len1=" << len1 << std::endl;

		//拷贝到消息缓冲区
		memcpy(clientSocket->recvMsgBuf() + clientSocket->getlastPos(), recvBUF, len1);
		clientSocket->setlastPos(clientSocket->getlastPos() + len1);

		//两级缓冲区消息拷贝移动
		while (clientSocket->getlastPos() >= sizeof(DataHead))
		{
			//将缓冲区内容转化成数据头
			DataHead* header = (DataHead*)clientSocket->recvMsgBuf();

			if (clientSocket->getlastPos() >= header->dataLength)
			{
				int nSize = clientSocket->getlastPos() - header->dataLength;
				//处理消息
				coupleMessage(header, clientSocket->socketfd());
				//剩余未处理消息前移
				memcpy(clientSocket->recvMsgBuf(), clientSocket->recvMsgBuf() + header->dataLength, nSize);
				clientSocket->setlastPos(nSize);
			}
			else
			{
				std::cout << "消息长度不够" << std::endl;
				break;
			}
		}
		return 0;
	}
	//子细胞线程处理消息
	void coupleMessage(DataHead* header, SOCKET clientSocket)
	{
		switch (header->cmd)
		{
		case CMD_LOGIN:
		{
			Login* login = (Login*)header;
			//接受客户端发送的请求体
			//cout << "请求命令：CMD_LOGIN" << "     数据长度：" << login->dataLength << "    请求用户名：" << login->userName << "    密码：" << login->password << endl;

			//判断账户名密码是否正确serSocket
			//此处判断逻辑

			//响应客户端请求
			LoginResult loginResult = {};
			loginResult.result = 1;
			//send(clientSocket, (const char*)&loginResult, sizeof(loginResult), 0);//响应消息
			return;
		}
		case CMD_LOGOUT:
		{
			//接受客户端发送的请求
			Logout* logout = (Logout*)header;
			//std::cout << "请求命令：CMD_LOGOUT" << "    数据长度：" << logout->dataLength << "    请求用户名：" << logout->userName << std::endl;

			//响应客户端请求
			LogoutResult logoutResult = {};
			logoutResult.result = 1;
			send(clientSocket, (const char*)&logoutResult, sizeof(logoutResult), 0);//响应消息
			return;
		}
		case CMD_NEWUSER_JOIN:
		{
			//接受客户端发送的请求
			NewUser_Join* newJoin = (NewUser_Join*)header;
			std::cout << "请求命令：CMD_NEW_USER_JOIN,<socket>" << clientSocket << ",IP：" << newJoin->ip << ",端口：" << newJoin->port << std::endl;

			//响应客户端请求
			NewUser_Join_Result newJoinRet = {};
			strcpy(newJoinRet.ip, newJoin->ip);
			newJoinRet.port = newJoin->port;
			send(clientSocket, (const char*)&newJoinRet, sizeof(NewUser_Join_Result), 0);//响应消息
			return;
		}
		case CMD_EXIT: {
			Exit* exit = (Exit*)header;
			std::cout << "客户端退出" << std::endl;
			return;
		}
		default:
		{
			std::cout << "服务器收到未定义消息" << std::endl;
			break;
		}
		}
	}

	//是否工作中
	bool isRun()
	{
		return this->_serverSocket != INVALID_SOCKET;
	}
	//子细胞线程运行内容
	void onRun()
	{
		while (isRun())
		{
			//如果客户端缓冲区队列有客户端加入 
			if (this->_clientsBuf.size() > 0)
			{
				//锁警卫 自动解锁
				std::lock_guard<std::mutex> lock(this->_mutex);				//调用析构函数应该是大括号{}结束的时候
				for (auto client : this->_clientsBuf)
				{
					//将客户端加入正式队列
					this->_clients.push_back(client);
					//std::cout << "客户端加入子线程正式队列" << std::endl;
				}
				//清除缓冲区队列
				this->_clientsBuf.clear();
			}
			//如果没有需要处理的客户端 跳过 不向下执行
			if (this->_clients.empty())
			{
				//表示当前线程休眠一段时间，休眠期间不与其他线程竞争cpu
				std::chrono::microseconds timeSleep(100);			//100毫秒
				std::this_thread::sleep_for(timeSleep);
				//std::cout << "无客户端加入该子细胞线程，休眠100毫秒" << std::endl;
				continue;
			}
			fd_set readfds = {};
			fd_set writefds = {};
			fd_set exceptfds = {};
			//最后一个prama:select的阻塞时间，若为Null则select一直阻塞直到有时间发生
			//清空fdset中所有文件描述符
			FD_ZERO(&readfds);
			FD_ZERO(&writefds);
			FD_ZERO(&exceptfds);
			//添加文件描述符fd到fdset集合中
			FD_SET(_serverSocket, &readfds);
			FD_SET(_serverSocket, &writefds);
			FD_SET(_serverSocket, &exceptfds);

			//先定义首个客户端socket描述符最大
			SOCKET maxSocket = _clients[0]->socketfd();
			//把新客户端socket加入可写集合
			for (int i = _clients.size() - 1; i >= 0; i--)
			{
				FD_SET(_clients[i]->socketfd(), &readfds);
				if (maxSocket < _clients[i]->socketfd())
				{
					maxSocket = _clients[i]->socketfd();
				}
			}
			//最后一个参数：
			timeval time_out = { 1,0 };
			int ret = select(maxSocket + 1, &readfds, &writefds, &exceptfds, &time_out);
			//std::cout << "服务器子线程select客户端数ret=" << ret << std::endl;
			if (ret <= 0)
			{
				//std::cout << "客户端全部退出....select任务结束";
				return;
			}
			//子线程 客户端正式vector队列 接受数据
			for (int i = _clients.size() - 1; i >= 0; i--)
			{
				//如果可读数组里面有可读的socket数据（fd_set是一个长度为64的数组）
				if (FD_ISSET(_clients[i]->socketfd(), &readfds))
				{
					if (-1 == recvData(_clients[i]))//有客户端退出
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

	//查询哪个子线程队列客户端数量最少
	size_t getClientCount()
	{
		return this->_clients.size() + this->_clientsBuf.size();
	}
	//线程析构关闭
	void Close()
	{
		if (INVALID_SOCKET != _serverSocket)
		{
#ifdef _WIN32
			//关闭所有客户端socket
			for (int i = _clients.size() - 1; i >= 0; i--) {
				closesocket(_clients[i]->socketfd());
				if (NULL != _clientsBuf[i])	delete _clientsBuf[i];
				if (NULL != _clients[i])	delete _clients[i];
			}
			//关闭服务器socket
			closesocket(_serverSocket);
			//清除WinSock环境 
			WSACleanup();
#else
			//关闭所有客户端socket
			for (int i = _clients.size() - 1; i >= 0; i--) {
				close(_clients[i]->socketfd());
				if (NULL != _clientsBuf[i])	delete _clientsBuf[i];
				if (NULL != _clients[i])	delete _clients[i];
			}
			//关闭socket
			close(_serverSocket);
#endif
			//清理数组
			_clients.clear();
			//最后再置为无效
			_serverSocket = INVALID_SOCKET;
		}
	}
	//启动线程
	void Start()
	{
		this->_thread = new std::thread(&CellServer::onRun, this);
	}
	//添加客户端
	void addClient(ClientSocket* client)
	{
		std::lock_guard<std::mutex> lock(this->_mutex);
		//this->_mutex.lock();
		//cout << "客户端加入子线程缓冲区队列" << endl;
		this->_clientsBuf.push_back(client);

		//this->_mutex.unlock();
	}
};