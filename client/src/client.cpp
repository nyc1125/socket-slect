#include<iostream>
#include"client.hpp"
#include<thread>
#include<mutex>
#include"CELLTimesTamp.hpp"
using namespace std;
//客户端UI线程
void cliThread(Client *cliSocket) {
	while (true) {
		char buf[1024] = {};
		std::cout << "请输入命令......" << endl;
		cin >> buf;
		if (0 == strcmp(buf, "exit")) {
			Exit *exit=NULL;
			cliSocket->sendMessage(exit,sizeof(exit));
			cliSocket->Close();
			cout << "退出客户端线程" << endl;
			return;
		}
		if (0 == strcmp(buf, "login")) {
			//向服务器发送消息 只发送了消息体（继承了消息头，但是长度只有消息体那么长）
			Login login;
			strcpy(login.userName, "张仕豪");
			strcpy(login.password, "1233211234567");
			cliSocket->sendMessage(&login,sizeof(login));	
		}
		else if ((0 == strcmp(buf, "logout"))) {
			//向服务器发送数据
			Logout logout ;//登出请求体
			strcpy(logout.userName, "张仕豪");
			cliSocket->sendMessage(&logout,sizeof(logout));
		}
		else
		{
			cout << "命令错误，请重新输入......" << endl;
		}
	}
}

//客户端最大连接数量
const int clientMaxSize = 4000;
//创建客户端集合
Client* clients[clientMaxSize];
//线程数量
const int threadCount = 4;

//线程是否运行
bool isRun = true;
//发送线程
void sendThread(int id)
{
	//线程分配
	int c = clientMaxSize / threadCount;
	int begin = (id - 1) * c;
	int end = id * c;

	//创建客户端
	for (int i = begin; i < end; i++)
	{
		if (!isRun)
		{
			return;
		}
		clients[i] = new Client();
	}
	for (int i = begin; i < end; i++)
	{
		if (!isRun)
		{
			return;
		}
		int ret = clients[i]->connectServer("127.0.0.1", 88);
		if (SOCKET_ERROR == ret)
		{
			continue;
		}
	}
	cout << "客户端连接完毕" << endl;

	Login login;
	strcpy_s(login.userName, "张仕豪");
	strcpy_s(login.password, "123456");
	const int nLen = sizeof(login);

	//发送数据
	while (isRun)
	{
		for (int i = begin; i < end; i++)
		{
			clients[i]->sendMessage(&login, nLen);
		}
	}

	for (int i = 0; i < clientMaxSize - 1; i++)
	{
		clients[i]->Close();
	}
}
int main() 
{
	//UI线程
	/*thread t1(cliThread, &clients);
	t1.detach();*/

	//启动发送线程
	for (int i =0; i < threadCount; i++)
	{
		thread tSend(sendThread,i+1);
		if (true == tSend.joinable())
		{
			tSend.detach();
			cout << "启动线程:" << i + 1 << endl;
		}
	}

	//主线程在此阻塞住
	while (1);
	
	//清空堆区
	for (int i = 0; i < clientMaxSize; i++)
	{
		delete clients[i];
	}
	cout << "退出主程序" << endl;
	system("pause");
	return 0;
}


