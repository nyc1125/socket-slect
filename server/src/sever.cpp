#include"ser_tcpSocket.h"
#include<vector>
vector<SOCKET> v_cliSocket;
using namespace std;

int main() {

	init_Socket();
	//创建服务器端socket
	SOCKET serSocket=creatServerSocket();
	cout << "等待客户端连接..." << endl;
	cout<<"---------------------------------------"<<endl;

	while (true) {

		fd_set readfds = {};
		fd_set writefds = {};
		fd_set exceptfds = {};
		//最后一个prama:select的阻塞时间，若为Null则select一直阻塞直到有时间发生
		//清空fdset中所有文件描述符
		FD_ZERO(&readfds); 
		FD_ZERO(&writefds); 
		FD_ZERO(&exceptfds);
		//添加文件描述符fd到fdset集合中
		FD_SET(serSocket,&readfds); 
		FD_SET(serSocket, &writefds);
		FD_SET(serSocket, &exceptfds);

		//把新客户端socket加入可写集合
		for (int i = v_cliSocket.size()-1; i >=0; i--) {
			FD_SET(v_cliSocket[i], &readfds);
		}

		//最后一个参数：
		timeval time_out = { 1,0 };
		int ret=select(serSocket + 1, &readfds, &writefds, &exceptfds, &time_out);
		if (ret < 0) {
			cout << "select出现错误\n";
			break;
		}
		

		if (FD_ISSET(serSocket, &readfds)) {//如果有可读socket
			FD_CLR(serSocket, &readfds);
			// 接受新客户端连接
			sockaddr_in socket_in = {};
			int Len = sizeof(sockaddr_in);
			char* ip = {};
			int port = {};
			SOCKET cliSocket = accept(serSocket, (sockaddr*)&socket_in, &Len);

			//客户端的ip和端口
			ip = inet_ntoa(socket_in.sin_addr);
			port = socket_in.sin_port;

			if (SOCKET_ERROR == cliSocket) {
				cout << "接受到无效客户端..." << endl;
			}
			else {
				//新用户加入，群发给所有客户端
				NewUser_Join join;
				strcpy_s(join.ip, ip);
				join.port = port;
				for (int i = v_cliSocket.size() - 1; i >= 0; i--) {
					send(v_cliSocket[i], (const char*)&join, sizeof(NewUser_Join), 0);
				}
				//把新客户端加入vector集合中
				v_cliSocket.push_back(cliSocket);
				cout << "新客户端：" << ip << " " << port << "  加入" << endl;
			}
		}

		//新客户端加入处理进程
		for (int i = 0; i < readfds.fd_count; i++) {
			if (-1 == processor(readfds.fd_array[i])) {
				auto iter = find(v_cliSocket.begin(), v_cliSocket.end(), readfds.fd_array[i]);
				if (iter != v_cliSocket.end()) {
					v_cliSocket.erase(iter);
				}
			}
		}
		/*cout << "空闲时间处理其他业务...." << endl;*/
	}
	//关闭所有socket
	for (int i = 0; i < v_cliSocket.size(); i++) {
		closesocket(v_cliSocket[i]);
	}
	
	close_Socket();
	cout << "服务器已关闭" << endl;
	system("pause");
	return 0;
}