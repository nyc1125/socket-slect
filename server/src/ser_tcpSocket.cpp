#include "ser_tcpSocket.h"
#include"Data_Server.h"

bool init_Socket()
{
	//wsa  windows socket async windows异步套接字
	//parm1:请求socket的版本号 2.2 2.1 2.0   parm2:传出参数 
	WSADATA wsadata;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsadata)) {
		cout << "服务器socket启动失败" << WSAGetLastError() << endl;
		return false;
	}
	return true;
}

bool close_Socket()
{
	if (0 != WSACleanup()) {
		cout << "服务器socket关闭失败" << WSAGetLastError() << endl;
		return false;
	}
	return true;
}

SOCKET creatServerSocket()
{
	//1、创建空Socket
	//parm1:af 地址协议族 ipv4 ipv6
	//parm2:type 传输协议类型 流式套接字 数据包
	//parm3:protocl 使用具体的某个传输协议
	SOCKET serSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serSocket == INVALID_SOCKET) {
		/*err("socket");*/
		cout << "服务器socket创建失败" << endl;
		return INVALID_SOCKET;
	}
	//2、绑定接受客户端连接的端口
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = 8888;
	addr.sin_addr.S_un.S_addr = ADDR_ANY;//inet_addr(ip);
	if (INVALID_SOCKET == bind(serSocket, (sockaddr*)&addr, sizeof(addr)))
	{
		cout << "服务器绑定端口失败" << endl;
	}
	else {
		cout << "服务器绑定端口成功" << endl;
	}
	//监听网络端口
	if (SOCKET_ERROR == listen(serSocket, 10)) {
		cout << "监听网络端口失败" << endl;
	}
	else {
		cout << "监听网络端口成功" << endl;
	}
	return serSocket;
}

//处理进程
int processor(SOCKET cliSocket)
{

	DataHead _rcvheader = {};//接收数据头
	//接受客户端发送的请求
	recv(cliSocket, (char*)&_rcvheader, sizeof(DataHead), 0);
	if (_rcvheader.dataLength <= 0)
	{
		/*cout << "客户端" << inet_ntoa(socket_in.sin_addr) << " " << socket_in.sin_port << "已退出\n";*/
		return -1;//客户端退出返回-1
	}

	switch (_rcvheader.cmd)
	{
		case CMD_LOGIN:
		{
			//接受客户端发送的请求体
			Login login = {};
			recv(cliSocket, (char*)&login+sizeof(DataHead), sizeof(Login) - sizeof(DataHead), 0);
			cout << "-------------------------------------------------" << endl;
			cout << "请求命令：CMD_LOGIN"  << "   数据长度：" << login.dataLength << endl;
			cout << "请求用户名：" << login.userName<< endl;
			cout << "密      码：" << login.password << endl;
			cout << "-------------------------------------------------" << endl;

			//判断账户名密码是否正确serSocket
			//此处判断逻辑

			//响应客户端请求
			LoginResult loginResult = {};
			loginResult.result = 1;
			loginResult.cmd = CMD_LOGIN_RESULT;//201
			loginResult.dataLength = sizeof(LoginResult);//32
			send(cliSocket, (const char*)&loginResult, sizeof(loginResult), 0);//响应消息
			return 1;
		}

		case CMD_LOGOUT:
		{
			//接受客户端发送的请求
			Logout logout = {};
			recv(cliSocket, (char*)&logout+ sizeof(DataHead), sizeof(Logout)- sizeof(DataHead), 0);
			cout << "-------------------------------------------------" << endl;
			cout << "请求命令：CMD_LOGOUT"  << "   数据长度：" << _rcvheader.dataLength << endl;
			cout << "请求用户名：" << logout.userName << endl;
			cout << "-------------------------------------------------" << endl;

			//响应客户端请求
			LogoutResult logoutResult = {};
			logoutResult.result = 1;
			logoutResult.cmd = CMD_LOGOUT_RESULT;
			logoutResult.dataLength = sizeof(LogoutResult);
			send(cliSocket, (const char*)&logoutResult, sizeof(logoutResult), 0);//响应消息
			return 1;
		}
		case CMD_ERROR: {
			//接受客户端发送的请求
			Error error = {};
			recv(cliSocket, (char*)&error+ sizeof(DataHead), sizeof(Error)- sizeof(DataHead), 0);
			cout << "-------------------------------------------------" << endl;
			cout << "请求命令：CMD_ERROR" << _rcvheader.cmd << "   数据长度：" << _rcvheader.dataLength << endl;
			cout << "请求用户名：" << error.userName << endl;
			cout << "-------------------------------------------------" << endl;

			ErrorResult errorResult = {};
			errorResult.result;
			errorResult.cmd = CMD_ERROR;
			errorResult.dataLength = sizeof(ErrorResult);
			send(cliSocket, (const char*)&errorResult, sizeof(errorResult), 0);//响应消息
			return 1;
		}

		default:
		{
			//接受客户端发送的请求
			Error error = {};
			recv(cliSocket, (char*)&error+ sizeof(DataHead), sizeof(Error)- sizeof(DataHead), 0);
			cout << "-------------------------------------------------" << endl;
			/*cout << "客户端" << inet_ntoa(socket_in.sin_addr) << " " << socket_in.sin_port << "发来请求" << endl;*/
			cout << "请求命令：CMD_ERROR" << _rcvheader.cmd << "   数据长度：" << _rcvheader.dataLength << endl;
			cout << "请求用户名：" << error.userName << endl;
			cout << "-------------------------------------------------" << endl;

			ErrorResult errorResult = {};
			errorResult.result;
			errorResult.cmd = CMD_ERROR;
			errorResult.dataLength = sizeof(ErrorResult);
			send(cliSocket, (const char*)&errorResult, sizeof(errorResult), 0);//响应消息
			return 1;
		}

	}
}

