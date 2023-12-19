#include"cli_tcpSocket.h"
#include"Data_Client.h"
#include"CMD_Client.h"
#include<thread>
#include<iostream>
using namespace std;

bool isRun = true;
void cliThread(SOCKET cliSocket) {
	while (true) {

		char buf[1024];
		cout << "请输入命令......" << endl;
		cin >> buf;
		if (0 == strcmp(buf, "exit")) {
			cout << "退出客户端线程" << endl;
			isRun = false;
			return;
		}
		if (0 == strcmp(buf, "login")) {
			//向服务器发送消息
			Login login;
			login.cmd = CMD_LOGIN;
			login.dataLength = sizeof(Login);
			strcpy_s(login.userName, "张仕豪");
			strcpy_s(login.password, "123456");
			send(cliSocket, (const char*)&login, sizeof(Login), 0);
		}
		else if ((0 == strcmp(buf, "logout"))) {
			//向服务器发送数据
			Logout logout = {};//登出请求体
			logout.cmd = CMD_LOGOUT;
			logout.dataLength = sizeof(Logout);
			strcpy_s(logout.userName, "zsh");
			send(cliSocket, (const char*)&logout, sizeof(Logout), 0);
		}
		else
		{
			//向服务器发送数据
			Error error = {};
			strcpy_s(error.userName, "zsh");
			send(cliSocket, (const char*)&error, sizeof(Error), 0);
		}
	}
}

int main() {

	init_Socket();
	//创建客户端socket
	SOCKET cliSocket = creatClientSocket("127.0.0.1");
	if (false == cliSocket) {
		cout << "客户端创建失败" << endl;
		return 0;
	}
	//连接服务器
	sockaddr_in socket_in = {};
	int len = sizeof(socket_in);
	SOCKET serSocket = connect(cliSocket, (const sockaddr*)&socket_in, len);

	/*thread t1(cliThread, cliSocket);
	t1.detach();*/
	//向服务器端发信息
	
	while (isRun) {
		fd_set readfds = {};
		fd_set writefds = {};
		fd_set exceptfds = {};
		//最后一个prama:select的阻塞时间，若为Null则select一直阻塞直到有时间发生
		//清空fdset中所有文件描述符
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		FD_ZERO(&exceptfds);
		//添加文件描述符fd到fdset集合中
		FD_SET(cliSocket, &readfds);
		FD_SET(cliSocket, &writefds);
		FD_SET(cliSocket, &exceptfds);

		//把新客户端socket加入可读集合
		FD_SET(cliSocket, &readfds);

		//最后一个参数：
		timeval time_out = { 1,0 };
		int ret = select(cliSocket + 1, &readfds, 0, 0, &time_out);
		if (ret < 0)
		{
			cout << "客户端select出错" << endl;
			break;
		}
		if (FD_ISSET(cliSocket, &readfds)) {
			FD_CLR(cliSocket, &readfds);
			if (-1 == processor(cliSocket)) {
				cout << "select任务结束" << endl;
				break;
			}
		}
		cout << "空闲时间处理其他业务...." << endl;
	}
	closesocket(cliSocket);
	close_Socket();
	cout << "成功退出\n";

	system("pause");
	return 0;
}