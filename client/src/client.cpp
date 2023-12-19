#include<iostream>
#include"client.hpp"
#include<thread>
#include<mutex>
#include"CELLTimesTamp.hpp"
using namespace std;
//�ͻ���UI�߳�
void cliThread(Client *cliSocket) {
	while (true) {
		char buf[1024] = {};
		std::cout << "����������......" << endl;
		cin >> buf;
		if (0 == strcmp(buf, "exit")) {
			Exit *exit=NULL;
			cliSocket->sendMessage(exit,sizeof(exit));
			cliSocket->Close();
			cout << "�˳��ͻ����߳�" << endl;
			return;
		}
		if (0 == strcmp(buf, "login")) {
			//�������������Ϣ ֻ��������Ϣ�壨�̳�����Ϣͷ�����ǳ���ֻ����Ϣ����ô����
			Login login;
			strcpy(login.userName, "���˺�");
			strcpy(login.password, "1233211234567");
			cliSocket->sendMessage(&login,sizeof(login));	
		}
		else if ((0 == strcmp(buf, "logout"))) {
			//���������������
			Logout logout ;//�ǳ�������
			strcpy(logout.userName, "���˺�");
			cliSocket->sendMessage(&logout,sizeof(logout));
		}
		else
		{
			cout << "�����������������......" << endl;
		}
	}
}

//�ͻ��������������
const int clientMaxSize = 4000;
//�����ͻ��˼���
Client* clients[clientMaxSize];
//�߳�����
const int threadCount = 4;

//�߳��Ƿ�����
bool isRun = true;
//�����߳�
void sendThread(int id)
{
	//�̷߳���
	int c = clientMaxSize / threadCount;
	int begin = (id - 1) * c;
	int end = id * c;

	//�����ͻ���
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
	cout << "�ͻ����������" << endl;

	Login login;
	strcpy_s(login.userName, "���˺�");
	strcpy_s(login.password, "123456");
	const int nLen = sizeof(login);

	//��������
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
	//UI�߳�
	/*thread t1(cliThread, &clients);
	t1.detach();*/

	//���������߳�
	for (int i =0; i < threadCount; i++)
	{
		thread tSend(sendThread,i+1);
		if (true == tSend.joinable())
		{
			tSend.detach();
			cout << "�����߳�:" << i + 1 << endl;
		}
	}

	//���߳��ڴ�����ס
	while (1);
	
	//��ն���
	for (int i = 0; i < clientMaxSize; i++)
	{
		delete clients[i];
	}
	cout << "�˳�������" << endl;
	system("pause");
	return 0;
}


