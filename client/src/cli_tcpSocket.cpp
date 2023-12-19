#include "cli_tcpSocket.h"
#include"Data_Client.h"

bool init_Socket()
{
	//wsa  windows socket async windows异步套接字
	//parm1:请求socket的版本号 2.2 2.1 2.0   parm2:传出参数 
	WSADATA wsadata;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsadata)) {
		cout << "WSAStartup failed code" << WSAGetLastError() << endl;
		return false;
	}
	return true;
}

bool close_Socket()
{
	if (0 != WSACleanup()) {
		cout << "WSACleanup failed code" << WSAGetLastError() << endl;
		return false;
	}
	return true;
	return false;
}

SOCKET creatClientSocket(const char* ip)
{
	//1、创建空Socket
	//parm1:af 地址协议族 ipv4 ipv6
	//parm2:type 传输协议类型 流式套接字 数据包
	//parm3:protocl 使用具体的某个传输协议
	SOCKET cliSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (cliSocket == INVALID_SOCKET) {
		cout << "客户端建立socket失败" << endl;
		return false;
	}
	//2、与服务器建立连接
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = 8888;
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (SOCKET_ERROR == connect(cliSocket, (sockaddr*)&addr, sizeof(sockaddr_in)))
	{
		cout << "客户端连接服务器失败" << endl;
		return false;
	}
	return cliSocket;
}


//客户端业务处理函数
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
		case CMD_LOGIN_RESULT:
		{
			//接受服务器响应数据
			LoginResult loginRet = {};//登录结果
			recv(cliSocket, (char*)&loginRet + sizeof(DataHead), sizeof(LoginResult) - sizeof(DataHead), 0);
			cout << "-------------------------------------------------" << endl;
			cout << "服务器响应：" << endl;
			cout << "响应头：" << loginRet.cmd << "   响应长度:" << loginRet.dataLength << endl;
			cout << "登录结果：" << loginRet.result << endl;
			cout << "-------------------------------------------------" << endl;
			return 1;
		}

		case CMD_LOGOUT_RESULT:
		{
			//接受服务器响应数据
			LogoutResult logoutRet = {};//登录结果
			recv(cliSocket, (char*)&logoutRet + sizeof(DataHead), sizeof(LogoutResult) - sizeof(DataHead), 0);
			cout << "-------------------------------------------------" << endl;
			cout << "服务器响应：" << endl;
			cout << "响应头：" << logoutRet.cmd << "   响应长度:" << logoutRet.dataLength << endl;
			cout << "登出结果：" << logoutRet.result << endl;
			cout << "-------------------------------------------------" << endl;
			return 1;
		}
		case CMD_ERROR_RESULT: {
			//接受服务器响应数据
			ErrorResult errorRet = {};//登录结果
			recv(cliSocket, (char*)&errorRet + sizeof(DataHead), sizeof(ErrorResult) - sizeof(DataHead), 0);
			cout << "-------------------------------------------------" << endl;
			cout << "服务器响应：" << endl;
			cout << "响应头：" << errorRet.cmd << "   响应长度:" << errorRet.dataLength << endl;
			cout << "错误命令结果：" << errorRet.result << endl;
			cout << "-------------------------------------------------" << endl;
			return 1;
		}
		case CMD_NEWUSER_JOIN: {
			//接受服务器响应数据
			NewUser_Join newJoin;//登录结果
			recv(cliSocket, (char*)&newJoin+sizeof(DataHead), sizeof(NewUser_Join)-sizeof(DataHead), 0);
			cout << "-------------------------------------------------" << endl;
			cout << "新用户加入到线程中" << endl;
			cout << "响应头：" << newJoin.cmd << "   响应长度:" << newJoin.dataLength << endl;
			cout << "新用户ip:" << newJoin.ip << "   port:" << newJoin.port << endl;
			cout << "-------------------------------------------------" << endl;
			return 1;
		}
		default:
		{
			//接受服务器响应数据
			ErrorResult errorRet = {};//登录结果
			recv(cliSocket, (char*)&errorRet + sizeof(DataHead), sizeof(ErrorResult) - sizeof(DataHead), 0);
			cout << "-------------------------------------------------" << endl;
			cout << "服务器响应：" << endl;
			cout << "响应头：" << errorRet.cmd << "   响应长度:" << errorRet.dataLength << endl;
			cout << "错误命令结果：" << errorRet.result << endl;
			cout << "-------------------------------------------------" << endl;
			return 1;
		}
	}
}
