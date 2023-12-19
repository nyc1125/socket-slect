#include"ser_tcpSocket.h"
#include<vector>
vector<SOCKET> v_cliSocket;
using namespace std;

int main() {

	init_Socket();
	//������������socket
	SOCKET serSocket=creatServerSocket();
	cout << "�ȴ��ͻ�������..." << endl;
	cout<<"---------------------------------------"<<endl;

	while (true) {

		fd_set readfds = {};
		fd_set writefds = {};
		fd_set exceptfds = {};
		//���һ��prama:select������ʱ�䣬��ΪNull��selectһֱ����ֱ����ʱ�䷢��
		//���fdset�������ļ�������
		FD_ZERO(&readfds); 
		FD_ZERO(&writefds); 
		FD_ZERO(&exceptfds);
		//����ļ�������fd��fdset������
		FD_SET(serSocket,&readfds); 
		FD_SET(serSocket, &writefds);
		FD_SET(serSocket, &exceptfds);

		//���¿ͻ���socket�����д����
		for (int i = v_cliSocket.size()-1; i >=0; i--) {
			FD_SET(v_cliSocket[i], &readfds);
		}

		//���һ��������
		timeval time_out = { 1,0 };
		int ret=select(serSocket + 1, &readfds, &writefds, &exceptfds, &time_out);
		if (ret < 0) {
			cout << "select���ִ���\n";
			break;
		}
		

		if (FD_ISSET(serSocket, &readfds)) {//����пɶ�socket
			FD_CLR(serSocket, &readfds);
			// �����¿ͻ�������
			sockaddr_in socket_in = {};
			int Len = sizeof(sockaddr_in);
			char* ip = {};
			int port = {};
			SOCKET cliSocket = accept(serSocket, (sockaddr*)&socket_in, &Len);

			//�ͻ��˵�ip�Ͷ˿�
			ip = inet_ntoa(socket_in.sin_addr);
			port = socket_in.sin_port;

			if (SOCKET_ERROR == cliSocket) {
				cout << "���ܵ���Ч�ͻ���..." << endl;
			}
			else {
				//���û����룬Ⱥ�������пͻ���
				NewUser_Join join;
				strcpy_s(join.ip, ip);
				join.port = port;
				for (int i = v_cliSocket.size() - 1; i >= 0; i--) {
					send(v_cliSocket[i], (const char*)&join, sizeof(NewUser_Join), 0);
				}
				//���¿ͻ��˼���vector������
				v_cliSocket.push_back(cliSocket);
				cout << "�¿ͻ��ˣ�" << ip << " " << port << "  ����" << endl;
			}
		}

		//�¿ͻ��˼��봦�����
		for (int i = 0; i < readfds.fd_count; i++) {
			if (-1 == processor(readfds.fd_array[i])) {
				auto iter = find(v_cliSocket.begin(), v_cliSocket.end(), readfds.fd_array[i]);
				if (iter != v_cliSocket.end()) {
					v_cliSocket.erase(iter);
				}
			}
		}
		/*cout << "����ʱ�䴦������ҵ��...." << endl;*/
	}
	//�ر�����socket
	for (int i = 0; i < v_cliSocket.size(); i++) {
		closesocket(v_cliSocket[i]);
	}
	
	close_Socket();
	cout << "�������ѹر�" << endl;
	system("pause");
	return 0;
}