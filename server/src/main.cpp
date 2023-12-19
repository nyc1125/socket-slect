#include<iostream>
#include"server.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS

int main() {
	//创建一个客户端
	Server server;
	//客户端初始化：创建服务器server的socket
	server.Init();
	//绑定ip和端口
	int ret=server.Bind(nullptr,88);
	//成功则返回0 ，失败返回-1
	if(ret<0)
	{
		return 0;
	}
	//开启服务器监听端口
	server.Listen(SOMAXCONN);
	//服务器创建子细胞线程 并开启子细胞线程
	server.Start();

	//主线程运行
	//std::cout << "服务器主线程开始运行" << std::endl;
	while (server.isRun())
	{
		//主线程运行接受客户端连接
		server.onRun();
		//std::cout << "空闲时间处理其他业务" << std::endl;
	}
	std::cout << "服务器已关闭" << std::endl;
	system("pause");
	return 0;
}