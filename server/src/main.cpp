#include<iostream>
#include"server.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS

int main() {
	//����һ���ͻ���
	Server server;
	//�ͻ��˳�ʼ��������������server��socket
	server.Init();
	//��ip�Ͷ˿�
	int ret=server.Bind(nullptr,88);
	//�ɹ��򷵻�0 ��ʧ�ܷ���-1
	if(ret<0)
	{
		return 0;
	}
	//���������������˿�
	server.Listen(SOMAXCONN);
	//������������ϸ���߳� ��������ϸ���߳�
	server.Start();

	//���߳�����
	//std::cout << "���������߳̿�ʼ����" << std::endl;
	while (server.isRun())
	{
		//���߳����н��ܿͻ�������
		server.onRun();
		//std::cout << "����ʱ�䴦������ҵ��" << std::endl;
	}
	std::cout << "�������ѹر�" << std::endl;
	system("pause");
	return 0;
}