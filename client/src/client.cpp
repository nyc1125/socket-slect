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
		cout << "����������......" << endl;
		cin >> buf;
		if (0 == strcmp(buf, "exit")) {
			cout << "�˳��ͻ����߳�" << endl;
			isRun = false;
			return;
		}
		if (0 == strcmp(buf, "login")) {
			//�������������Ϣ
			Login login;
			login.cmd = CMD_LOGIN;
			login.dataLength = sizeof(Login);
			strcpy_s(login.userName, "���˺�");
			strcpy_s(login.password, "123456");
			send(cliSocket, (const char*)&login, sizeof(Login), 0);
		}
		else if ((0 == strcmp(buf, "logout"))) {
			//���������������
			Logout logout = {};//�ǳ�������
			logout.cmd = CMD_LOGOUT;
			logout.dataLength = sizeof(Logout);
			strcpy_s(logout.userName, "zsh");
			send(cliSocket, (const char*)&logout, sizeof(Logout), 0);
		}
		else
		{
			//���������������
			Error error = {};
			strcpy_s(error.userName, "zsh");
			send(cliSocket, (const char*)&error, sizeof(Error), 0);
		}
	}
}

int main() {

	init_Socket();
	//�����ͻ���socket
	SOCKET cliSocket = creatClientSocket("127.0.0.1");
	if (false == cliSocket) {
		cout << "�ͻ��˴���ʧ��" << endl;
		return 0;
	}
	//���ӷ�����
	sockaddr_in socket_in = {};
	int len = sizeof(socket_in);
	SOCKET serSocket = connect(cliSocket, (const sockaddr*)&socket_in, len);

	/*thread t1(cliThread, cliSocket);
	t1.detach();*/
	//��������˷���Ϣ
	
	while (isRun) {
		fd_set readfds = {};
		fd_set writefds = {};
		fd_set exceptfds = {};
		//���һ��prama:select������ʱ�䣬��ΪNull��selectһֱ����ֱ����ʱ�䷢��
		//���fdset�������ļ�������
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		FD_ZERO(&exceptfds);
		//����ļ�������fd��fdset������
		FD_SET(cliSocket, &readfds);
		FD_SET(cliSocket, &writefds);
		FD_SET(cliSocket, &exceptfds);

		//���¿ͻ���socket����ɶ�����
		FD_SET(cliSocket, &readfds);

		//���һ��������
		timeval time_out = { 1,0 };
		int ret = select(cliSocket + 1, &readfds, 0, 0, &time_out);
		if (ret < 0)
		{
			cout << "�ͻ���select����" << endl;
			break;
		}
		if (FD_ISSET(cliSocket, &readfds)) {
			FD_CLR(cliSocket, &readfds);
			if (-1 == processor(cliSocket)) {
				cout << "select�������" << endl;
				break;
			}
		}
		cout << "����ʱ�䴦������ҵ��...." << endl;
	}
	closesocket(cliSocket);
	close_Socket();
	cout << "�ɹ��˳�\n";

	system("pause");
	return 0;
}